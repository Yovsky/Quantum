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

#include "downloadworker.h"
#include "downloadstatus.h"
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
    void download(const QUrl &url, const QString &savePath, int chunkNumber, const QString &SHA256);
    void WriteDownloadData();
    void StartDataTimer();
    void SetupWorkers();
    void downloadStop();
    void downloadPause();
    void downloadResume(downloadInformations info);
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
    QString m_savePath;
    int m_chunkNumber;
    int m_chunksCompleted;
    QVector<qint64> chunkProgress;
    QString m_SHA256;
    qint64 m_filesize;
    qint64 m_bytesDownloaded;
    qint64 currentSize = 0;
    QTimer *saveTimer = nullptr;
    QString m_qdmTempDir;
    QString m_downloadID;
    bool isPausing = false;
    bool isResuming;
    QStringList m_tempPaths;
};

#endif // DOWNLOADER_H
