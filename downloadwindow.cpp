#include "appglobals.h"
#include "downloadwindow.h"
#include "ui_downloadwindow.h"
#include "downloader.h"
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


    connect(download, &Downloader::progressChanged, this, &DownloadWindow::onProgressChange);
    connect(download, &Downloader::downloadFinished, this, &DownloadWindow::onDownloadFinish);
    connect(download, &Downloader::downloadStarted, this, [this]()
    {
        Status = "Downloading...";
        ui->status->setText(Status);
    });
}

void DownloadWindow::startDownload(const QUrl &url, const QString &savePath)
{
    ui->adress->setText(url.toString());
    ui->progressBar->setValue(0);
    Status = "Starting Download...";
    ui->status->setText(Status);

    fileUrl = url.toString();
    filePath = savePath;

    downloadTimer.start();
    lastBytesReceived = 0;
    DownloadDate = QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm:ss");
    lastUpdateTime = QTime::currentTime();
    download->download(url, savePath);
}

void DownloadWindow::onProgressChange(qint64 bytesReceived, qint64 bytesTotal)
{
    if (bytesTotal <= 0) {
        ui->progressBar->setValue(0);
        return;
    }

    double progress = (static_cast<double>(bytesReceived) / bytesTotal) * 100.0;
    ui->progressBar->setValue(static_cast<int>(progress));

    double Received = 0;
    double Total = 0;
    QString Recmsg;


    if(bytesReceived < 1024)
    {
        Recmsg = QString::number(bytesReceived, 'f', 2) + " Bytes";
    }
    else if (bytesReceived < std::pow(1024, 2))
    {
        Received = bytesReceived / 1024.0;
        Recmsg = QString::number(Received, 'f', 2) + " KB";
    }
    else if (bytesReceived < std::pow(1024, 3))
    {
        Received = bytesReceived / (1024.0 * 1024.0);
        Recmsg = QString::number(Received, 'f', 2) + " MB";
    }
    else if (bytesReceived < std::pow(1024, 4))
    {
        Received = bytesReceived / (1024.0 * 1024.0 * 1024);
        Recmsg = QString::number(Received, 'f', 2) + " GB";
    }


    if(bytesTotal < 1024)
    {
        Size = QString::number(bytesTotal, 'f', 2) + " Bytes";
    }
    else if (bytesTotal < std::pow(1024, 2))
    {
        Total = bytesTotal / 1024.0;
        Size = QString::number(Total, 'f', 2) + " KB";
    }
    else if (bytesTotal < std::pow(1024, 3))
    {
        Total = bytesTotal / (1024.0 * 1024.0);
        Size = QString::number(Total, 'f', 2) + " MB";
    }
    else if (bytesTotal < std::pow(1024, 4))
    {
        Total = bytesTotal / (1024.0 * 1024.0 * 1024);
        Size = QString::number(Total, 'f', 2) + " GB";
    }

    QTime currentTime = QTime::currentTime();
    int elapsedMs = lastUpdateTime.msecsTo(currentTime);


    if (elapsedMs > 500) { // Update speed every 100ms for stability
        qint64 bytesSinceLast = bytesReceived - lastBytesReceived;
        double mbSinceLast = bytesSinceLast / (1024.0 * 1024.0);
        double secondsSinceLast = elapsedMs / 1000.0;

        if (secondsSinceLast > 0) {
            double instantSpeed = mbSinceLast / secondsSinceLast;
            Transfer = QString::number(instantSpeed, 'f', 2) + " MB/s";
            ui->transSpeed->setText(Transfer);

            int sRTA = ((bytesTotal - bytesReceived) / (1024 * 1024)) / instantSpeed;
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
    QString Info = QFileInfo(filePath).fileName() + "|";
    Info += Size + "|";
    Info += Status + "|";
    Info += Transfer + "|";
    Info += DownloadDate;
    emit DownloadInfo(Info);
}

void DownloadWindow::onDownloadFinish(bool success, const QString &message)
{
    Status = (success)? "Completed." : "Failed.";
    ui->status->setText(Status);
    if (!success)
        QMessageBox::critical(this, "Download Error", message);
    else
    {
        ui->progressBar->setValue(100);
        QSystemTrayIcon tray;
        tray.showMessage("QDMan", message);
    }

    this->close();

    FinishWindow finish(this, fileUrl, filePath);
    finish.exec();
}

void DownloadWindow::downloadStop()
{
    download->downloadStop();
    this->close();
}

DownloadWindow::~DownloadWindow()
{
    delete ui;
}

void DownloadWindow::on_Cancel_clicked()
{
    downloadStop();
}

