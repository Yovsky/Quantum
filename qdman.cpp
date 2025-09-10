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

    ui->tableWidget->setColumnCount(5);
    ui->tableWidget->setHorizontalHeaderLabels({"Name", "Size", "Status", "Transfer", "Date"});

    AppGlobals::instance().setMainWindow(this);
    LoadSettings();
    connect(&AppGlobals::instance(), &AppGlobals::downloadWindowCreated, this, &QDMan::onDownloadWindowCreated);
}

void QDMan::SaveSettings()
{
    QJsonArray jsonArray;

    for (int i = 0; i < ui->tableWidget->rowCount(); i++) {
        QJsonArray jsonRow;
        for (int j = 0; j < ui->tableWidget->columnCount(); j++) {
            QTableWidgetItem *cell = ui->tableWidget->item(i, j);
            jsonRow.append(cell ? cell->text() : "");
        }
        jsonArray.append(jsonRow);
    }

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

    ui->tableWidget->setRowCount(0);

    for (int i = 0; i < jsonArray.size(); i++) {
        if (!jsonArray[i].isArray())
            continue;

        QJsonArray jsonRow = jsonArray[i].toArray();
        QStringList items;

        for (int j = 0; j < jsonRow.size(); j++) {
            items.append(jsonRow[j].toString(""));
        }

        int row = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(row);
        InsertItems(items, row);
    }
}

void QDMan::onDownloadWindowCreated(DownloadWindow *dw)
{
    if (dw)
        connect(dw, &DownloadWindow::DownloadInfo, this, &QDMan::SetTable);
}

QDMan::~QDMan()
{
    SaveSettings();
    delete ui;
}

void QDMan::InsertItems(QStringList items, int row)
{
    for (int col = 0; col < items.size() && col < ui->tableWidget->columnCount(); col++) {
        QTableWidgetItem *item = new QTableWidgetItem(items[col]);
        ui->tableWidget->setItem(row, col, item);
    }
}

void QDMan::SetTable(QString Info)
{
    QStringList parts = Info.split("|");
    if (parts.size() != 5 || parts[0].isEmpty()) return;
    if(downloadsList.contains(parts.first()))
    {
        int row = downloadsList.value(parts[0]);

        InsertItems(parts, row);
    }
    else
    {
        int row = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(row);
        downloadsList.insert(parts[0], row);

        InsertItems(parts, row);
    }
}

void QDMan::on_actionAdd_New_Download_triggered()
{
    urlDialog dialog(this);
    dialog.exec();
}
