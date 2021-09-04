#include "gototime.h"
#include "ui_gototime.h"

GoToTime::GoToTime(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GoToTime)
{
    ui->setupUi(this);

    this->setFixedSize(this->size());
}

GoToTime::~GoToTime()
{
    delete ui;
}

void GoToTime::on_okButton_clicked()
{
    QTime time = ui->timeEdit->time();
    quint64 timeInMillSec = (time.hour() * 3600000) + (time.minute() * 60000) + (time.second() * 1000);

    emit goToTime(timeInMillSec);

    this->hide();
}


void GoToTime::on_cancelButton_clicked()
{
    this->hide();
}

void GoToTime::on_refreshToolButton_clicked()
{
    ui->timeEdit->setTime(QTime(0,0,0));
}
