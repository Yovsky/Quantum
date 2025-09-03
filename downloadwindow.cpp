#include "downloadwindow.h"
#include "ui_downloadwindow.h"
#include "downloader.h"
#include <QMessageBox>
#include <QSystemTrayIcon>
#include <QElapsedTimer>

DownloadWindow::DownloadWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DownloadWindow)
    , download(new Downloader(this))
{
    ui->setupUi(this);

    connect(download, &Downloader::progressChanged, this, &DownloadWindow::onProgressChange);
    connect(download, &Downloader::downloadFinished, this, &DownloadWindow::onDownloadFinish);
    connect(download, &Downloader::downloadStarted, this, [this]()
    {
        ui->status->setText("Downloading...");
    });
}

void DownloadWindow::startDownload(const QUrl &url, const QString &savePath)
{
    ui->adress->setText(url.toString());
    ui->progressBar->setValue(0);
    ui->status->setText("Starting Download...");

    downloadTimer.start();
    lastBytesReceived = 0;
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
    QString Totmsg;


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
        Totmsg = QString::number(bytesTotal, 'f', 2) + " Bytes";
    }
    else if (bytesTotal < std::pow(1024, 2))
    {
        Received = bytesTotal / 1024.0;
        Totmsg = QString::number(Total, 'f', 2) + " KB";
    }
    else if (bytesTotal < std::pow(1024, 3))
    {
        Received = bytesTotal / (1024.0 * 1024.0);
        Totmsg = QString::number(Total, 'f', 2) + " MB";
    }
    else if (bytesTotal < std::pow(1024, 4))
    {
        Received = bytesTotal / (1024.0 * 1024.0 * 1024);
        Totmsg = QString::number(Total, 'f', 2) + " GB";
    }

    QTime currentTime = QTime::currentTime();
    int elapsedMs = lastUpdateTime.msecsTo(currentTime);


    if (elapsedMs > 500) { // Update speed every 100ms for stability
        qint64 bytesSinceLast = bytesReceived - lastBytesReceived;
        double mbSinceLast = bytesSinceLast / (1024.0 * 1024.0);
        double secondsSinceLast = elapsedMs / 1000.0;

        if (secondsSinceLast > 0) {
            double instantSpeed = mbSinceLast / secondsSinceLast;
            ui->transSpeed->setText(QString::number(instantSpeed, 'f', 2) + " MB/s");

            int sRTA = (bytesTotal / (1024 * 1024)) / instantSpeed;
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
            if(sRTA != 0)
                timeRemaining += QString::number(sRTA) + " s";
            ui->RTA->setText(timeRemaining);
        }

        lastBytesReceived = bytesReceived;
        lastUpdateTime = currentTime;
    }



    ui->downloaded->setText(Recmsg);

    if (bytesTotal > 0) {
        ui->fileSize->setText(Totmsg);
    } else {
        ui->fileSize->setText("Unknown");
    }
}

void DownloadWindow::onDownloadFinish(bool success, const QString &message)
{
    ui->status->setText((success)? "Completed." : "Failed.");
    if (!success)
        QMessageBox::critical(this, "Download Error", message);
    else
    {
        ui->progressBar->setValue(100);
        QSystemTrayIcon tray;
        tray.showMessage("QDMan", message);
    }
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

