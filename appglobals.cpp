#include "appglobals.h"
#include "qdman.h"
#include "downloadwindow.h"


AppGlobals::AppGlobals() : QObject(nullptr)
{

}

QDMan *AppGlobals::MainWindow() const
{
    return m_MainWindow;
}

void AppGlobals::setMainWindow(QDMan *MainWindow)
{
    m_MainWindow = MainWindow;
}

DownloadWindow *AppGlobals::downloadWindow() const
{
    return m_downloadWindow;
}

void AppGlobals::setDownloadWindow(DownloadWindow *downloadWindow)
{
    m_downloadWindow = downloadWindow;
    emit downloadWindowCreated(m_downloadWindow); // EMIT THE SIGNAL
}
