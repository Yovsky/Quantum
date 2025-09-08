#include "finishwindow.h"
#include "ui_finishwindow.h"
#include <QDesktopServices>
#include <QFileInfo>

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
    this->close();
    QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(path).absolutePath()));
}

