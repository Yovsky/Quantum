#include "downloadwindow.h"
#include "ui_downloadwindow.h"
#include "downloader.h"
#include <QMessageBox>
#include <QSystemTrayIcon>

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

    double mbReceived = bytesReceived / (1024.0 * 1024.0);
    double mbTotal = bytesTotal / (1024.0 * 1024.0);

    ui->downloaded->setText(QString::number(mbReceived, 'f', 2) + " MB");

    if (bytesTotal > 0) {
        ui->fileSize->setText(QString::number(mbTotal, 'f', 2) + " MB");
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

DownloadWindow::~DownloadWindow()
{
    delete ui;
}
