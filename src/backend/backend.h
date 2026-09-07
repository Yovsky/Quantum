#ifndef BACKEND_H
#define BACKEND_H

#include "src/models/downloadstatus.h"
#include "src/backend/downloadmodel.h"
#include "src/core/downloader.h"
#include "src/models/downloadcategories.h"
#include "src/backend/filenamehandler.h"
#include "src/core/databasemanager.h"

#include <QObject>
#include <QStandardPaths>
#include <QNetworkAccessManager>
#include <QRect>
#include <QGuiApplication>
#include <QScreen>
#include <QFile>
#include <QRegularExpression>
#include <QDesktopServices>
#include <QMessageBox>
#include <QTcpServer>
#include <QTcpSocket>
#include <QElapsedTimer>

class Backend : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString fileName READ fileName NOTIFY fileNameChanged)
    Q_PROPERTY(qint64 fileSize READ fileSize NOTIFY fileSizeChanged)
    Q_PROPERTY(bool isHeadReqActive READ isHeadReqActive NOTIFY isHeadReqActiveChanged)
    Q_PROPERTY(DownloadModel* downloadModel READ downloadModel CONSTANT)
    Q_PROPERTY(int downloadCount READ downloadCount NOTIFY countChanged)
    Q_PROPERTY(int completedCount READ completedCount NOTIFY countChanged)
    Q_PROPERTY(int pausedCount READ pausedCount NOTIFY countChanged FINAL)
    Q_PROPERTY(int activeCount READ activeCount NOTIFY countChanged FINAL)
    Q_PROPERTY(bool headReqCompleted READ headReqCompleted WRITE setHeadReqCompleted NOTIFY headReqCompletedChanged FINAL)

public:
    explicit Backend(QObject *parent = nullptr);

    void StartWebServer();
    Q_INVOKABLE bool downloadRequested(const QString &fileUrl, const QString &fileName, const QString &filePath, const int &connections, const QString &SHA256);
    Q_INVOKABLE void CreateDownload(const QString &fileUrl, const QString &fileName, const QString &filePath, const int &connections, const QString &SHA256);
    Q_INVOKABLE void getHeadInfo(const QString &fileUrl, const QString &targetPath);
    Q_INVOKABLE void buttonClicked(const QString id);
    Q_INVOKABLE void cancelClicked(const QString id);
    Q_INVOKABLE void openRequested(const QString id);
    Q_INVOKABLE void removeRequested(const QString id);
    Q_INVOKABLE void pauseAll();
    Q_INVOKABLE void resumeAll();
    Q_INVOKABLE QRect availableScreenGeometry() const;
    Q_INVOKABLE QString coloredSvg(const QString &path, const QString &color);
    Q_INVOKABLE void setCategory(int category);
    Q_INVOKABLE void setHeadReqCompleted(bool val);

    DownloadCategory detectCategory(const QString &filename);

    QString fileName() const;
    qint64 fileSize() const;
    DownloadModel *downloadModel();
    bool isHeadReqActive() const;
    int downloadCount() const;
    int completedCount() const;
    int pausedCount() const;
    int activeCount() const;
    bool headReqCompleted() const;
    int rowForId(const QString &id) const;

signals:
    void urlRecieved(const QString &url);
    void fileNameChanged();
    void fileSizeChanged();
    void isHeadReqActiveChanged();
    void headReqCompletedChanged();
    void countChanged();

private:
    // Functions
    void loadDownloads();
    void wireDownloadConnections(Downloader *downloader, const downloadInformations &info);

    // Variables
    QTcpServer *m_webServer;
    QNetworkAccessManager *manager;
    QHash<QString, Downloader*> m_activeDownloaders;
    FileNameHandler m_fileNameHandler;
    DatabaseManager *m_databaseManager;
    QString m_fileName;
    qint64 m_fileSize = 0;
    bool m_isHeadReqActive = false;
    bool m_headReqCompleted = false;
    QList<downloadInformations> m_downloads;
    DownloadModel m_downloadModel;
    int m_currentCategory = 0;

};

#endif // BACKEND_H
