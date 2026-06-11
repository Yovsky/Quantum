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

#include "src/core/appglobals.h"
#include "src/ui/downloadwindow.h"
#include "ui_downloadwindow.h"
#include "src/core/downloader.h"
#include "finishwindow.h"
#include <QMessageBox>
#include <QSystemTrayIcon>
#include <QElapsedTimer>
#include <QFileInfo>

DownloadWindow::DownloadWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DownloadWindow)
    , download(new Downloader(this))
    , Status("Unknown")
{
    ui->setupUi(this);
    AppGlobals::instance().setDownloadWindow(this);
    ui->adress->setReadOnly(true);

    connect(download, &Downloader::progressChanged, this, &DownloadWindow::onProgressChange);
    connect(download, &Downloader::downloadFinished, this, &DownloadWindow::onDownloadFinish);
    connect(download, &Downloader::downloadStarted, this, [this]()
    {
        Status = "Downloading...";
        ui->status->setText(Status);
    });

    isPaused = false;
}

void DownloadWindow::startDownload(const QUrl &url, const QString &savePath, int threadNumber, const QString &SHA256)
{
    QUrl finalUrl = QUrl::fromUserInput(url.toString());
    if (finalUrl.scheme().isEmpty())
        finalUrl.setScheme("http");

    ui->adress->setText(finalUrl.toString());
    ui->progressBar->setValue(0);
    Status = "Starting Download...";
    ui->status->setText(Status);

    fileUrl = finalUrl.toString();
    filePath = savePath;

    downloadTimer.start();
    lastBytesReceived = 0;
    DownloadDate = QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm:ss");
    lastUpdateTime = QTime::currentTime();
    download->download(finalUrl, savePath, threadNumber, SHA256);
}

void DownloadWindow::Resume(downloadInformations info)
{
    ui->adress->setText(info.url);
    ui->progressBar->setValue(static_cast<int>(info.progress));
    Status = "Resuming...";
    ui->status->setText(Status);

    fileUrl = info.url;
    filePath = info.savePath;

    qint64 currentBytes = 0;
    for (qint64 c : info.chunkProgress) currentBytes += c;
    lastDownloaded = currentBytes;
    lastBytesReceived = currentBytes;
    lastUpdateTime = QTime::currentTime();

    if (currentBytes < 1024)
        Recmsg = QString::number(currentBytes) + " B";
    else if (currentBytes < 1024 * 1024)
        Recmsg = QString::number(currentBytes / 1024.0, 'f', 2) + " KB";
    else if (currentBytes < 1024 * 1024 * 1024)
        Recmsg = QString::number(currentBytes / (1024.0 * 1024.0), 'f', 2) + " MB";
    else
        Recmsg = QString::number(currentBytes / (1024.0 * 1024.0 * 1024.0), 'f', 2) + " GB";

    ui->downloaded->setText(Recmsg);

    DownloadDate = QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm:ss");
    downloadTimer.start();
    download->downloadResume(info);
}

void DownloadWindow::onProgressChange(qint64 bytesReceived, qint64 bytesTotal)
{
    if (bytesTotal <= 0) {
        ui->progressBar->setValue(lastProgress);
        return;
    }

    lastDownloaded = bytesReceived;

    Progress = (static_cast<double>(lastDownloaded) / bytesTotal) * 100.0;
    ui->progressBar->setValue(static_cast<int>(Progress));

    double Received = 0;
    double Total = 0;

    if(lastDownloaded < 1024)
    {
        Recmsg = QString::number(lastDownloaded, 'f', 2) + " B";
    }
    else if (lastDownloaded < 1024.0 * 1024.0)
    {
        Received = lastDownloaded / 1024.0;
        Recmsg = QString::number(Received, 'f', 2) + " KB";
    }
    else if (lastDownloaded < (1024.0 * 1024.0 * 1024))
    {
        Received = lastDownloaded / (1024.0 * 1024.0);
        Recmsg = QString::number(Received, 'f', 2) + " MB";
    }
    else if (lastDownloaded > (1024.0 * 1024.0 * 1024))
    {
        Received = lastDownloaded / (1024.0 * 1024.0 * 1024);
        Recmsg = QString::number(Received, 'f', 2) + " GB";
    }


    if(bytesTotal < 1024)
    {
        Size = QString::number(bytesTotal, 'f', 2) + " B";
    }
    else if (bytesTotal < 1024*1024)
    {
        Total = bytesTotal / 1024.0;
        Size = QString::number(Total, 'f', 2) + " KB";
    }
    else if (bytesTotal < 1024*1024*1024)
    {
        Total = bytesTotal / (1024.0 * 1024.0);
        Size = QString::number(Total, 'f', 2) + " MB";
    }
    else if (bytesTotal >= 1024*1024*1024)
    {
        Total = bytesTotal / (1024.0 * 1024.0 * 1024);
        Size = QString::number(Total, 'f', 2) + " GB";
    }

    QTime currentTime = QTime::currentTime();
    int elapsedMs = lastUpdateTime.msecsTo(currentTime);


    if (elapsedMs > 500) {
        qint64 bytesSinceLast = bytesReceived - lastBytesReceived;
        double mbSinceLast = bytesSinceLast / (1024.0 * 1024.0);
        double secondsSinceLast = elapsedMs / 1000.0;

        if (secondsSinceLast > 0) {
            double instantSpeed = mbSinceLast / secondsSinceLast;
            Transfer = QString::number(instantSpeed, 'f', 2) + " MB/s";
            ui->transSpeed->setText(Transfer);

            int sRTA = 0;
            if (instantSpeed != 0)
                sRTA = ((bytesTotal - bytesReceived) / (1024 * 1024)) / instantSpeed;
            else
                ui->RTA->setText("Unknown");
            int mRTA = 0;
            int hRTA = 0;
            while (sRTA >= 60)
            {
                sRTA -= 60;
                mRTA++;
            }
            while (mRTA >= 60)
            {
                mRTA -= 60;
                hRTA++;
            }
            QString timeRemaining;
            if(hRTA != 0)
                timeRemaining += QString::number(hRTA) + " h ";
            if(mRTA != 0)
                timeRemaining += QString::number(mRTA) + " m ";
            timeRemaining += QString::number(sRTA) + " s";
            ui->RTA->setText(timeRemaining);
        }

        lastBytesReceived = bytesReceived;
        lastUpdateTime = currentTime;
        GatherDownloadInfo();
    }



    ui->downloaded->setText(Recmsg);

    if (bytesTotal > 0) {
        ui->fileSize->setText(Size);
    } else {
        ui->fileSize->setText("Unknown");
    }
}

void DownloadWindow::GatherDownloadInfo()
{
    Info.fileName = QFileInfo(filePath).fileName();
    Info.fileSize = Size;
    Info.speed = Transfer;
    Info.Date = DownloadDate;
    Info.progress = Progress;
    Info.status = Status;
    Info.currentSize = Recmsg;
    Info.url = fileUrl;
    Info.savePath = filePath;
    Info.fileByteSize = download->fileSize();
    Info.chunkCount = download->chunkNumber();
    Info.chunkProgress = download->chunkProgressData();
    Info.ID = download->downloadID();

    emit DownloadInfo(Info);
}

void DownloadWindow::onDownloadFinish(bool success, const QString &message)
{
    if (isPaused || message.contains("canceled", Qt::CaseInsensitive))
        return;

    Status = success ? "Completed." : "Failed.";
    ui->status->setText(Status);

    if (!success)
    {
        QMessageBox::critical(this, "Download Error", message);
        return;
    }

    ui->progressBar->setValue(100);

    downloadInformations Info;
    Info.fileName = QFileInfo(filePath).fileName();
    Info.fileSize = Size;
    Info.Date = DownloadDate;
    Info.progress = 100.0f;
    Info.status = "Completed";
    Info.currentSize = Recmsg;
    emit DownloadInfo(Info);

    FinishWindow finish(nullptr, fileUrl, filePath);

    QSystemTrayIcon tray;
    tray.showMessage("QDMan", message);

    this->hide();
    finish.exec();
    this->close();
}

void DownloadWindow::downloadStop()
{
    disconnect(download, nullptr, this, nullptr);
    download->downloadStop();
    this->close();
}

DownloadWindow::~DownloadWindow()
{
    delete ui;
}

void DownloadWindow::on_Cancel_clicked()
{
    ui->status->setText("Stopping download...");
    downloadStop();
}

void DownloadWindow::on_Pause_clicked()
{
    if (!isPaused)
    {
        isPaused = true;
        download->downloadPause();
        ui->Pause->setText("Resume");
        Status = "Paused";
        lastProgress = ui->progressBar->value();

    }
    else
    {
        isPaused = false;
        ui->Pause->setText("Pause");
        Status = "Downloading...";

        qint64 currentBytes = download->bytesDownloaded();
        lastDownloaded = currentBytes;
        lastBytesReceived = currentBytes;
        lastUpdateTime = QTime::currentTime();

        download->downloadResume(Info);
    }
    ui->status->setText(Status);
}
