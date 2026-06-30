#ifndef DOWNLOADWORKER_H
#define DOWNLOADWORKER_H

#include "src/models/downloadstatus.h"
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QFile>

class DownloadWorker : public QObject
{
    Q_OBJECT
public slots:
    void StartDownload();
    void Stop();
public:
    explicit DownloadWorker(int chunkIndex, qint64 start, qint64 end, bool isResuming, downloadInformations Info);

    QNetworkReply *reply = nullptr;
private slots:
    void OnReadReady();
    void OnReplyFinished();
private:
    qint64 m_start;
    qint64 m_end;
    QFile m_file;
    qint64 m_downloadedBytes = 0;
    int m_chunkIndex;
    int retryCount = 0;
    int retryMax = 3;
    bool m_isResuming = false;
    bool m_Stopped = false;
    QByteArray m_writeBuffer;
    static constexpr qint64 BUFFER_SIZE = 4 * 1024 * 1024;
    downloadInformations info;
    qint64 m_downloadOffset;

    QNetworkAccessManager *manager = nullptr;
signals:
    void Progress(int chunkIndex, qint64 bytesRec);
    void Finished();
    void ErrorOcc(QString errStr);
};

#endif // DOWNLOADWORKER_H
