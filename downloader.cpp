/*
* Quantum, A free and open-source download manager that is written in C++
* Copyright (C) 2025  Yovsky <Yovsky@proton.me>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "downloader.h"
#include <QtNetwork/QNetworkRequest>
#include <QUrl>
#include <QTimer>

Downloader::Downloader(QObject *parent) : QObject(parent)
{
    manager = new QNetworkAccessManager(this);
}

void Downloader::download(const QUrl &url, const QString &savePath, int chunkNumber)
{
    // file.setFileName(savePath);
    // SavePath = savePath;

    // if(!file.open(QIODevice::WriteOnly))
    // {
    //     emit downloadFinished(false, "File not downloaded, error: " + file.errorString());
    //     return;
    // }

    // QNetworkRequest request(url);
    // reply = manager->get(request);

    // connect(reply, &QIODevice::readyRead, this, &Downloader::onReadReady);
    // connect(reply, &QNetworkReply::finished, this, &Downloader::onDownloadFinished);
    // connect(reply, &QNetworkReply::downloadProgress, this, &Downloader::progressChanged);

    // emit downloadStarted();

    m_savePath = savePath;
    m_url = QUrl(url);
    m_chunkNumber = chunkNumber;
    m_chunksCompleted = 0;
    m_bytesDownloaded = 0;
    m_tempPaths.clear();

    QNetworkRequest request(m_url);
    QNetworkReply *reply = manager->head(request);
    connect (reply, &QNetworkReply::finished, this, &Downloader::onHeadFinished);
}

void Downloader::onHeadFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) return;
    reply->deleteLater();
    if (reply->error() != QNetworkReply::NoError)
    {
        qDebug() << "Failed to contact target host server:" << reply->errorString();
        return;
    }
    m_filesize = reply->header(QNetworkRequest::ContentLengthHeader).toLongLong();
    SetupWorkers();
}

void Downloader::SetupWorkers()
{
    qint64 chunkSize = m_filesize / m_chunkNumber;
    for (int i = 0; i < m_chunkNumber; i++)
    {
        qint64 start = i * chunkSize;
        qint64 end = (i == m_chunkNumber - 1) ? m_filesize - 1 : start + chunkSize - 1;

        QString tempPath = m_savePath + QString("qdm%1").arg(i);
        m_tempPaths.append(tempPath);
        QFile::remove(tempPath);

        QThread *workerThread = new QThread(this);
        DownloadWorker *worker = new DownloadWorker(m_url, start, end, tempPath);
        worker->moveToThread(workerThread);

        connect(workerThread, &QThread::started, worker, &::DownloadWorker::StartDownload);
        connect(worker, &::DownloadWorker::Finished, workerThread, &QThread::quit);
        connect(worker, &::DownloadWorker::Finished, worker, &DownloadWorker::deleteLater);
        connect(workerThread, &QThread::finished, workerThread, &QThread::deleteLater);

        connect(worker, &::DownloadWorker::Progress, this, &Downloader::onChunkProgress);
        connect(worker, &::DownloadWorker::Finished, this, &Downloader::onChunkFinished);

        workerThread->start();
    }
}

void Downloader::onChunkProgress(qint64 bytes) {
    m_bytesDownloaded += bytes;
    emit progressChanged(m_bytesDownloaded, m_filesize);
}

void Downloader::onChunkFinished() {
    m_chunksCompleted++;
    if (m_chunksCompleted == m_chunkNumber)
    {
        mergeTemporaryFiles();
    }
}

void Downloader::mergeTemporaryFiles() {
    QFile finalFile(m_savePath);
    if (!finalFile.open(QIODevice::WriteOnly))
    {
        qDebug() << "Cannot compile output file target assembly footprint.";
        return;
    }

    for (const QString &tempPath : m_tempPaths)
    {
        QFile tempFile(tempPath);
        if (tempFile.open(QIODevice::ReadOnly)) {
            finalFile.write(tempFile.readAll());
            tempFile.close();
            QFile::remove(tempPath);
        }
    }

    finalFile.close();
    qDebug() << "Download completed successfully!";
    emit downloadFinished(true, "Download completed successfully.");
}

void Downloader::onReadReady()
{
    if (reply->bytesAvailable() > 0)
    {
        QByteArray data = reply->readAll();
        currentSize += data.size();
        file.write(data);
    }
}

void Downloader::onDownloadFinished()
{
    QNetworkReply *senderReply = qobject_cast<QNetworkReply*>(sender());
    if (!senderReply) return;

    if (senderReply == reply) reply = nullptr;

    QNetworkReply::NetworkError err = senderReply->error();

    if (err == QNetworkReply::OperationCanceledError) {
        if (isPausing) {
            isPausing = false;
            file.flush();
            file.close();
        }
        senderReply->deleteLater();
        return;
    }

    if (err != QNetworkReply::NoError)
    {
        file.close();
        emit downloadFinished(false, "Error: " + senderReply->errorString());
    }
    else
    {
        if (senderReply->bytesAvailable() > 0)
            file.write(senderReply->readAll());

        file.close();
        emit downloadFinished(true, "Download completed successfully.");
    }

    senderReply->deleteLater();
}

void Downloader::downloadStop()
{
    isPausing = false;
    if (reply)
    {
        QNetworkReply *r = reply;
        reply = nullptr;
        r->abort();
        r->deleteLater();
    }
    if (file.isOpen()) file.close();
    file.remove();
}

void Downloader::downloadPause()
{
    if (!reply) return;
    isPausing = true;
    reply->abort();
}

void Downloader::downloadResume(const QUrl &url, const QString &savePath)
{
    file.setFileName(savePath);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Append)) {
        emit downloadFinished(false, "Can't open file for resume");
        return;
    }

    qint64 existingSize = file.size();
    currentSize = existingSize;

    QNetworkRequest request(url);
    QByteArray range = "bytes=" + QByteArray::number(existingSize) + "-";
    request.setRawHeader("Range", range);

    reply = manager->get(request);

    connect(reply, &QIODevice::readyRead, this, &Downloader::onReadReady);
    connect(reply, &QNetworkReply::finished, this, &Downloader::onDownloadFinished);
    connect(reply, &QNetworkReply::downloadProgress, this, &Downloader::progressChanged);

    emit downloadStarted();
}
