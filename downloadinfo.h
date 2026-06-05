#ifndef DOWNLOADINFO_H
#define DOWNLOADINFO_H

#include "downloadstatus.h"
#include <QWidget>

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
    void UpdateInfo(const DownloadStatus &Info);
    ~DownloadInfo();

private:
    Ui::DownloadInfo *ui;
};

#endif // DOWNLOADINFO_H
