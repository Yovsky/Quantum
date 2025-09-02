#ifndef QDMAN_H
#define QDMAN_H

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
