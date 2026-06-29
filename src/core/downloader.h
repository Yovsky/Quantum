#ifndef DOWNLOADER_H
#define DOWNLOADER_H

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

#include "src/core/downloadworker.h"
#include "src/models/downloadstatus.h"
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QNetworkRequest>
#include <QFile>
#include <QFileInfo>
#include <QUrl>
#include <QTimer>
#include <QDebug>
#include <QThread>
#include <QStandardPaths>
#include <QDir>
#include <QUuid>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

class Downloader : public QObject
{
    Q_OBJECT

public:
    explicit Downloader(QObject *parent = nullptr);
    void download(downloadInformations info);
    void WriteDownloadData();
    void StartDataTimer();
    void SetupWorkers();
    void downloadStop();
    void downloadPause();
    void downloadResume(downloadInformations info);
    qint64 fileSize();
    int chunkNumber();
    QVector<qint64> chunkProgressData();
    QString downloadID();
    qint64 bytesDownloaded();
signals:
    void downloadStarted();
    void progressChanged(qint64 bytesRecived, qint64 bytesTotal);
    void downloadFinished(bool success, const QString &message);
private slots:
    void onHeadFinished();
    void onHeadTestFinished();
    void onChunkProgress(int chunkIndex, qint64 bytes);
    void onChunkFinished();
    void onReadReady();
    void onDownloadFinished();
private:
    void mergeTemporaryFiles();
    QNetworkAccessManager *manager;
    QNetworkReply *reply;
    QFile file;
    QUrl m_url;
    int m_chunksCompleted;
    QVector<qint64> chunkProgress;
    qint64 m_bytesDownloaded;
    qint64 currentSize = 0;
    QTimer *saveTimer = nullptr;
    QString m_qdmTempDir;
    QVector<DownloadWorker*> m_workers;
    QVector<QThread*> m_workerThreads;
    bool isPausing = false;
    bool isResuming;
    QStringList m_tempPaths;
    downloadInformations info;
    QFile m_file;
};

#endif // DOWNLOADER_H
