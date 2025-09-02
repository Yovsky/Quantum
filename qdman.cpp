#include "qdman.h"
#include "ui_qdman.h"
#include "urldialog.h"

QDMan::QDMan(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::QDMan)
{
    ui->setupUi(this);
}

QDMan::~QDMan()
{
    delete ui;
}

void QDMan::on_actionAdd_New_Download_triggered()
{
    urlDialog dialog(this);
    dialog.exec();
}

