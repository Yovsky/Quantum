#ifndef FINISHWINDOW_H
#define FINISHWINDOW_H

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
