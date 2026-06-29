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

void Downloader::download(downloadInformations Info)
{
    info = Info;
    m_chunksCompleted = 0;
    m_bytesDownloaded = 0;
    m_tempPaths.clear();
    m_url = QUrl(Info.url);

    m_qdmTempDir = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/Quantum";
    info.ID = QUuid::createUuid().toString(QUuid::WithoutBraces);
    QDir dir;
    dir.mkpath(m_qdmTempDir + "/" + info.ID);

    info.tempPath = m_qdmTempDir + "/" + info.fileName + ".qdm";
    m_file.setFileName(info.tempPath);
    m_file.resize(info.fileByteSize);

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
    info.fileByteSize = reply->header(QNetworkRequest::ContentLengthHeader).toLongLong();
    if (info.fileByteSize < 1024 * 1024 * 10) info.chunkCount = 1;

    QByteArray acceptRanges = reply->rawHeader("Accept-Ranges");
    if (acceptRanges.toLower().contains("bytes") || info.chunkCount == 1)
    {
        qDebug() << "chunk count :" + QString::number(info.chunkCount);
        chunkProgress.resize(info.chunkCount);
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
        info.chunkCount = 1;
    else if (test->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() != 206)
        info.chunkCount = 1;
    chunkProgress.resize(info.chunkCount);
    WriteDownloadData();
    StartDataTimer();
    SetupWorkers();
}

void Downloader::WriteDownloadData()
{
    QFile dataFile(m_qdmTempDir + "/" + info.ID + "/" + QFileInfo(info.savePath).fileName() + ".qdmtemp");
    if (!dataFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        emit downloadFinished(false, "Failed to store download data.");
        return;
    }

    QJsonObject root;
    root["url"] = m_url.toString();
    root["downloadID"] = info.ID;
    root["savePath"] = info.savePath;
    root["chunkCount"] = info.chunkCount;
    root["fileSize"] = info.fileByteSize;

    QJsonArray chunks;
    for (qint64 progress : chunkProgress)
    {
        chunks.append(progress);
    }
    root["chunks"] = chunks;
    QJsonDocument doc(root);
    dataFile.write(doc.toJson());

    // Used .qdmtemp then renamed it to ensure .qdmdata is created successfully and is not corrupted.
    dataFile.close();
    QFile::remove(m_qdmTempDir + "/" + info.ID + "/" + QFileInfo(info.savePath).fileName() + ".qdmdata");
    if (!dataFile.rename(m_qdmTempDir + "/" + info.ID + "/" + QFileInfo(info.savePath).fileName() + ".qdmdata"))
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
    qint64 chunkSize = info.fileByteSize / info.chunkCount;
    for (int i = 0; i < info.chunkCount; i++)
    {
        qint64 start = i * chunkSize;
        qint64 end = (i == info.chunkCount - 1) ? info.fileByteSize - 1 : start + chunkSize - 1;

        QString tempPath = m_qdmTempDir + "/" + info.ID + "/" + QString("chunk%1.qdm").arg(i);
        m_tempPaths.append(tempPath);
        if (!isResuming)
            QFile::remove(tempPath);

        QThread *workerThread = new QThread(this);
        DownloadWorker *worker = new DownloadWorker(m_url, i, start, end, tempPath, false, info);
        worker->moveToThread(workerThread);

        m_workers.append(worker);
        m_workerThreads.append(workerThread);

        connect(workerThread, &QThread::started, worker, &::DownloadWorker::StartDownload);
        connect(worker, &::DownloadWorker::Finished, workerThread, &QThread::quit);
        connect(worker, &::DownloadWorker::Finished, worker, &DownloadWorker::deleteLater);
        connect(workerThread, &QThread::finished, workerThread, &QThread::deleteLater);

        connect(worker, &::DownloadWorker::Progress, this, &Downloader::onChunkProgress);
        connect(worker, &::DownloadWorker::Finished, this, &Downloader::onChunkFinished);

        connect(worker, &DownloadWorker::Finished, this, [this, worker, workerThread]() {
            m_workers.removeOne(worker);
            m_workerThreads.removeOne(workerThread);
        });

        workerThread->start();
    }
}

void Downloader::onChunkProgress(int chunkIndex, qint64 bytes)
{
    m_bytesDownloaded += bytes;
    if (chunkIndex >= 0 && chunkIndex < chunkProgress.size())
        chunkProgress[chunkIndex] += bytes;
    emit progressChanged(m_bytesDownloaded, info.fileByteSize);
}

void Downloader::onChunkFinished()
{
    m_chunksCompleted++;
    if (isPausing) return;
    if (m_chunksCompleted == info.chunkCount)
    {
        handleDownloadFinish();
    }
}

void Downloader::handleDownloadFinish()
{
    if (saveTimer) saveTimer->stop();

    m_file.rename(info.savePath);

    if (!info.SHA256.isEmpty())
    {
        QFile verFile(info.savePath);
        if (!verFile.open(QIODevice::ReadOnly))
        {
            emit downloadFinished(false, "Failed to verify file hash.");
            return;
        }

        QByteArray hash = QCryptographicHash::hash(verFile.readAll(), QCryptographicHash::Sha256);
        if (hash.toHex().toLower() != info.SHA256.toLower())
        {
            verFile.close();
            QFile::remove(info.savePath);
            emit downloadFinished(false, "File hash does not meet provided hash.");
            return;
        }
        verFile.close();
    }

    QDir tempDir(m_qdmTempDir + "/" + info.fileName + ".qdm");
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

void Downloader::downloadResume(downloadInformations Info)
{
    info = Info;
    isPausing = false;
    m_workers.clear();
    m_workerThreads.clear();
    m_url = QUrl(info.url);
    m_chunksCompleted = 0;
    m_tempPaths.clear();
    m_qdmTempDir = QStandardPaths::writableLocation(QStandardPaths::TempLocation)
                   + "/Quantum";
    isResuming = true;

    m_bytesDownloaded = 0;
    chunkProgress.resize(info.chunkCount);

    QNetworkRequest req(m_url);
    QNetworkReply *headReply = manager->head(req);
    connect(headReply, &QNetworkReply::finished, this, [this, headReply, Info]() mutable {
    headReply->deleteLater();
    qint64 freshSize = headReply->header(QNetworkRequest::ContentLengthHeader).toLongLong();
    if (freshSize > 0 && freshSize != info.fileByteSize)
    {
        qDebug() << "File size changed on server! Was:" << info.fileByteSize << "Now:" << freshSize;
        emit downloadFinished(false, "File changed on server, cannot resume.");
        return;
    }

    StartDataTimer();

    qint64 chunkSize = info.fileByteSize / info.chunkCount;
    for (int i = 0; i < info.chunkCount; i++)
    {
        QString tempPath = m_qdmTempDir + "/" + info.ID + "/" + QString("chunk%1.qdm").arg(i);
        m_tempPaths.append(tempPath);

        qint64 actualBytesOnDisk = QFileInfo(tempPath).exists() ? QFileInfo(tempPath).size() : 0;
        chunkProgress[i] = actualBytesOnDisk;
        m_bytesDownloaded += actualBytesOnDisk;

        qint64 chunkEnd = (i == info.chunkCount - 1) ? info.fileByteSize - 1 : (i + 1) * chunkSize - 1;
        qint64 expectedChunkSize = chunkEnd - i * chunkSize + 1;

        if (actualBytesOnDisk >= expectedChunkSize)
        {
            m_chunksCompleted++;
            continue;
        }

        qint64 start = i * chunkSize + actualBytesOnDisk;
        qint64 end = chunkEnd;

        qDebug() << tempPath << "size:" << actualBytesOnDisk
                 << "start:" << start << "end:" << end;

        QThread *workerThread = new QThread(this);
        DownloadWorker *worker = new DownloadWorker(info.url, i, start, end, tempPath, true, info);
        worker->moveToThread(workerThread);

        m_workers.append(worker);
        m_workerThreads.append(workerThread);

        connect(workerThread, &QThread::started, worker, &DownloadWorker::StartDownload);
        connect(worker, &DownloadWorker::Finished, workerThread, &QThread::quit);
        connect(worker, &DownloadWorker::Finished, worker, &DownloadWorker::deleteLater);
        connect(workerThread, &QThread::finished, workerThread, &QThread::deleteLater);
        connect(worker, &DownloadWorker::Progress, this, &Downloader::onChunkProgress);
        connect(worker, &DownloadWorker::Finished, this, &Downloader::onChunkFinished);

        connect(worker, &DownloadWorker::Finished, this, [this, worker, workerThread]()
        {
            m_workers.removeOne(worker);
            m_workerThreads.removeOne(workerThread);
        });

        workerThread->start();
    }
    });

    StartDataTimer();

    emit progressChanged(m_bytesDownloaded, info.fileByteSize);
}

void Downloader::downloadStop()
{
    isPausing = false;
    if (saveTimer) saveTimer->stop();

    for (DownloadWorker *worker : m_workers)
        QMetaObject::invokeMethod(worker, "Stop", Qt::QueuedConnection);

    for (QThread *thread : m_workerThreads)
    {
        thread->quit();
        thread->wait(3000);
    }

    m_workers.clear();
    m_workerThreads.clear();

    if (reply)
    {
        QNetworkReply *r = reply;
        reply = nullptr;
        r->abort();
        r->deleteLater();
    }
    if (file.isOpen()) file.close();

    for (const QString &tempPath : m_tempPaths)
        QFile::remove(tempPath);
    QDir tempDir(m_qdmTempDir + "/" + info.ID);
    tempDir.removeRecursively();

    m_tempPaths.clear();
}

void Downloader::downloadPause()
{
    if (m_workers.isEmpty()) return;

    isPausing = true;
    if (saveTimer) saveTimer->stop();

    WriteDownloadData();

    for (DownloadWorker *worker : m_workers)
    {
        QMetaObject::invokeMethod(worker, "Stop", Qt::QueuedConnection);
    }
}

qint64 Downloader::fileSize()
{
    return info.fileByteSize;
}

int Downloader::chunkNumber()
{
    return info.chunkCount;
}

QString Downloader::downloadID()
{
    return info.ID;
}

QVector<qint64> Downloader::chunkProgressData()
{
    return chunkProgress;
}

qint64 Downloader::bytesDownloaded()
{
    return m_bytesDownloaded;
}