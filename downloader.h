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
    void downloadStop();
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
