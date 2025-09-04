#ifndef APPGLOBALS_H
#define APPGLOBALS_H

#include <QObject>

class DownloadWindow;
class QDMan;

class AppGlobals : public QObject
{

    Q_OBJECT

public:
    static AppGlobals& instance()
    {
        static AppGlobals instance;
        return instance;
    }

    //For the main window.
    QDMan *MainWindow() const;
    void setMainWindow(QDMan *MainWindow);

    //For the second window.
    DownloadWindow *downloadWindow() const;
    void setDownloadWindow(DownloadWindow *downloadWindow);

signals:
    void downloadWindowCreated(DownloadWindow *newWindow);

private:
    AppGlobals();

    AppGlobals(const AppGlobals&) = delete;
    void operator = (const AppGlobals&) = delete;

    QDMan *m_MainWindow = nullptr;
    DownloadWindow *m_downloadWindow = nullptr;
};

#endif // APPGLOBALS_H
