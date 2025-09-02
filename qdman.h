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

#include <QMainWindow>

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

private slots:
    void on_actionAdd_New_Download_triggered();

private:
    Ui::QDMan *ui;
};
#endif // QDMAN_H
