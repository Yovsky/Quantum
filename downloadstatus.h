#ifndef DOWNLOADSTATUS_H
#define DOWNLOADSTATUS_H

#include <QString>
#include <QMetaType>

struct DownloadStatus
{
    QString fileName;
    QString status;
    QString Date;
    QString fileSize;
    QString currentSize;
    QString speed;
    float disk;
    float progress;
};

Q_DECLARE_METATYPE(DownloadStatus)

#endif // DOWNLOADSTATUS_H
