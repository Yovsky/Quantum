#ifndef DOWNLOADWINDOW_H
#define DOWNLOADWINDOW_H

#include "downloader.h"
#include <QWidget>
#include <QUrl>
#include <QElapsedTimer>

namespace Ui {
class DownloadWindow;
}

class DownloadWindow : public QWidget
{
    Q_OBJECT

public:
    explicit DownloadWindow(QWidget *parent = nullptr);
    void startDownload(const QUrl &url, const QString &savePath);
    ~DownloadWindow();

private slots:
    void onProgressChange(qint64 bytesrecived, qint64 bytestotal);
    void onDownloadFinish(bool success, const QString &message);

private:
    Ui::DownloadWindow *ui;
    Downloader *download;
    QElapsedTimer downloadTimer;
    qint64 lastBytesReceived = 0;
    QTime lastUpdateTime;
};

#endif // DOWNLOADWINDOW_H
