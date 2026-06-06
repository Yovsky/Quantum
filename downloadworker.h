#ifndef DOWNLOADWORKER_H
#define DOWNLOADWORKER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QFile>

class DownloadWorker : public QObject
{
    Q_OBJECT
public slots:
    void StartDownload();
public:
    explicit DownloadWorker(const QUrl& url, qint64 start, qint64 end, const QString &tempPath);
private slots:
    void OnReadReady();
    void OnReplyFinished();
private:
    QUrl m_url;
    qint64 m_start;
    qint64 m_end;
    QString m_tempPath;
    QFile m_file;
    int retryCount = 0;
    int retryMax = 3;

    QNetworkAccessManager *manager = nullptr;
    QNetworkReply *reply = nullptr;
signals:
    void Progress(qint64 bytesRec);
    void Finished();
    void ErrorOcc(QString errStr);
};

#endif // DOWNLOADWORKER_H
