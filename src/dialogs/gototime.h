#ifndef GOTOTIME_H
#define GOTOTIME_H

#include <QDialog>

namespace Ui {
class GoToTime;
}

class GoToTime : public QDialog
{
    Q_OBJECT

public:
    explicit GoToTime(QWidget *parent = nullptr);
    ~GoToTime();

signals:
    void goToTime(qint64 time);

private slots:
    void on_okButton_clicked();
    void on_cancelButton_clicked();
    void on_refreshToolButton_clicked();

private:
    Ui::GoToTime *ui;
};

#endif // GOTOTIME_H
