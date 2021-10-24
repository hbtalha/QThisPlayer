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

#ifndef MEDIAPROGRESSSLIDER_H
#define MEDIAPROGRESSSLIDER_H

#include <QObject>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QSlider>
#include <QToolTip>
#include <QRegularExpression>
#include <QLabel>
#include <QHBoxLayout>
#include <QProxyStyle>
#include <QStyleOptionSlider>
#include <QToolButton>
#include <QPainter>
#include <vlcqt/vlcqt.h>
#include <QIcon>

#include "../shared.h"
#include "../settings.h"

int const MIN_SLIDER_VALUE = 0;
int const MAX_SLIDER_VALUE = 10000;

class TimeLabel : public QLabel
{
    Q_OBJECT

public:
    TimeLabel(QString str, QWidget* parent) : QLabel(str, parent) {}

signals:
    void toggleTimeDisplay();

protected:
    void mousePressEvent( QMouseEvent *event ) override
    {
        emit toggleTimeDisplay();
        event->accept();
    }
};

class MediaProgressSlider : public QSlider
{
    Q_OBJECT

public:
    explicit MediaProgressSlider(QWidget *parent = 0)
        : QSlider(parent),
          vlcMediaPlayer(nullptr)
    {
        isLocked = false;
        chaptersPresent = false;
        seeRemainingTimeLabel = Settings.seeRemainingTime();

        this->setOrientation(Qt::Horizontal);
        this->setRange(MIN_SLIDER_VALUE, MAX_SLIDER_VALUE);
        this->setSingleStep( 2 );
        this->setPageStep( 10 );
        this->setTracking( true ); // true by default though
        this->setMouseTracking( true );
        this->setFocusPolicy( Qt::NoFocus );

        chapterLabel = new QLabel(this);
        chapterLabel->setToolTip(tr("Current chapter"));
        labelsLayout = new QHBoxLayout;
        timeElapsed = new QLabel("--:--", this);
        totalOrRemainingTimeLabel = new TimeLabel("--:--", this);
        connect(totalOrRemainingTimeLabel, &TimeLabel::toggleTimeDisplay, this, [this]
        {
            if(isPlayerSeekable())
            {
                seeRemainingTimeLabel = ! seeRemainingTimeLabel;
                Settings.setSeeRemainingTime(seeRemainingTimeLabel);
                totalOrRemainingTimeLabel->setText( seeRemainingTimeLabel ?
                                                    "-" + formattedTime((mediaLength() - vlcMediaPlayer->time())) :
                                                    formattedFullTime());
            }
        });

        labelsLayout->addWidget(timeElapsed);
        labelsLayout->addWidget(chapterLabel);
        labelsLayout->addStretch();
        labelsLayout->addWidget(totalOrRemainingTimeLabel);

        settingStyleSheet();
    }

    ~MediaProgressSlider() {}

    QHBoxLayout * timeLabelsLayout() const;
    void setMediaPlayer(VlcMediaPlayer *player);
    void settingStyleSheet();
    void unSetChapters();
    void setChapters(QStringList chapters, QList<qint64> timestamps);
    qint64 mediaLength();
    QString formattedFullTime();
    void updatePostionIfPlayerPaused();

    void testFunction();

signals:
    void chaptersSet(bool);

public slots:
    void goToNextChapter();
    void goToPreviousChapter();
protected slots:
    void updateCurrentTime(qint64 time);
    void updateCurrentPosition(float position);
    void updateFullTime(qint64 time);
    void onEndOfMedia();

private:
    QString currentChapter(qint64 time, bool shouldUpdateChapterIndex = true);
    float updateEvent(const QPoint &position);
    void lock();
    void unlock();
    qint64 timeInRange(qint64 time);
    bool isPlayerSeekable();
    int getValueFromXPos( int posX );
    float getValuePercentageFromXPos( int posX );
    int getValueFromMediaPlayerTime(qint64 time);

    VlcMediaPlayer *vlcMediaPlayer;
    qint64 length;
    QString fullTime;

    bool isLocked;
    bool _lockIn;
    bool chaptersPresent;
    bool seeRemainingTimeLabel;

    int currentChapterIndex;
    QLabel *timeElapsed;
    TimeLabel *totalOrRemainingTimeLabel;
    QLabel *chapterLabel;
    QHBoxLayout *labelsLayout;

    QStringList mediaChapters;
    QList<qint64> mediaChaptersTimestamps;

    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
};

inline QHBoxLayout * MediaProgressSlider::timeLabelsLayout() const
{
    return labelsLayout;
}

inline bool MediaProgressSlider::isPlayerSeekable()
{
    Vlc::State playerState = vlcMediaPlayer->state();
    return (playerState == Vlc::Playing || playerState == Vlc::Paused);
}

inline void MediaProgressSlider::setMediaPlayer(VlcMediaPlayer *player)
{
    if (vlcMediaPlayer)
    {
        disconnect(vlcMediaPlayer, &VlcMediaPlayer::lengthChanged, this, &MediaProgressSlider::updateFullTime);
        disconnect(vlcMediaPlayer, &VlcMediaPlayer::end, this, &MediaProgressSlider::onEndOfMedia);
        disconnect(vlcMediaPlayer, &VlcMediaPlayer::stopped, this, &MediaProgressSlider::onEndOfMedia);
        disconnect(vlcMediaPlayer, &VlcMediaPlayer::timeChanged, this, &MediaProgressSlider::updateCurrentTime);
        disconnect(vlcMediaPlayer, &VlcMediaPlayer::positionChanged, this, &MediaProgressSlider::updateCurrentPosition);
        disconnect(vlcMediaPlayer, &VlcMediaPlayer::mediaChanged, nullptr, nullptr);
        disconnect(vlcMediaPlayer, &VlcMediaPlayer::seekableChanged, this, &MediaProgressSlider::setEnabled);
    }

    vlcMediaPlayer = player;
    if (player == nullptr)
        return;

    connect(vlcMediaPlayer, &VlcMediaPlayer::lengthChanged, this, &MediaProgressSlider::updateFullTime);
    connect(vlcMediaPlayer, &VlcMediaPlayer::end, this, &MediaProgressSlider::onEndOfMedia);
    connect(vlcMediaPlayer, &VlcMediaPlayer::stopped, this, &MediaProgressSlider::onEndOfMedia);
    connect(vlcMediaPlayer, &VlcMediaPlayer::timeChanged, this, &MediaProgressSlider::updateCurrentTime);
    connect(vlcMediaPlayer, &VlcMediaPlayer::mediaChanged, [this] {settingStyleSheet();});
    connect(vlcMediaPlayer, &VlcMediaPlayer::mediaChanged, this, &MediaProgressSlider::unSetChapters);
    connect(vlcMediaPlayer, &VlcMediaPlayer::seekableChanged, this, &MediaProgressSlider::setEnabled);
    connect(vlcMediaPlayer, &VlcMediaPlayer::positionChanged, this, &MediaProgressSlider::updateCurrentPosition);
}

inline void MediaProgressSlider::settingStyleSheet()
{
    this->setStyleSheet( "QSlider::groove:horizontal {\n"
                         "border: 1px solid #bbb;\n"
                         "background: white;\n"
                         "height: 10px;\n"
                         "border-radius: 4px;\n"
                         "margin-top: -2px;\n"
                         "}\n"
                         "QSlider::sub-page:horizontal {\n"
                         "background-color: #53adcb;\n"
                         "border: 1px solid #777;\n"
                         "height: 10px;\n"
                         "border-radius: 4px;\n"
                         "margin-top: -2px;\n"
                         "}\n"
                         "QSlider::add-page:horizontal {\n"
                         "background: #d7d6d5;\n"
                         "border: 1px solid #777;\n"
                         "height: 10px;\n"
                         "border-radius: 4px;\n"
                         "margin-top: -2px;\n"
                         "}\n"
                         "QSlider::handle:horizontal {\n"
                         "background: qlineargradient(x1:0, y1:0, x2:1, y2:1,\n"
                         "    stop:0 #eee, stop:1 #ccc);\n"
                         "border: 1px solid #777;\n"
                         "width: 13px;\n"
                         "margin-top: -2px;\n"
                         "margin-bottom: -2px;\n"
                         "border-radius: 6px;\n"
                         "}\n"
                         "QSlider::handle:horizontal:hover {\n"
                         "background: qlineargradient(x1:0, y1:0, x2:1, y2:1,\n"
                         "    stop:0 #fff, stop:1 #ddd);\n"
                         "border: 1px solid #444;\n"
                         "border-radius: 6px;\n"
                         "}\n"
                         "QSlider::sub-page:horizontal:disabled {\n"
                         "background: #bbb;\n"
                         "border-color: #999;\n"
                         "}\n"
                         "QSlider::add-page:horizontal:disabled {\n"
                         "background: #eee;\n"
                         "border-color: #999;\n"
                         "}\n"
                         "QSlider::handle:horizontal:disabled {\n"
                         "background: #eee;\n"
                         "border: 1px solid #aaa;\n"
                         "border-radius: 6px;\n"
                         "}\n"
                       );
}

inline float MediaProgressSlider::updateEvent(const QPoint &position)
{
    if (position.x() < this->pos().x() || position.x() > this->pos().x() + this->width())
        return -1;

    float click = position.x() - this->pos().x();
    float op = this->maximum() / this->width();
    float newValue = click * op;

    return newValue;
}

inline void MediaProgressSlider::updateCurrentPosition(float position)
{
    blockSignals(true);
    this->setValue(position * static_cast<float>(maximum()));
    blockSignals(false);
}

inline void MediaProgressSlider::updateCurrentTime(qint64 time)
{
    timeElapsed->setText(formattedTime(time));

    if(chaptersPresent)
    {
        chapterLabel->setText(currentChapter(time));
    }
    if(seeRemainingTimeLabel)
    {
        totalOrRemainingTimeLabel->setText("-" + formattedTime(mediaLength() - time));
    }
}

inline void MediaProgressSlider::updateFullTime(qint64 time)
{
    if (!time)
    {
        totalOrRemainingTimeLabel->setText("--:--");
    }
    else
    {
        fullTime = formattedTime(time);

        QRegularExpression r("(?:(\\d+):)?(\\d+):(\\d+)");
        QRegularExpressionMatch match = r.match(fullTime);
        int hours   =  match.captured(1).isEmpty() ? 0 : match.captured(1).toInt();
        int minutes =  match.captured(2).isEmpty() ? 0 : match.captured(2).toInt();
        int seconds  = match.captured(3).isEmpty() ? 0 : match.captured(3).toInt();
        length = ((hours * 3600000) + (minutes * 60000) + (seconds * 1000));

        if(! seeRemainingTimeLabel)
            totalOrRemainingTimeLabel->setText(fullTime);
    }
}

inline QString MediaProgressSlider::formattedFullTime()
{
    return fullTime;
}

inline void MediaProgressSlider::updatePostionIfPlayerPaused()
{
    if(vlcMediaPlayer->isPaused())
    {
        updateCurrentPosition(vlcMediaPlayer->position());
    }
}

inline qint64 MediaProgressSlider::mediaLength()
{
    return length;
}

inline void MediaProgressSlider::onEndOfMedia()
{
    this->setValue(0);
    timeElapsed->setText("--:--");
    totalOrRemainingTimeLabel->setText("--:--");
    unSetChapters();
}

inline void MediaProgressSlider::goToNextChapter()
{
    if(currentChapterIndex < mediaChaptersTimestamps.size() - 1)
    {
        if(vlcMediaPlayer)
        {
            chapterLabel->setText(currentChapter(vlcMediaPlayer->time()));
            vlcMediaPlayer->setTime(mediaChaptersTimestamps.at(currentChapterIndex + 1));

            updatePostionIfPlayerPaused();
        }
    }
    this->update(); // just in case
}

inline void MediaProgressSlider::goToPreviousChapter()
{
    if(currentChapterIndex > 0)
    {
        if(vlcMediaPlayer)
        {
            vlcMediaPlayer->setTime(mediaChaptersTimestamps.at(currentChapterIndex - 1));
            chapterLabel->setText(currentChapter(vlcMediaPlayer->time()));

            updatePostionIfPlayerPaused();
        }
    }
    this->update(); // just in case
}

inline QString MediaProgressSlider::currentChapter(qint64 time, bool shouldUpdateChapterIndex)
{
    for( int i = 0; i < mediaChaptersTimestamps.size(); ++i)
    {
        try
        {
            qint64 nextTime = (i == (mediaChapters.size() - 1) ) ? mediaLength() : mediaChaptersTimestamps.at(i+1);

            if(time >= mediaChaptersTimestamps.at(i) && time < nextTime)
            {
                if(shouldUpdateChapterIndex)
                {
                    currentChapterIndex = i;
                }
                return mediaChapters.at(i);
                break;
            }
        }
        catch(std::exception& e)
        {
            /// because I can't fucking stop libvlc debug output I have to set my
            /// debug output like this so it can stand out for me to nitice them
            qDebug() << "\n\n****************************************************\n\n";
            qDebug() << "Exception: " << e.what();
            qDebug() << "\n\n****************************************************\n\n";
        }
    }

    return "";
}

inline void MediaProgressSlider::setChapters(QStringList chapters, QList<qint64> timestamps)
{
    emit chaptersSet(true);
    chaptersPresent = true;
    mediaChapters = chapters;
    mediaChaptersTimestamps = timestamps;
    if(vlcMediaPlayer)
    {
        chapterLabel->setText(currentChapter(vlcMediaPlayer->time()));
    }

    this->update();
}

inline void MediaProgressSlider::unSetChapters()
{
    emit chaptersSet(false);
    chapterLabel->clear();
    mediaChaptersTimestamps.clear();
    mediaChapters.clear();
    chaptersPresent = false;
    this->update();
}

inline void MediaProgressSlider::lock()
{
    isLocked = true;
}

inline void MediaProgressSlider::unlock()
{
    isLocked = false;
}

inline int MediaProgressSlider::getValueFromXPos( int posX )
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

inline float MediaProgressSlider::getValuePercentageFromXPos( int posX )
{
    return getValueFromXPos( posX ) / static_cast<float>( maximum() );
}

inline int MediaProgressSlider::getValueFromMediaPlayerTime(qint64 time)
{
    return ((time * maximum()) / mediaLength());
}

inline qint64 MediaProgressSlider::timeInRange(qint64 time)
{
    if(time > mediaLength())
    {
        return  mediaLength();
    }

    if(time < 0)
    {
        return 0;
    }

    return time;
}

inline void MediaProgressSlider::mouseMoveEvent(QMouseEvent *event)
{
    event->ignore();

    int valueFromXPos = ( getValueFromXPos( event->pos().x() ) );

    // convert x pos value to current time
    qint64 newTime = (valueFromXPos * mediaLength()) / static_cast<float>(maximum());

    if(vlcMediaPlayer != nullptr)
    {
        if(isPlayerSeekable())
        {
            QString hoverTime = formattedTime(newTime);

            QString chapter = chaptersPresent ?  currentChapter(newTime, false /*updateCurrentChapterIndex*/) : "";
            QString toolTipText = chapter.isEmpty() ? hoverTime :
                                  "<p style=\"text-align:center;\">" + chapter + "<br>" + hoverTime + "</p>";

            QToolTip::showText(QCursor::pos(), toolTipText, nullptr);
        }
    }

    if (!isLocked)
        return;

    vlcMediaPlayer->setTime(timeInRange(newTime));
    if(vlcMediaPlayer->isPaused())
    {
        this->setValue(valueFromXPos);
    }
}

inline void MediaProgressSlider::mousePressEvent(QMouseEvent *event)
{
    lock();
    if(isPlayerSeekable())
    {
        if(event->button()==Qt::LeftButton)
        {
            int valueFromXPos =( getValueFromXPos( event->pos().x() ) );
            qint64 newTime = (valueFromXPos * mediaLength()) / static_cast<float>(maximum());

            this->setValue(valueFromXPos);
            vlcMediaPlayer->setTime(timeInRange(newTime));//WARNING: possible bug when pressing on near the end
        }
    }
}

inline void MediaProgressSlider::mouseReleaseEvent(QMouseEvent *event)
{
    event->ignore();
    unlock();
}

inline void MediaProgressSlider::wheelEvent(QWheelEvent *event)
{
    if (!vlcMediaPlayer)
        return;

    if (event->angleDelta().y() > 0)
    {
        qint64 newTime =   vlcMediaPlayer->time() + 10000;

        vlcMediaPlayer->setTime(timeInRange(newTime));
    }
    else
    {
        int newTime = vlcMediaPlayer->time() - 10000;
        vlcMediaPlayer->setTime(timeInRange(newTime));
    }

    event->accept();
}

inline void MediaProgressSlider::paintEvent(QPaintEvent *event)
{
    if(chaptersPresent)
    {
        for(int i = 0; i < mediaChaptersTimestamps.size(); ++i)
        {
            int value = getValueFromMediaPlayerTime(mediaChaptersTimestamps.at(i));

            if(value > 0)
            {
                int position = 7 + QStyle::sliderPositionFromValue(minimum(),
                               maximum(),
                               value,
                               width() - 14);

                QPainter painter(this);
                painter.drawLine(position, 0, position, height());
            }
        }
    }

    QSlider::paintEvent(event);
}

inline void MediaProgressSlider::testFunction()
{

}
#endif // MEDIAPROGRESSSLIDER_H
