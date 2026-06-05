#include "downloadinfo.h"
#include "ui_downloadinfo.h"

DownloadInfo::DownloadInfo(QWidget *parent, QString Info)
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

void DownloadInfo::UpdateInfo(QString Info)
{
    QStringList parts = Info.split('|');
    if (parts.isEmpty() || parts[0].isEmpty()) return;

    ui->fileName->setText(parts[0]);
    ui->size->setText(parts[1]);
    ui->speed->setText(parts[3]);
}

DownloadInfo::~DownloadInfo()
{
    delete ui;
}
