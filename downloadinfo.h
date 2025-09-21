#ifndef DOWNLOADINFO_H
#define DOWNLOADINFO_H

#include <QWidget>

namespace Ui {
class DownloadInfo;
}

class DownloadInfo : public QWidget
{
    Q_OBJECT

public:
    explicit DownloadInfo(QWidget *parent, QString Type, QString Info);
    void Downloading(QString Data);
    void Paused(QString Data);
    void Finished(QString Data);
    ~DownloadInfo();

private:
    Ui::DownloadInfo *ui;
};

#endif // DOWNLOADINFO_H
