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
