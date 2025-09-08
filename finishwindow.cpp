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

#include "finishwindow.h"
#include "ui_finishwindow.h"
#include <QDesktopServices>
#include <QFileInfo>
#include <windows.h>
#include <shellapi.h>
#include <QDir>

FinishWindow::FinishWindow(QWidget *parent, const QString &url, const QString &path)
    : QDialog(parent)
    , ui(new Ui::FinishWindow)
    , path(path)
{
    ui->setupUi(this);
    ui->address->setText(url);
    ui->filePath->setText(path);
}

FinishWindow::~FinishWindow()
{
    delete ui;
}

void FinishWindow::on_close_clicked()
{
    this->close();
}

void FinishWindow::on_openFolder_clicked()
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(path).absolutePath()));

    this->close();
}


void FinishWindow::on_open_clicked()
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(path));

    this->close();
}


void FinishWindow::on_openWith_clicked()
{
    QString absolutePath = QDir::toNativeSeparators(QFileInfo(path).absoluteFilePath());

    SHELLEXECUTEINFO sei;
    ZeroMemory(&sei, sizeof(sei));
    sei.cbSize = sizeof(sei);
    sei.lpVerb = L"openas";
    sei.lpFile = reinterpret_cast<LPCWSTR>(absolutePath.utf16());
    sei.nShow = SW_SHOWNORMAL;
    sei.fMask = SEE_MASK_INVOKEIDLIST;

    ShellExecuteEx(&sei);

    this->close();
}

