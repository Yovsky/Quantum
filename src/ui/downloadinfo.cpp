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
    m_info = Info;
    if (m_info.fileName.isEmpty()) return;
    if (m_info.status == "Paused.") ui->pauseResume->setText("Resume");
    else if (m_info.status == "Downloading...") ui->pauseResume->setText("Pause");

    ui->status->setText(m_info.status);
    ui->fileName->setText(m_info.fileName);
    ui->size->setText(m_info.currentSize + "/" + m_info.fileSize);
    ui->speed->setText(m_info.speed);
    ui->percentage->setText(QString::number(m_info.progress, 'f', 0) + "%");
    ui->progressBar->setValue(static_cast<int>(m_info.progress));
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

