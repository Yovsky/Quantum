#include "downloadinfo.h"
#include "ui_downloadinfo.h"

DownloadInfo::DownloadInfo(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DownloadInfo)
{
    ui->setupUi(this);
    setFixedHeight(70);
    // if (Type == "Downloading")
    //     Downloading(Info);
    // else if (Type == "Paused")
    //     Paused(Info);
    // else if (Type == "Finished")
    //     Finished(Info);
}

void DownloadInfo::Downloading(QString Data)
{

}

void DownloadInfo::Paused(QString Data)
{

}

void DownloadInfo::Finished(QString Data)
{

}

void DownloadInfo::UpdateInfo(const DownloadStatus &Info)
{
    if (Info.fileName.isEmpty()) return;

    ui->fileName->setText(Info.fileName);
    ui->size->setText(Info.currentSize + "/" + Info.fileSize);
    ui->speed->setText(Info.speed);
    ui->percentage->setText(QString::number(Info.progress, 'f', 0) + "%");
    ui->progressBar->setValue(static_cast<int>(Info.progress));
}

DownloadInfo::~DownloadInfo()
{
    delete ui;
}
