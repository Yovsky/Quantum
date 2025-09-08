#ifndef DOWNLOADWINDOW_H
#define DOWNLOADWINDOW_H

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
#include <QWidget>
#include <QUrl>
#include <QElapsedTimer>

namespace Ui {
class DownloadWindow;
}

class DownloadWindow : public QWidget
{
    Q_OBJECT

public:
    explicit DownloadWindow(QWidget *parent = nullptr);
    void startDownload(const QUrl &url, const QString &savePath);
    void GatherDownloadInfo();
    ~DownloadWindow();

private slots:
    void onProgressChange(qint64 bytesrecived, qint64 bytestotal);
    void onDownloadFinish(bool success, const QString &message);
    void downloadStop();

    void on_Cancel_clicked();

signals:
    void DownloadInfo(QString Info);

private:
    Ui::DownloadWindow *ui;
    Downloader *download;
    QString filePath;
    QString Status;
    QString Transfer;
    QString Size;
    QString DownloadDate;
    QElapsedTimer downloadTimer;
    qint64 lastBytesReceived = 0;
    QTime lastUpdateTime;
    QString fileUrl;
};

#endif // DOWNLOADWINDOW_H
