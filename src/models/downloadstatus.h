#ifndef DOWNLOADSTATUS_H
#define DOWNLOADSTATUS_H

#include <QString>
#include <QMetaType>
#include <QList>
#include <QFile>

/*
* struct DownloadStatus
* {
*     QString fileName;
*     QString status;
*     QString Date;
*     QString fileSize;
*     QString currentSize;
*     QString speed;
*     float disk;
*     float progress;
* };
* struct resumeDownload
* {
*     QString ID;
*     QString url;
*     QString savePath;
*     qint64 fileSize;
*     int chunkCount;
*     QVector<qint64> chunkProgress;
* };
*/
struct downloadInformations
{
    // Display info:
    QString fileName;
    QString status;
    QString Date;
    QString fileSize;
    QString currentSize;
    QString speed;
    float disk;
    float progress;

    // Technical info:
    QString ID;
    QString url;
    QString savePath;
    QString tempPath;
    qint64 fileByteSize;
    int chunkCount;
    QVector<qint64> chunkProgress;
    QString SHA256;
};

Q_DECLARE_METATYPE(downloadInformations)

#endif // DOWNLOADSTATUS_H
