#ifndef MEDIAVOLUMESLIDER_H
#define MEDIAVOLUMESLIDER_H

#include <QObject>
#include <QSlider>
#include <QToolTip>
#include <QMouseEvent>
#include <QMouseEvent>
#include <QStyleOptionSlider>
#include <QProxyStyle>

#include "../settings.h"

class MyStyle : public QProxyStyle
{
public:
    using QProxyStyle::QProxyStyle;

    int styleHint(QStyle::StyleHint hint, const QStyleOption* option = 0, const QWidget * widget = 0, QStyleHintReturn *retunData = 0 ) const
    {
        if(hint == QStyle::SH_Slider_AbsoluteSetButtons)
            return (Qt::LeftButton | Qt::MiddleButton | Qt::RightButton);
        return QProxyStyle::styleHint(hint, option, widget, retunData);
    }
};

class MediaVolumeSlider : public QSlider
{
    Q_OBJECT
public:
    MediaVolumeSlider(QWidget* parent)
        :QSlider(parent)
    {
        this->setRange(0, 100);
        this->setMaximumWidth(150);
        this->setOrientation(Qt::Horizontal);

        m_lock = false;

        connect(this, &QSlider::valueChanged, [] (int volume)
        {
            Settings.SetVolume(volume);

        });

        this->setStyle(new MyStyle(this->style()));

        this->setStyleSheet( "QSlider::groove:horizontal {\n"
                             "border: .5px solid #bbb;\n"
                             "background: transparent;\n"
                             "height: 2px;\n"
                             "border-radius: 4px;\n"
                             "margin-top: -2px;\n"
                             "}\n"
                             "QSlider::handle:horizontal {\n"
                             "background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #eee, stop:1 #ccc);\n"
                             "border: 1px solid #777;\n"
                             "width: 8px;\n"
                             "margin-top: -2px;\n"
                             "margin-bottom: -2px;\n"
                             "border-radius: 6px;\n"
                             "}\n"
                             "\n"
                             "QSlider::handle:horizontal:hover {\n"
//                       "background: white;\n"
                             "border: 1px solid #444;\n"
                             "border-radius: 6px;\n"
                             "}");
    }

signals:
    void pressed(bool);

protected:
    void mouseMoveEvent(QMouseEvent *event) override
    {
        event->accept();

        float newValue = getValueFromXPos( event->position().x() );

        if(newValue >= 0 and newValue <= 100)
        {
            QToolTip::showText(QCursor::pos(), QString::number(newValue), nullptr);

            if (!m_lock)
                return;
            this->setValue(newValue);
        }
    }

    void mousePressEvent(QMouseEvent *event) override
    {
        lock();
        float newValue = getValueFromXPos( event->position().x() );
        if(newValue >= 0 and newValue <= 100)
        {
            this->setValue(newValue);
            emit pressed(false /*set volume button unchecked, hence unmute*/);
        }
    }

    void mouseReleaseEvent(QMouseEvent *event) override
    {
        event->ignore();
        unlock();
    }

    void wheelEvent(QWheelEvent *event) override
    {
        event->ignore();

        if (event->angleDelta().y() > 0)
            setValue(value() + 5);
        else
            setValue(value() - 5);
    }

    int getValueFromXPos( int posX )
    {
        QStyleOptionSlider option;
        initStyleOption( &option );
        int  mHandleLength = style()->pixelMetric( QStyle::PM_SliderLength, &option );

        return QStyle::sliderValueFromPosition(
                   minimum(), maximum(),
                   posX    - mHandleLength / 2,
                   width() - mHandleLength,
                   false
               );
    }

    float getValuePercentageFromXPos( int posX )
    {
        return getValueFromXPos( posX ) / static_cast<float>( maximum() );
    }

    bool m_lock;

    void lock()
    {
        m_lock = true;
    }

    void unlock()
    {
        m_lock = false;
    }
};

#endif // MEDIAVOLUMESLIDER_H
