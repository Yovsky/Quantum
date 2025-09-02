#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QFile>
#include <QUrl>

class Downloader : public QObject
{
    Q_OBJECT

public:
    explicit Downloader(QObject *parent = nullptr);
    void download(const QUrl &url, const QString &savePath);
signals:
    void downloadStarted();
    void progressChanged(qint64 bytesRecived, qint64 bytesTotal);
    void downloadFinished(bool success, const QString &message);
private slots:
    void onReadReady();
    void onDownloadFinished();
private:
    QNetworkAccessManager *manager;
    QNetworkReply *reply;
    QFile file;
    QUrl url;
};

#endif // DOWNLOADER_H
