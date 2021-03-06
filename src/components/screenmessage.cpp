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

#include "screenmessage.h"
#include <QPainter>
#include <QApplication>
#include <QScreen>
#include <QDebug>
#include <QStyle>
#include <QMouseEvent>

ScreenMessage::ScreenMessage()
{
    setWindowFlags(Qt::FramelessWindowHint |        // Disable window decoration
                   Qt::Tool |                       // Discard display in a separate window
                   Qt::WindowStaysOnTopHint);       // Set on top of all windows
    setAttribute(Qt::WA_TranslucentBackground);     // Indicates that the background will be transparent
    setAttribute(Qt::WA_ShowWithoutActivating);     // At the show, the widget does not get the focus automatically

    popupTextDuration = 2;

    label.setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    label.setStyleSheet("QLabel { color: white;"
                        "font-size:15pt;" // TODO: resize according to parent size
                        "margin: 0px;"
                        "padding: 0px;}");

    layout.addWidget(&label, 0, 0);
    setLayout(&layout);

    timer = new QTimer();
    connect(timer, &QTimer::timeout, this, &ScreenMessage::hide);
}

void ScreenMessage::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRect roundedRect;
    roundedRect.setX(rect().x() + 5);
    roundedRect.setY(rect().y() + 5);
    roundedRect.setWidth(rect().width() - 10);
    roundedRect.setHeight(rect().height() - 10);

    QColor color =  (showOption == ShowOption::GENERAL) ? QColor(20,20,20) : QColor(231, 76, 60);
    painter.setBrush(QBrush(color));
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(roundedRect, 10, 10);
}

void ScreenMessage::setPopupText(const QString &text)
{
    label.setText(text);
    adjustSize();
}

void ScreenMessage::displayMessage(const QString& text, ShowOption showOpt)
{
    showOption = showOpt;
    setPopupText(text);

    this->move(viewWidget->geometry().topRight() - this->rect().topRight() - QPoint(50, 0)); // TODO: readjust position

    this->show();

    timer->start(popupTextDuration * 1000.0);
}

void ScreenMessage::setViewWidget(QWidget *parent)
{
    viewWidget = parent;
}

void ScreenMessage::setPopupTextColor(const QString& color)
{
    textColor = color;
}

void ScreenMessage::setPopupTextDuration(const float sec)
{
    popupTextDuration = sec;
}

void ScreenMessage::setPopupOpacity(const float opacity)
{
    popupOpacity = opacity;

    setWindowOpacity(opacity);
}

float ScreenMessage::getPopupOpacity() const
{
    return popupOpacity;
}

void ScreenMessage::wheelEvent(QWheelEvent *event)
{
    if (event->angleDelta().y() > 0)
        emit mouseWheelRolledUp();
    else
        emit mouseWheelRolledDown();
}
