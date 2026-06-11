#include "downloadinfo.h"
#include "ui_downloadinfo.h"

DownloadInfo::DownloadInfo(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DownloadInfo)
{
    ui->setupUi(this);
    setFixedHeight(70);
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

void DownloadInfo::UpdateInfo(const downloadInformations &Info)
{
    if (Info.fileName.isEmpty()) return;
    m_info = Info;
    if (Info.status == "Paused.") ui->pauseResume->setText("Resume");
    else if (Info.status == "Downloading...") ui->pauseResume->setText("Pause");

    ui->status->setText(Info.status);
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

void DownloadInfo::on_pauseResume_clicked()
{
    if (ui->status->text() == "Downloading...")
        emit pauseRequested();
    else if (ui->status->text() == "Paused.")
        emit resumeRequested(m_info);
}

