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

#include "pictureinpicturewindow.h"

#include <QApplication>
#include <QScreen>
#include <QHideEvent>
#include <QCloseEvent>

PictureInPictureWindow::PictureInPictureWindow(QWidget *parent) :
    QMainWindow(parent)
{
    this->setMouseTracking(false);
    this->resize(389, 317);
    this->setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint);

    QRect screenrect = QApplication::primaryScreen()->geometry();
    this->move((screenrect.right() - width() - 10), (screenrect.bottom() - height() - 70));

    timerMouse = new QTimer(this);
    connect(timerMouse, &QTimer::timeout, this, &PictureInPictureWindow::hideMouse);

    playerController = nullptr;
    playerControllerPositionLocked = true;
}

void PictureInPictureWindow::takeController(QWidget *controller)
{
    playerController = controller;
}

void PictureInPictureWindow::showMouse()
{
    if(playerController != nullptr)
    {
        this->setCursor(Qt::ArrowCursor);
        playerController->setWindowFlags(Qt::WindowStaysOnTopHint);
        playerController->show();
        timerMouse->start(1000);
    }
}

void PictureInPictureWindow::hideMouse()
{
    if(playerController != nullptr)
    {
        timerMouse->stop();
        if( ! playerController->underMouse())
        {
            playerController->hide();
            this->setCursor(Qt::BlankCursor);
        }
    }
}

void PictureInPictureWindow::setWinTitle(QString title)
{
    this->setWindowTitle(title);
}

void PictureInPictureWindow::showEvent(QShowEvent *)
{
    timerMouse->start(1000);

    playerControllerPositionLocked = false;
}

void PictureInPictureWindow::closeEvent(QCloseEvent *event)
{
    this->hide();
    emit exitPicInPic(false);
    event->ignore();
}

void PictureInPictureWindow::hideEvent(QHideEvent *event)
{
    timerMouse->stop();
    playerController = nullptr;

    event->accept();
}


