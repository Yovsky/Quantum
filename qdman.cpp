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

#include "qdman.h"
#include "appglobals.h"
#include "downloadinfo.h"
#include "downloadwindow.h"
#include "ui_qdman.h"
#include "urldialog.h"
#include <QJsonArray>

QDMan::QDMan(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::QDMan)
    , m_settings("Yovsky", "QuantumDownloadManager")
{
    ui->setupUi(this);
    ui->downloadsLayout->setAlignment(Qt::AlignTop);

    // Initialize the temp directory.
    m_qdmTempDir = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/Quantum";
    QDir dir;
    dir.mkdir(m_qdmTempDir);

    // Search for unfinished downloads.
    QStringList filters;
    filters << "*.qdmdata";
    QDirIterator it(m_qdmTempDir, filters, QDir::Files, QDirIterator::Subdirectories);

    while (it.hasNext())
    {
        m_unfinishedDownloads.append(it.next());
    }
    GatherUnfinishedDownsInfo();
    CreateResumeCards();

    // ui->tableWidget->setColumnCount(5);
    // ui->tableWidget->setHorizontalHeaderLabels({"Name", "Size", "Status", "Transfer", "Date"});
    // ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    AppGlobals::instance().setMainWindow(this);
    LoadSettings();
    connect(&AppGlobals::instance(), &AppGlobals::downloadWindowCreated, this, &QDMan::onDownloadWindowCreated);
}

void QDMan::GatherUnfinishedDownsInfo()
{
    for (QString dataFilePath : m_unfinishedDownloads)
    {
        QFile file(dataFilePath);
        if (!file.open(QIODevice::ReadOnly))
            continue;
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        file.close();
        if (!doc.isObject())
        {
            continue;
        }
        QJsonObject root = doc.object();
        downloadInformations item;
        item.url = root["url"].toString();
        item.ID = root["downloadID"].toString();
        item.savePath = root["savePath"].toString();
        item.fileByteSize = root["fileSize"].toInteger();
        item.chunkCount = root["chunkCount"].toInt();
        QVector<qint64> chunkProgress;
        QJsonArray chunks = root["chunks"].toArray();
        for (auto chunk : chunks)
        {
            chunkProgress.append(chunk.toInteger());
        }
        item.chunkProgress = chunkProgress;
        m_resumeDownloads.append(item);
    }
}

void QDMan::CreateResumeCards()
{
    for (const downloadInformations &item : m_resumeDownloads)
    {
        downloadInformations info = item;
        info.status = "Paused.";
        info.fileSize = GetSizeStr(item.fileByteSize);
        qint64 currentSize = 0;
        for (qint64 chunk : item.chunkProgress)
        {
            currentSize += chunk;
        }
        info.currentSize = GetSizeStr(currentSize);
        info.fileName = QFileInfo(item.savePath).fileName();
        if (item.fileByteSize > 0)
            info.progress = (currentSize / item.fileByteSize) * 100;
        info.speed = "- B/s";
        DownloadInfo *card = new DownloadInfo(this);
        ui->downloadsLayout->addWidget(card);
        card->UpdateInfo(info);
        connect(card, &DownloadInfo::resumeRequested, this, &QDMan::onResumeDownload);
    }
}

void QDMan::onResumeDownload(downloadInformations item)
{
    Downloader *downloader = new Downloader();
    downloader->downloadResume(item);
}

QString QDMan::GetSizeStr(qint64 size)
{
    QString result;
    if (size <= 1024)
        result = QString::number(size) + " B";
    else if (size <= (1024 * 1024))
        result = QString::number(size / 1024) + " KB";
    else if (size <= (1024 * 1024 * 1024))
        result = QString::number(size / (1024 * 1024)) + " MB";
    else
        result = QString::number(size / (1024 * 1024 * 1024)) + " GB";
    return result;
}

void QDMan::SaveSettings()
{
    QJsonArray jsonArray;

    // for (int i = 0; i < ui->tableWidget->rowCount(); i++) {
    //     QJsonArray jsonRow;
    //     for (int j = 0; j < ui->tableWidget->columnCount(); j++) {
    //         QTableWidgetItem *cell = ui->tableWidget->item(i, j);
    //         jsonRow.append(cell ? cell->text() : "");
    //     }
    //     jsonArray.append(jsonRow);
    // }

    QJsonDocument doc(jsonArray);
    QByteArray jsonData = doc.toJson(QJsonDocument::Compact);
    m_settings.setValue("TableDataJson", jsonData);
}

void QDMan::LoadSettings()
{
    QByteArray jsonData = m_settings.value("TableDataJson").toByteArray();

    if (jsonData.isEmpty())
        return;

    QJsonDocument doc = QJsonDocument::fromJson(jsonData);
    if (doc.isNull())
        return;
    if (!doc.isArray())
        return;

    QJsonArray jsonArray = doc.array();

    // ui->tableWidget->setRowCount(0);

    for (int i = 0; i < jsonArray.size(); i++) {
        if (!jsonArray[i].isArray())
            continue;

        QJsonArray jsonRow = jsonArray[i].toArray();
        QStringList items;

        for (int j = 0; j < jsonRow.size(); j++) {
            items.append(jsonRow[j].toString(""));
        }

        // int row = ui->tableWidget->rowCount();
        // ui->tableWidget->insertRow(row);
        // InsertItems(items, row);
    }
}

void QDMan::onDownloadWindowCreated(DownloadWindow *dw)
{
    if (dw)
    {
        connect(dw, &DownloadWindow::DownloadInfo, this, &QDMan::SetTable);
    }
}

QDMan::~QDMan()
{
    SaveSettings();
    delete ui;
}

void QDMan::InsertItems(QStringList items, int row)
{
    // for (int col = 0; col < items.size() && col < ui->tableWidget->columnCount(); col++) {
    //     QTableWidgetItem *item = new QTableWidgetItem(items[col]);
    //     ui->tableWidget->setItem(row, col, item);
    // }
}

void QDMan::SetTable(const downloadInformations &Info)
{
    // QStringList parts = Info.split("|");
    // if (parts.size() != 6 || parts[0].isEmpty()) return;
    if(downloadsList.contains(Info.fileName))
    {
        // int row = downloadsList.value(parts[0]);
        // InsertItems(parts, row);

        downloadsList[Info.fileName]->UpdateInfo(Info);
    }
    else
    {
        // int row = ui->tableWidget->rowCount();
        // ui->tableWidget->insertRow(row);
        // downloadsList.insert(parts[0], row);

        auto* downloadInfo = new DownloadInfo(nullptr);
        downloadInfo->UpdateInfo(Info);
        ui->downloadsLayout->addWidget(downloadInfo);
        downloadsList.insert(Info.fileName, downloadInfo);
    }
}

void QDMan::on_actionAdd_New_Download_triggered()
{
    urlDialog dialog(this);
    dialog.exec();
}
