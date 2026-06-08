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

void Downloader::download(const QUrl &url, const QString &savePath, int chunkNumber, const QString &SHA256)
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
    m_SHA256 = SHA256;

    m_qdmTempDir = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/Quantum";
    m_downloadID = QUuid::createUuid().toString(QUuid::WithoutBraces);
    QDir dir;
    dir.mkpath(m_qdmTempDir + "/" + m_downloadID);

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
    if (m_filesize <= 0) m_chunkNumber = 1;
    if (m_filesize < 1024 * 1024 * 10) m_chunkNumber = 1;

    QByteArray acceptRanges = reply->rawHeader("Accept-Ranges");
    if (acceptRanges.toLower().contains("bytes") || m_chunkNumber == 1)
    {
        chunkProgress.resize(m_chunkNumber);
        WriteDownloadData();
        StartDataTimer();
        SetupWorkers();
    }
    else
    {
        QNetworkRequest request(m_url);
        QByteArray rangeHeader = "bytes=0-0";
        request.setRawHeader("Range", rangeHeader);
        QNetworkReply *test = manager->get(request);
        connect (test, &QNetworkReply::finished, this, &Downloader::onHeadTestFinished);
    }
}

void Downloader::onHeadTestFinished()
{
    QNetworkReply *test = qobject_cast<QNetworkReply*>(sender());
    if (!test) return;
    test->deleteLater();
    if (test->error() != QNetworkReply::NoError)
        m_chunkNumber = 1;
    else if (test->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() != 206)
        m_chunkNumber = 1;
    chunkProgress.resize(m_chunkNumber);
    WriteDownloadData();
    StartDataTimer();
    SetupWorkers();
}

void Downloader::WriteDownloadData()
{
    QFile dataFile(m_qdmTempDir + "/" + m_downloadID + "/" + QFileInfo(m_savePath).fileName() + ".qdmtemp");
    if (!dataFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        emit downloadFinished(false, "Failed to store download data.");
        return;
    }
    // QTextStream out(&dataFile);
    // out << "\"url\": \"" + m_url.toString() + "\",\n\"downloadID\": \"" + m_downloadID + "\",\n\"savePath\": \"" + m_savePath + "\", \n\"chunkCount\": " + QString::number(m_chunkNumber) + ",\n\"fileSize\": " + QString::number(m_filesize) << Qt::endl;

    // for (int i = 0; i < chunkProgress.size(); i++)
    // {
    //     out << "\n\"chunk[" + QString::number(i) + "]\": " + QString::number(chunkProgress[i]) << Qt::endl;
    // }

    QJsonObject root;
    root["url"] = m_url.toString();
    root["downloadID"] = m_downloadID;
    root["savePath"] = m_savePath;
    root["chunkCount"] = m_chunkNumber;
    root["fileSize"] = m_filesize;

    QJsonArray chunks;
    for (qint64 progress : chunkProgress)
    {
        chunks.append(progress);
    }
    root["chunks"] = chunks;
    QJsonDocument doc(root);
    dataFile.write(doc.toJson());

    dataFile.close();
    QFile::remove(m_qdmTempDir + "/" + m_downloadID + "/" + QFileInfo(m_savePath).fileName() + ".qdmdata");
    if (!dataFile.rename(m_qdmTempDir + "/" + m_downloadID + "/" + QFileInfo(m_savePath).fileName() + ".qdmdata"))
    {
        emit downloadFinished(false, "Failed to store download data.");
        return;
    }
}

void Downloader::StartDataTimer()
{
    if (!saveTimer)
    {
        saveTimer = new QTimer(this);
        connect(saveTimer, &QTimer::timeout, this, &Downloader::WriteDownloadData);
    }
    saveTimer->start(5000);
}

void Downloader::SetupWorkers()
{
    qint64 chunkSize = m_filesize / m_chunkNumber;
    for (int i = 0; i < m_chunkNumber; i++)
    {
        qint64 start = i * chunkSize;
        qint64 end = (i == m_chunkNumber - 1) ? m_filesize - 1 : start + chunkSize - 1;

        QString tempPath = m_qdmTempDir + "/" + m_downloadID + "/" + QString("chunk%1.qdm").arg(i);
        m_tempPaths.append(tempPath);
        if (!isResuming)
            QFile::remove(tempPath);

        QThread *workerThread = new QThread(this);
        DownloadWorker *worker = new DownloadWorker(m_url, i, start, end, tempPath);
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

void Downloader::onChunkProgress(int chunkIndex, qint64 bytes)
{
    m_bytesDownloaded += bytes;
    if (chunkIndex >= 0 && chunkIndex < chunkProgress.size())
        chunkProgress[chunkIndex] += bytes;
    emit progressChanged(m_bytesDownloaded, m_filesize);
}

void Downloader::onChunkFinished()
{
    m_chunksCompleted++;
    if (m_chunksCompleted == m_chunkNumber)
    {
        mergeTemporaryFiles();
    }
}

void Downloader::mergeTemporaryFiles()
{
    saveTimer->stop();
    QFile finalFile(m_savePath);
    if (!finalFile.open(QIODevice::WriteOnly))
    {
        for (const QString &tempPath : m_tempPaths)
        {
            QFile::remove(tempPath);
        }
        emit downloadFinished(false, "Cannot compile output file target assembly footprint.");
        return;
    }

    for (const QString &tempPath : m_tempPaths)
    {
        QFile tempFile(tempPath);
        if (!tempFile.open(QIODevice::ReadOnly))
        {
            emit downloadFinished(false, "Failed while merging chunks.");
            return;
        }
        finalFile.write(tempFile.readAll());
        tempFile.close();
        QFile::remove(tempPath);
    }
    finalFile.close();
    if (QFileInfo(finalFile).size() != m_filesize)
    {
        QFile::remove(m_savePath);
        emit downloadFinished(false, "File size mismatch.");
        return;
    }


    if (!m_SHA256.isEmpty())
    {
        QFile verFile(m_savePath);
        if (!verFile.open(QIODevice::ReadOnly))
        {
            emit downloadFinished(false, "Failed to verify file hash.");
            return;
        }

        QByteArray hash = QCryptographicHash::hash(verFile.readAll(), QCryptographicHash::Sha256);
        if (hash.toHex().toLower() != m_SHA256.toLower())
        {
            verFile.close();
            QFile::remove(m_savePath);
            emit downloadFinished(false, "File hash does not meet provided hash.");
            return;
        }
        verFile.close();
    }

    QDir tempDir(m_qdmTempDir + "/" + m_downloadID);
    tempDir.removeRecursively();
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

// ===========================================================

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
