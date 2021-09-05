/****************************************************************************
* QThisPlayer - media player
* Copyright (C) 2021 Helder Batalha <helderbatalha3@gmail.com>
*
* This library is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published
* by the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this library. If not, see <http://www.gnu.org/licenses/>.
*****************************************************************************/

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
