#include "downloading.h"
#include "ui_downloading.h"

Downloading::Downloading(QWidget *parent, const QString &adress)
    : QWidget(parent)
    , ui(new Ui::Downloading)
    , urlAdress(adress)
{
    ui->setupUi(this);
}

Downloading::~Downloading()
{
    delete ui;
}
