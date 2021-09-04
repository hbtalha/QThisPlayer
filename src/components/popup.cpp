#include "popup.h"
#include <QPainter>
#include <QApplication>
#include <QScreen>
#include <QDebug>
#include <QStyle>

PopUp::PopUp()
{
    setWindowFlags(Qt::FramelessWindowHint |        // Disable window decoration
                   Qt::Tool |                       // Discard display in a separate window
                   Qt::WindowStaysOnTopHint);       // Set on top of all windows
    setAttribute(Qt::WA_TranslucentBackground);     // Indicates that the background will be transparent
    setAttribute(Qt::WA_ShowWithoutActivating);     // At the show, the widget does not get the focus automatically

    popupTextDuration = 2;

    label.setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    label.setStyleSheet("QLabel { color: white;"
                        "font-size:15pt;"
                        "margin-top: 6px;"
                        "margin-bottom: 6px;"
                        "margin-left: 0px;"
                        "margin-right:0px; }");

    layout.addWidget(&label, 0, 0);
    setLayout(&layout);

    timer = new QTimer();
    connect(timer, &QTimer::timeout, this, &PopUp::hide);
}

void PopUp::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRect roundedRect;
    roundedRect.setX(rect().x() + 5);
    roundedRect.setY(rect().y() + 5);
    roundedRect.setWidth(rect().width() - 10);
    roundedRect.setHeight(rect().height() - 10);

    painter.setBrush(QBrush(QColor(20,20,20)));
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(roundedRect, 10, 10);
}

void PopUp::setPopupText(const QString &text)
{
    label.setText(text);
    adjustSize();
}

void PopUp::displayMessage(const QString& text)
{
    setPopupText(text);

    this->move(parentWidget->geometry().topRight() - this->rect().topRight() - QPoint(50, 0));

    this->show();

    timer->start(popupTextDuration * 1000.0);
}

void PopUp::setParentWidget(QWidget *parent)
{
    parentWidget = parent;
}

void PopUp::setPopupTextColor(const QString& color)
{
    textColor = color;
}

void PopUp::setPopupTextDuration(const float sec)
{
    popupTextDuration = sec;
}

void PopUp::setPopupOpacity(const float opacity)
{
    popupOpacity = opacity;

    setWindowOpacity(opacity);
}

float PopUp::getPopupOpacity() const
{
    return popupOpacity;
}
