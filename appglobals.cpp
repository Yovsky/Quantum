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

DownloadWindow *AppGlobals::Download() const
{
    return m_Download;
}

void AppGlobals::setDownload(DownloadWindow *Download)
{
    m_Download = Download;
}
