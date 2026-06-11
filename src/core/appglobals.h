#ifndef APPGLOBALS_H
#define APPGLOBALS_H

/*
* Quantum, A free and open-source download manager that is written in C++
* Copyright (C) 2025  Yovsky <Yovsky@proton.me>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

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
