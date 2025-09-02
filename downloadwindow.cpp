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
    ui->progressBar->setValue(0);
    ui->status->setText("Starting Download...");
    download->download(url, savePath);
}

void DownloadWindow::onProgressChange(qint64 bytesrecived, qint64 bytestotal)
{
    ui->progressBar->setValue((bytesrecived / bytestotal) * 100);

    ui->downloaded->setText(QString::number(bytesrecived / 1024 / 1024));
    ui->fileSize->setText(QString::number(bytestotal / 1024 / 1024));
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
