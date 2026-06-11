#ifndef QDMAN_H
#define QDMAN_H

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

#include "src/ui/downloadwindow.h"
#include "src/ui/downloadinfo.h"
#include "src/models/downloadstatus.h"
#include <QMainWindow>
#include <QSettings>
#include <QStandardPaths>
#include <QDir>
#include <QDirIterator>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

QT_BEGIN_NAMESPACE
namespace Ui {
class QDMan;
}
QT_END_NAMESPACE

class QDMan : public QMainWindow
{
    Q_OBJECT

public:
    QDMan(QWidget *parent = nullptr);
    ~QDMan();
    void GatherUnfinishedDownsInfo();
    void CreateResumeCards();
    QString GetSizeStr(qint64 size);
    void onResumeDownload(downloadInformations item);
    void LoadSettings();
    void SaveSettings();
    void InsertItems(QStringList items, int row);

private slots:
    void onDownloadWindowCreated(DownloadWindow *dw);

public slots:
    void SetTable(const downloadInformations& Info);

private slots:
    void on_actionAdd_New_Download_triggered();

private:
    Ui::QDMan *ui;
    QHash<QString, DownloadInfo*> downloadsList;
    QSettings m_settings;
    QString m_qdmTempDir;
    QStringList m_unfinishedDownloads;
    QVector<downloadInformations> m_resumeDownloads;
};
#endif // QDMAN_H
