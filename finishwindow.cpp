#include "finishwindow.h"
#include "ui_finishwindow.h"

FinishWindow::FinishWindow(QWidget *parent, const QString &url, const QString &path)
    : QDialog(parent)
    , ui(new Ui::FinishWindow)
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

