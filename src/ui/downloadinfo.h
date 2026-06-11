#ifndef DOWNLOADINFO_H
#define DOWNLOADINFO_H

#include "src/models/downloadstatus.h"
#include <QWidget>
#include <QPushButton>

namespace Ui {
class DownloadInfo;
}

class DownloadInfo : public QWidget
{
    Q_OBJECT

public:
    explicit DownloadInfo(QWidget *parent);
    void Downloading(QString Data);
    void Paused(QString Data);
    void Finished(QString Data);
    void UpdateInfo(const downloadInformations &Info);
    ~DownloadInfo();

signals:
    void resumeRequested(downloadInformations info);
    void pauseRequested();

private slots:
    void on_pauseResume_clicked();

private:
    downloadInformations m_info;
    Ui::DownloadInfo *ui;
};

#endif // DOWNLOADINFO_H
