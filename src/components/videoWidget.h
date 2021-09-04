#ifndef VIDEOWIDGET_H
#define VIDEOWIDGET_H

#include <QVideoWidget>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QPainterPath>
#include <QObject>

class VideoWidget : public QVideoWidget
{
    Q_OBJECT

public:
    VideoWidget(QWidget* parent)
        : QVideoWidget(parent)
    {
        this->setMouseTracking(true);
    }

signals:
    void mouseMove();

protected:
    void mouseMoveEvent(QMouseEvent *event) override
    {
        event->ignore();
        emit mouseMove();
    }
    void paintEvent(QPaintEvent*) override
    {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        QPainterPath path;
        path.addRoundedRect(this->rect(), 0,0);
        QPen pen(Qt::white, 1);
        p.setPen(pen);
        p.fillPath(path,QColor(27,27,27));
        p.drawPath(path);
    }
};

#endif // VIDEOWIDGET_H
