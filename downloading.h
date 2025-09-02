#ifndef DOWNLOADING_H
#define DOWNLOADING_H

#include <QWidget>

namespace Ui {
class Downloading;
}

class Downloading : public QWidget
{
    Q_OBJECT

public:
    explicit Downloading(QWidget *parent, const QString &adress);
    ~Downloading();

private:
    Ui::Downloading *ui;
    QString urlAdress;
};

#endif // DOWNLOADING_H
