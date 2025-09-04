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

#include "urldialog.h"
#include "downloadwindow.h"
#include "ui_urldialog.h"
#include <QStandardPaths>
#include <QDir>
#include <QMessageBox>
#include <QPushButton>

urlDialog::urlDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::urlDialog)
{
    ui->setupUi(this);
}

urlDialog::~urlDialog()
{
    delete ui;
}

void urlDialog::on_buttonBox_accepted()
{
    QString urlText = ui->Url->text().trimmed();

    QUrl address(urlText);
    if (!address.isValid() || address.scheme().isEmpty())
    {
        QMessageBox::warning(this, "Invalid URL", "Please enter a valid URL (e.g., https://example.com/file.zip)");
        return;
    }

    QString fileName = QFileInfo(address.path()).fileName();
    if (fileName.isEmpty() || fileName == ".")
    {
        fileName = "downloaded_file_" + QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss");
    }

    QString downloadsDir = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
    if (downloadsDir.isEmpty())
    {
        QMessageBox::warning(this, "Error", "Cannot find Downloads directory");
        return;
    }

    QDir dir(downloadsDir);
    if (!dir.exists())
    {
        if (!dir.mkpath("."))
        {
            QMessageBox::warning(this, "Error", "Cannot create Downloads directory: " + downloadsDir);
            return;
        }
    }

    QString savePath = downloadsDir + "/" + fileName;

    QFileInfo file(savePath);
    if (file.isFile() || file.exists())
    {
        QMessageBox msg;
        msg.setWindowTitle("File Already Exists");
        msg.setText("The target file already exists in the directory. \nDo you want to replace it?");
        msg.setIcon(QMessageBox::Warning);
        QPushButton *No = msg.addButton("No", QMessageBox::DestructiveRole);
        QPushButton *Yes = msg.addButton("Yes", QMessageBox::AcceptRole);
        msg.setDefaultButton(Yes);
        msg.exec();
        if (msg.clickedButton() == Yes)
        {
            DownloadWindow *window = new DownloadWindow(nullptr);
            window->startDownload(address, savePath);
            window->setAttribute(Qt::WA_DeleteOnClose);
            window->show();
        }
        if (msg.clickedButton() == No)
            return;
    }
}
