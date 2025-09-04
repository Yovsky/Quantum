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
    ui->nuh->setText(Info);
}

void QDMan::on_actionAdd_New_Download_triggered()
{
    urlDialog dialog(this);
    dialog.exec();
}
