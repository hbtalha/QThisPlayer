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

#ifndef PLAYERCONTROLLER_H
#define PLAYERCONTROLLER_H

#include <QWidget>
#include <QMediaPlayer>
#include <QBoxLayout>

#include "mediaprogressslider.h"
#include "mediavolumeslider.h"

class QLabel;
class QSlider;
class QToolButton;
class MediaSlider;
class MediaSlider;
class MediaProgressSlider;
class VlcMediaPlayer;
class SeekSlider;
class QIcon;


class PlayerController : public QWidget
{
    Q_OBJECT
public:
    enum
    {
        NO_LOOP,
        LOOP_ALL,
        LOOP_CURRENT
    };
    explicit PlayerController(QWidget *parent = nullptr);

    MediaProgressSlider* mediaProgressSlider() const;
    void setVolume(int vol);
    bool isRandom();
    bool isMuted();
    int mediaVolume();
    int loopOption();
    bool shouldAllowWheelEventOperation();
    QIcon *playIcon() const;
    QIcon *pauseIcon() const;
    void setPicInPicView(bool minView);
    void setMediProgressSliderMediaPlayer(VlcMediaPlayer* mediaPlayer);
    void setFullScreenButtonIcon(bool isInFullscreen);
    void syncToVideoTime();

    void clickPlayButton();
    void clickPreviousButton();
    void clickSeekBackwardButton();
    void clickStopButton();
    void clickSeekForwardButton();
    void clickNextButton();
    void clickFullScreenButton();
    void clickPicInPicButton();
    void clickExtendedSettingsButton();
    void clickPlaylistButton();
    void clickChapterListButton();
    void clickLoopButton();
    void clickRandomButton();
    void clickVolButton();

signals:
    void playWithNoMedia();
    void play();
    void pause();
    void stop();
    void previous();
    void seekBackward();
    void next();
    void seekForward();
    void toggleFullScreen();
    void togglePicInPicWindow(bool);
    void togglePlaylist(bool);
    void toggleChapterList(bool);
    void muteVolume(bool);
    void volumeChanged(int);
    void sliderPositionChanged(qint64);
    void loopToggled(int);
    void randomToggled(bool);
    void mouseMove();
    void videoTimeSynced(QString);
    void currentChapterUpdated(QString);

public slots:
    void toggleVolButton(bool isLoud);
    void onPlayClicked();
    void mediaStateChanged(Vlc::State state);
    /*void onExtendedSettingsClicked();*/
    void toggleLoop();
    void setupLoopButton(int mode);
    void onRandomClicked(bool clicked);

private:
    void setPlayButtonIcon(bool playButtonIcon);

    // because when changing media time continuosly when it's paused the app crashes a lot with a QIcon:isNull excptn
    // not sure about what the problem is but using an heap allocated QIcon seem to stop the crash
    QIcon *mPlayIcon;
    QIcon *mPauseIcon;

    SeekSlider* seekSlider;
    MediaProgressSlider * mediaProgress;
    MediaVolumeSlider* mediaVolumeSlider;
    QToolButton *playButton;
    QToolButton *previousButton;
    QToolButton *seekBackwardButton;
    QToolButton *stopButton;
    QToolButton *seekForwardButton;
    QToolButton *nextButton;
    QToolButton *fullScreenButton;
    QToolButton *picInPicButton;
    QToolButton *extendedSettingsButton;
    QToolButton *playlistButton;
    QToolButton *chapterListButton;
    QToolButton *loopButton;
    QToolButton *randomButton;
    QToolButton *volButton;
    Vlc::State mediaState;

    bool muted;
    bool random;
    int loop;
    int volume;

    void mouseMoveEvent(QMouseEvent *event);
};

#endif // PLAYERCONTROLLER_H
