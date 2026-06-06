#ifndef DOWNLOADWORKER_H
#define DOWNLOADWORKER_H

#include <QObject>

class DownloadWorker : public QObject
{
    Q_OBJECT
public:
    explicit DownloadWorker(QObject *parent = nullptr);

signals:
};

#endif // DOWNLOADWORKER_H
