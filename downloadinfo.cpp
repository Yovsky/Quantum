#include "downloadinfo.h"
#include "ui_downloadinfo.h"

DownloadInfo::DownloadInfo(QWidget *parent, QString Type, QString Info)
    : QWidget(parent)
    , ui(new Ui::DownloadInfo)
{
    ui->setupUi(this);
    if (Type == "Downloading")
        Downloading(Info);
    else if (Type == "Paused")
        Paused(Info);
    else if (Type == "Finished")
        Finished(Info);
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

DownloadInfo::~DownloadInfo()
{
    delete ui;
}
