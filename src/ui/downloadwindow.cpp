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
#include "src/models/downloadstatus.h"
#include <QMessageBox>
#include <QSystemTrayIcon>
#include <QElapsedTimer>
#include <QFileInfo>

DownloadWindow::DownloadWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DownloadWindow)
    , download(new Downloader(this))
{
    ui->setupUi(this);
    AppGlobals::instance().setDownloadWindow(this);
    ui->adress->setReadOnly(true);

    connect(download, &Downloader::progressChanged, this, &DownloadWindow::onProgressChange);
    connect(download, &Downloader::downloadFinished, this, &DownloadWindow::onDownloadFinish);
    connect(download, &Downloader::downloadStarted, this, [this]()
    {
        info.status = "Downloading...";
        ui->status->setText(info.status);
    });

    isPaused = false;
}

void DownloadWindow::startDownload(downloadInformations Info)
{
    info = Info;
    info.status = "Starting Download...";
    QUrl finalUrl = QUrl::fromUserInput(Info.url);
    if (finalUrl.scheme().isEmpty())
        finalUrl.setScheme("http");
    info.url = finalUrl.toString();

    ui->adress->setText(info.url);
    ui->progressBar->setValue(0);
    ui->status->setText(info.status);

    this->setWindowTitle(QFileInfo(info.savePath).fileName());

    downloadTimer.start();
    lastBytesReceived = 0;
    info.Date = QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm:ss");
    lastUpdateTime = QTime::currentTime();
    download->download(info);

}

void DownloadWindow::Resume(downloadInformations Info)
{
    info = Info;

    this->setWindowTitle(info.fileName);
    ui->adress->setText(info.url);
    ui->progressBar->setValue(static_cast<int>(info.progress));
    info.status = "Resuming...";
    ui->status->setText(info.status);

    qint64 currentBytes = 0;
    for (qint64 c : info.chunkProgress) currentBytes += c;
    lastDownloaded = currentBytes;
    lastBytesReceived = currentBytes;
    lastUpdateTime = QTime::currentTime();

    if (currentBytes < 1024)
        info.currentSize = QString::number(currentBytes) + " B";
    else if (currentBytes < 1024 * 1024)
        info.currentSize = QString::number(currentBytes / 1024.0, 'f', 2) + " KB";
    else if (currentBytes < 1024 * 1024 * 1024)
        info.currentSize = QString::number(currentBytes / (1024.0 * 1024.0), 'f', 2) + " MB";
    else
        info.currentSize = QString::number(currentBytes / (1024.0 * 1024.0 * 1024.0), 'f', 2) + " GB";

    ui->downloaded->setText(info.currentSize);

    info.Date = QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm:ss");
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

    info.progress = (static_cast<double>(lastDownloaded) / bytesTotal) * 100.0;
    ui->progressBar->setValue(static_cast<int>(info.progress));
    ui->status->setText("Downloading...");

    double Received = 0;
    double Total = 0;

    if(lastDownloaded < 1024)
    {
        info.currentSize = QString::number(lastDownloaded, 'f', 2) + " B";
    }
    else if (lastDownloaded < 1024.0 * 1024.0)
    {
        Received = lastDownloaded / 1024.0;
        info.currentSize = QString::number(Received, 'f', 2) + " KB";
    }
    else if (lastDownloaded < (1024.0 * 1024.0 * 1024))
    {
        Received = lastDownloaded / (1024.0 * 1024.0);
        info.currentSize = QString::number(Received, 'f', 2) + " MB";
    }
    else if (lastDownloaded > (1024.0 * 1024.0 * 1024))
    {
        Received = lastDownloaded / (1024.0 * 1024.0 * 1024);
        info.currentSize = QString::number(Received, 'f', 2) + " GB";
    }


    if(bytesTotal < 1024)
    {
        info.fileSize = QString::number(bytesTotal, 'f', 2) + " B";
    }
    else if (bytesTotal < 1024*1024)
    {
        Total = bytesTotal / 1024.0;
        info.fileSize = QString::number(Total, 'f', 2) + " KB";
    }
    else if (bytesTotal < 1024*1024*1024)
    {
        Total = bytesTotal / (1024.0 * 1024.0);
        info.fileSize = QString::number(Total, 'f', 2) + " MB";
    }
    else if (bytesTotal >= 1024*1024*1024)
    {
        Total = bytesTotal / (1024.0 * 1024.0 * 1024);
        info.fileSize = QString::number(Total, 'f', 2) + " GB";
    }

    QTime currentTime = QTime::currentTime();
    int elapsedMs = lastUpdateTime.msecsTo(currentTime);


    if (elapsedMs > 500) {
        qint64 bytesSinceLast = bytesReceived - lastBytesReceived;
        double mbSinceLast = bytesSinceLast / (1024.0 * 1024.0);
        double secondsSinceLast = elapsedMs / 1000.0;

        if (secondsSinceLast > 0) {
            double instantSpeed = mbSinceLast / secondsSinceLast;
            info.speed = QString::number(instantSpeed, 'f', 2) + " MB/s";
            ui->transSpeed->setText(info.speed);

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



    ui->downloaded->setText(info.currentSize);

    if (bytesTotal > 0) {
        ui->fileSize->setText(info.fileSize);
    } else {
        ui->fileSize->setText("Unknown");
    }
}

void DownloadWindow::GatherDownloadInfo()
{
    info.fileByteSize = download->fileSize();
    info.chunkCount = download->chunkNumber();
    info.chunkProgress = download->chunkProgressData();
    info.ID = download->downloadID();

    emit DownloadInfo(info);
}

void DownloadWindow::onDownloadFinish(bool success, const QString &message)
{
    if (isPaused || message.contains("canceled", Qt::CaseInsensitive))
        return;

    info.status = success ? "Completed." : "Failed.";
    ui->status->setText(info.status);

    if (!success)
    {
        QMessageBox::critical(this, "Download Error", message);
        return;
    }

    ui->progressBar->setValue(100);

    info.progress = 100.0f;
    info.status = "Completed";
    emit DownloadInfo(info);

    FinishWindow finish(nullptr, info.url, info.savePath);

    QSystemTrayIcon tray;
    tray.showMessage("QDMan", message);

    this->hide();
    finish.exec();
    this->close();
}

void DownloadWindow::downloadStop()
{
    // disconnect(download, nullptr, this, nullptr);
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
        info.status = "Paused";
        lastProgress = ui->progressBar->value();

    }
    else
    {
        isPaused = false;
        ui->Pause->setText("Pause");
        info.status = "Downloading...";

        qint64 currentBytes = download->bytesDownloaded();
        lastDownloaded = currentBytes;
        lastBytesReceived = currentBytes;
        lastUpdateTime = QTime::currentTime();

        download->downloadResume(info);
    }
    ui->status->setText(info.status);
}
