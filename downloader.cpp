#include "downloader.h"
#include <QtNetwork/QNetworkRequest>
#include <QUrl>

Downloader::Downloader(QObject *parent) : QObject(parent)
{
    manager = new QNetworkAccessManager(this);
}

void Downloader::download(const QUrl &url, const QString &savePath)
{
    file.setFileName(savePath);

    if(!file.open(QIODevice::WriteOnly))
    {
        emit downloadFinished(false, "File not downloaded, error: " + file.errorString());
        return;
    }

    QNetworkRequest request(url);
    reply = manager->get(request);

    connect(reply, &QIODevice::readyRead, this, &Downloader::onReadReady);
    connect(reply, &QNetworkReply::finished, this, &Downloader::onDownloadFinished);
    connect(reply, &QNetworkReply::downloadProgress, this, &Downloader::progressChanged);

    emit downloadStarted();
}

void Downloader::onReadReady()
{
    if (reply->bytesAvailable() > 0)
        file.write(reply->readAll());
}

void Downloader::onDownloadFinished()
{
    if (reply->error() != QNetworkReply::NoError)
    {
        file.remove();
        emit downloadFinished(false, "Download not complete, error: " + reply->errorString());
        reply->deleteLater();
        return;
    }

    if (reply->bytesAvailable() > 0)
        file.write(reply->readAll());
    file.close();

    emit downloadFinished(true, "Download completed successfully.");
    reply->deleteLater();
}

void Downloader::downloadStop()
{
    file.remove();
    reply->deleteLater();
}
