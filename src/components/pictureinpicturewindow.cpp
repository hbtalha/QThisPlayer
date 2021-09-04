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


