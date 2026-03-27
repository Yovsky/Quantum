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

void Downloader::download(const QUrl &url, const QString &savePath)
{
    file.setFileName(savePath);
    SavePath = savePath;

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
