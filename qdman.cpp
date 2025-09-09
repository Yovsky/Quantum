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

QDMan::QDMan(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::QDMan)
{
    ui->setupUi(this);
    AppGlobals::instance().setMainWindow(this);
    connect(&AppGlobals::instance(), &AppGlobals::downloadWindowCreated, this, &QDMan::onDownloadWindowCreated);
}

void QDMan::onDownloadWindowCreated(DownloadWindow *dw)
{
    if (dw)
        connect(dw, &DownloadWindow::DownloadInfo, this, &QDMan::SetTable);
}

QDMan::~QDMan()
{
    delete ui;
}

void QDMan::SetTable(QString Info)
{
    QStringList parts = Info.split("|");
    if (parts.size() != 5 || parts[0].isEmpty()) return;
    if(downloadsList.contains(parts.first()))
    {
        int row = downloadsList.value(parts[0]);

        QTableWidgetItem *name = new QTableWidgetItem(parts[0]);
        ui->tableWidget->setItem(row, 0, name);

        QTableWidgetItem *size = new QTableWidgetItem(parts[1]);
        ui->tableWidget->setItem(row, 1, size);

        QTableWidgetItem *status = new QTableWidgetItem(parts[2]);
        ui->tableWidget->setItem(row, 2, status);

        QTableWidgetItem *transfer = new QTableWidgetItem(parts[3]);
        ui->tableWidget->setItem(row, 3, transfer);

        QTableWidgetItem *date = new QTableWidgetItem(parts[4]);
        ui->tableWidget->setItem(row, 4, date);
    }
    else
    {
        int row = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(row);
        downloadsList.insert(parts[0], row);

        QTableWidgetItem *name = new QTableWidgetItem(parts[0]);
        ui->tableWidget->setItem(row, 0, name);

        QTableWidgetItem *size = new QTableWidgetItem(parts[1]);
        ui->tableWidget->setItem(row, 1, size);

        QTableWidgetItem *status = new QTableWidgetItem(parts[2]);
        ui->tableWidget->setItem(row, 2, status);

        QTableWidgetItem *transfer = new QTableWidgetItem(parts[3]);
        ui->tableWidget->setItem(row, 3, transfer);

        QTableWidgetItem *date = new QTableWidgetItem(parts[4]);
        ui->tableWidget->setItem(row, 4, date);
    }
}

void QDMan::on_actionAdd_New_Download_triggered()
{
    urlDialog dialog(this);
    dialog.exec();
}
