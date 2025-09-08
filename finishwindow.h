#ifndef FINISHWINDOW_H
#define FINISHWINDOW_H

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

#include <QDialog>

namespace Ui {
class FinishWindow;
}

class FinishWindow : public QDialog
{
    Q_OBJECT

public:
    explicit FinishWindow(QWidget *parent, const QString &url, const QString &path);
    ~FinishWindow();

private slots:
    void on_close_clicked();

    void on_openFolder_clicked();

    void on_open_clicked();

    void on_openWith_clicked();

private:
    Ui::FinishWindow *ui;
    QString path;
};

#endif // FINISHWINDOW_H
