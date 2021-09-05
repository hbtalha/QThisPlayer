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
