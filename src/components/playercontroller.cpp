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

#include "playercontroller.h"

#include <QBoxLayout>
#include <QToolButton>
#include <QHBoxLayout>
#include <QLabel>
#include <QStyle>

#include "vlcqt/vlcqt.h"
#include "../shared.h"
#include "../settings.h"

PlayerController::PlayerController(QWidget *parent) : QWidget(parent)
{
//    setAttribute(Qt::WA_NoSystemBackground);
//    setAttribute(Qt::WA_TranslucentBackground);
//    setAttribute(Qt::WA_PaintOnScreen);

    random =  Settings.isRandom();
    loop =    Settings.playlistMode();
    muted =   Settings.isMuted();
    volume =  Settings.volume();

    mediaState = Vlc::Idle;

    this->setMouseTracking(true);
    this->setStyleSheet("QToolButton{background-color: #dddddd;}");

    mediaProgress = new MediaProgressSlider(this);

    mPlayIcon  = new QIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    mPauseIcon = new QIcon(style()->standardIcon(QStyle::SP_MediaPause));

    playButton = new QToolButton;
    playButton->setCursor(Qt::PointingHandCursor);
    setPlayButtonIcon(true);
    playButton->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    playButton->setToolTip(tr("Play\nIf the playlist is empty, open a medium"));
    playButton->setMaximumSize(QSize(36,36));

    QHBoxLayout *playBackLayout = new QHBoxLayout;

    previousButton = new QToolButton;
    previousButton->setCursor(Qt::PointingHandCursor);
    previousButton->setIcon(style()->standardIcon(QStyle::SP_MediaSkipBackward));
    previousButton->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    previousButton->setMaximumSize(QSize(28,28));
    previousButton->setToolTip(tr("Previous media in the playlist"));

    seekBackwardButton = new QToolButton;
    seekBackwardButton->setCursor(Qt::PointingHandCursor);
    seekBackwardButton->setIcon(style()->standardIcon(QStyle::SP_MediaSeekBackward));
    seekBackwardButton->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    seekBackwardButton->setMaximumSize(QSize(28,28));
    seekBackwardButton->setToolTip(tr("Seek backward 10 seconds"));
    seekBackwardButton->setAutoRepeat(true);
    seekBackwardButton->setAutoRepeatDelay(500);

    stopButton = new QToolButton;
    stopButton->setCursor(Qt::PointingHandCursor);
    stopButton->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
    stopButton->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    stopButton->setMaximumSize(QSize(28,28));
    stopButton->setToolTip(tr("Stop playback"));

    seekForwardButton = new QToolButton;
    seekForwardButton->setCursor(Qt::PointingHandCursor);
    seekForwardButton->setIcon(style()->standardIcon(QStyle::SP_MediaSeekForward));
    seekForwardButton->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    seekForwardButton->setMaximumSize(QSize(28,28));
    seekForwardButton->setToolTip(tr("Seek forward 10 seconds"));
    seekForwardButton->setAutoRepeat(true);
    seekForwardButton->setAutoRepeatDelay(500);

    nextButton = new QToolButton;
    nextButton->setCursor(Qt::PointingHandCursor);
    nextButton->setIcon(style()->standardIcon(QStyle::SP_MediaSkipForward));
    nextButton->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    nextButton->setMaximumSize(QSize(28,28));
    nextButton->setToolTip(tr("Next media in the playlist"));

    playBackLayout->addWidget(previousButton);
    playBackLayout->addWidget(seekBackwardButton);
    playBackLayout->addWidget(stopButton);
    playBackLayout->addWidget(seekForwardButton);
    playBackLayout->addWidget(nextButton);
    playBackLayout->setSpacing(1);

    fullScreenButton = new QToolButton;
    setFullScreenButtonIcon(false);
    fullScreenButton->setCursor(Qt::PointingHandCursor);
    fullScreenButton->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    fullScreenButton->setMaximumSize(QSize(28,28));
    fullScreenButton->setToolTip(tr("Toggle the video in full screen"));
    fullScreenButton->setEnabled(false);

    picInPicButton = new QToolButton;
    picInPicButton->setCursor(Qt::PointingHandCursor);
    picInPicButton->setIcon(QIcon(":/images/icons/picture-in-picture.png"));
    picInPicButton->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    picInPicButton->setMaximumSize(QSize(28,28));
    picInPicButton->setToolTip(tr("Toggle the video in picture in picture"));

    extendedSettingsButton = new QToolButton;
    extendedSettingsButton->setCursor(Qt::PointingHandCursor);
    extendedSettingsButton->setIcon(QIcon(":/images/icons/extendedSettings.jpg"));
    extendedSettingsButton->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    extendedSettingsButton->setMaximumSize(QSize(28,28));
    extendedSettingsButton->setToolTip(tr("Show extended settings"));
    extendedSettingsButton->hide();

    QHBoxLayout *settingsLayout = new QHBoxLayout;
    settingsLayout->addWidget(fullScreenButton);
    settingsLayout->addWidget(picInPicButton);
    settingsLayout->addWidget(extendedSettingsButton);
    settingsLayout->setSpacing(0);

    playlistButton = new QToolButton;
    playlistButton->setCursor(Qt::PointingHandCursor);
    playlistButton->setIcon(QIcon(":/images/icons/playlist.png"));
    playlistButton->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    playlistButton->setMaximumSize(QSize(28,28));
    playlistButton->setToolTip(tr("Show/hide playlist"));

    loopButton = new QToolButton;
    loopButton->setCursor(Qt::PointingHandCursor);
    loopButton->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    loopButton->setMaximumSize(QSize(28,28));
    loopButton->setToolTip(tr("Click to toggle between loop all, loop one and no loop"));
    loopButton->setCheckable(true);
    setupLoopButton(loop);

    randomButton = new QToolButton;
    randomButton->setCursor(Qt::PointingHandCursor);
    randomButton->setIcon(QIcon(":/images/icons/random.png"));
    randomButton->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    randomButton->setMaximumSize(QSize(28,28));
    randomButton->setToolTip(tr("Random"));
    randomButton->setCheckable(true);
    randomButton->setChecked(random);

    QHBoxLayout *playListLayout = new QHBoxLayout;
    playListLayout->addWidget(loopButton);
    playListLayout->addWidget(randomButton);
    playListLayout->setSpacing(0);

    chapterListButton = new QToolButton;
    chapterListButton->setIcon(QIcon(":/images/icons/chapterList.png"));
    chapterListButton->setCursor(Qt::PointingHandCursor);
    chapterListButton->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    chapterListButton->setMaximumSize(QSize(28,28));
    chapterListButton->setToolTip(tr("Show/hide chapter list"));

    QToolButton* nextChapterButton = new QToolButton;
    nextChapterButton->setIcon(QIcon(":/images/icons/nextChapter.png"));
    nextChapterButton->setCursor(Qt::PointingHandCursor);
    nextChapterButton->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    nextChapterButton->setToolTip(tr("Next Chapter"));
    nextChapterButton->setMaximumSize(QSize(27,27));
    nextChapterButton->hide();
    connect(nextChapterButton, &QToolButton::clicked, mediaProgress, &MediaProgressSlider::goToNextChapter);

    QToolButton* previousChapterButton = new QToolButton;
    previousChapterButton->setIcon(QIcon(":/images/icons/previousChapter.png"));
    previousChapterButton->setCursor(Qt::PointingHandCursor);
    previousChapterButton->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    previousChapterButton->setToolTip(tr("Previous Chapter"));
    previousChapterButton->setMaximumSize(QSize(27,27));
    previousChapterButton->hide();
    connect(previousChapterButton, &QToolButton::clicked, mediaProgress, &MediaProgressSlider::goToPreviousChapter);

    QHBoxLayout* chaptersButtonLayout = new QHBoxLayout;
    chaptersButtonLayout->addWidget(previousChapterButton);
    chaptersButtonLayout->addWidget(nextChapterButton);
    chaptersButtonLayout->setSpacing(1);

    volButton = new QToolButton;
    volButton->setCursor(Qt::PointingHandCursor);
    volButton->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));
    volButton->setStyleSheet("border:none;background:transparent;");
    volButton->setCheckable(true);
    volButton->setChecked(muted);
    toggleVolButton(muted);
    connect(volButton, &QToolButton::toggled, this, &PlayerController::toggleVolButton);

    mediaVolumeSlider = new MediaVolumeSlider(this);
    mediaVolumeSlider->setValue(volume);

    QHBoxLayout *volumeLayout = new QHBoxLayout;
    volumeLayout->addWidget(volButton);
    volumeLayout->addWidget(mediaVolumeSlider);
    volumeLayout->setSpacing(10);

    QHBoxLayout *bottomLayout = new QHBoxLayout;
    bottomLayout->addWidget(playButton);
    bottomLayout->addLayout(playBackLayout);
    bottomLayout->addLayout(settingsLayout);
    bottomLayout->addLayout(playListLayout);
    bottomLayout->addWidget(chapterListButton);
    bottomLayout->addLayout(chaptersButtonLayout);
    bottomLayout->addStretch();
    bottomLayout->addLayout(volumeLayout);
    bottomLayout->addWidget(playlistButton);
    bottomLayout->setContentsMargins(3,0,3,0);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(mediaProgress);
    mainLayout->addLayout(mediaProgress->timeLabelsLayout());
    mainLayout->addLayout(bottomLayout);
    mainLayout->setContentsMargins(3,0,3,0);

    this->setLayout(mainLayout);

    this->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Fixed);
    this->setMinimumSize(QSize(480,72));
    this->setMaximumHeight(80);

    connect(playButton, &QToolButton::clicked, this, &PlayerController::onPlayClicked);
    connect(stopButton, &QToolButton::clicked, this,&PlayerController::stop);
    connect(fullScreenButton, &QToolButton::clicked, this, &PlayerController::toggleFullScreen);
    connect(picInPicButton, &QToolButton::clicked, this, &PlayerController::togglePicInPicWindow);
    connect(playlistButton, &QToolButton::clicked, this, &PlayerController::togglePlaylist);
    connect(chapterListButton, &QToolButton::clicked, this, &PlayerController::toggleChapterList);
    connect(previousButton, &QToolButton::clicked, this, &PlayerController::previous);
    connect(seekBackwardButton, &QToolButton::clicked, this, &PlayerController::seekBackward);
    connect(nextButton, &QToolButton::clicked, this, &PlayerController::next);
    connect(seekForwardButton, &QToolButton::clicked, this, &PlayerController::seekForward);
    connect(loopButton, &QToolButton::clicked, this, &PlayerController::toggleLoop);
    connect(randomButton, &QToolButton::clicked, this, &PlayerController::onRandomClicked);
    connect(mediaVolumeSlider,&MediaVolumeSlider::valueChanged, this, &PlayerController::volumeChanged);
    connect(mediaVolumeSlider,&MediaVolumeSlider::pressed, volButton, &QToolButton::setChecked);
    connect(mediaProgress, &MediaProgressSlider::chaptersSet, nextChapterButton, &QToolButton::setVisible);
    connect(mediaProgress, &MediaProgressSlider::chaptersSet, previousChapterButton, &QToolButton::setVisible);
}

MediaProgressSlider *PlayerController::mediaProgressSlider() const
{
    return mediaProgress;
}

void PlayerController::setVolume(int vol)
{
    mediaVolumeSlider->setValue(vol);
}

bool PlayerController::isRandom()
{
    return random;
}

bool PlayerController::isMuted()
{
    return muted;
}

int PlayerController::mediaVolume()
{
    return mediaVolumeSlider->value();
}

void PlayerController::toggleVolButton(bool isLoud)
{
    Settings.setMute(isLoud);

    if(isLoud)
    {
        volButton->setIcon(style()->standardIcon(QStyle::SP_MediaVolumeMuted));
        emit muteVolume(true);
    }
    else
    {
        volButton->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));
        emit muteVolume(false);
    }
}

void PlayerController::mediaStateChanged(Vlc::State state)
{
    mediaState = state;

    if(state == Vlc::Opening)
    {
        setPlayButtonIcon(false);
        fullScreenButton->setEnabled(true);
    }
    if(state == Vlc::Paused || state == Vlc::Stopped || state == Vlc::Ended)
    {
        setPlayButtonIcon(true);
    }
    if(state ==  Vlc::Stopped || state == Vlc::Ended)
    {
        fullScreenButton->setEnabled(false);
    }
}

void PlayerController::onPlayClicked()
{
    switch (mediaState)
    {
    case Vlc::Playing:
        emit pause();
        setPlayButtonIcon(true);
        break;
    case Vlc::Paused:
    case Vlc::Stopped:
        emit play();
        setPlayButtonIcon(false);
        break;
    case Vlc::Idle:
    case Vlc::Ended:
        emit playWithNoMedia();
        break;
    default:
        break;
    }
}

int PlayerController::loopOption()
{
    return loop;
}

bool PlayerController::shouldAllowWheelEventOperation()
{
    return ! mediaProgress->underMouse() && ! mediaVolumeSlider->underMouse();
}

QIcon *PlayerController::playIcon() const
{
    return mPlayIcon;
}

QIcon *PlayerController::pauseIcon() const
{
    return mPauseIcon;
}

void PlayerController::setPicInPicView(bool picInPicView)
{
    fullScreenButton->setVisible(! picInPicView);
    extendedSettingsButton->setVisible(! picInPicView);
    playlistButton->setVisible(! picInPicView);
    chapterListButton->setVisible(! picInPicView);
    volButton->setVisible(! picInPicView);
    mediaVolumeSlider->setVisible(! picInPicView);
    picInPicButton->setIcon(picInPicView ? QIcon(":/images/icons/exit-picture-in-picture.png") :  QIcon(":/images/icons/picture-in-picture.png"));

    this->setMinimumSize(QSize( picInPicView ? 0 : 480,  picInPicView ? 0: 72));
}
void PlayerController::toggleLoop()
{
    if(loopButton->property("state").toInt() == NO_LOOP)
    {
        setupLoopButton(LOOP_ALL);
        emit loopToggled(LOOP_ALL);
    }
    else if(loopButton->property("state").toInt() == LOOP_ALL)
    {
        setupLoopButton(LOOP_CURRENT);
        emit loopToggled(LOOP_CURRENT);
    }
    else if(loopButton->property("state").toInt() == LOOP_CURRENT)
    {
        setupLoopButton(NO_LOOP);
        emit loopToggled(NO_LOOP);
    }
}

void PlayerController::setupLoopButton(int mode)
{
    loop = mode;
    Settings.setPlaylistMode(mode);

    if(mode == NO_LOOP)
    {
        loopButton->setProperty("state",QVariant(NO_LOOP));
        loopButton->setIcon(QIcon(":/images/icons/toggleRepeat.png"));
        loopButton->setChecked(false);
    }
    else if(mode == LOOP_ALL)
    {
        loopButton->setProperty("state",QVariant(LOOP_ALL));
        loopButton->setIcon(QIcon(":/images/icons/toggleRepeat.png"));
        loopButton->setChecked(true);
    }
    else if(mode == LOOP_CURRENT)
    {
        loopButton->setProperty("state",QVariant(LOOP_CURRENT));
        loopButton->setIcon(QIcon(":/images/icons/loop_current.png"));
        loopButton->setChecked(true);
    }
}

void PlayerController::onRandomClicked(bool clicked)
{
    Settings.setRandom(clicked);
    random = clicked;
    emit randomToggled(clicked);
}

void PlayerController::setFullScreenButtonIcon(bool isInFullscreen)
{
    fullScreenButton->setIcon( isInFullscreen ? QIcon(":/images/icons/exit-fullscreen.png") : QIcon(":/images/icons/fullscreen.png"));
}

void PlayerController::setPlayButtonIcon(bool playButtonIcon)
{
    if(playButtonIcon)
    {
        playButton->setIcon(*mPlayIcon);
        playButton->setToolTip(tr("Play"));
    }
    else
    {
        playButton->setIcon(*mPauseIcon);
        playButton->setToolTip(tr("Pause the playback"));
    }
}

void PlayerController::setMediProgressSliderMediaPlayer(VlcMediaPlayer *mediaPlayer)
{
    mediaProgress->setMediaPlayer(mediaPlayer);
}

void PlayerController::clickPlayButton()
{
    playButton->click();
}

void PlayerController::clickPreviousButton()
{
    previousButton->click();
}

void PlayerController::clickSeekBackwardButton()
{
    seekBackwardButton->click();
}

void PlayerController::clickStopButton()
{
    stopButton->click();
}

void PlayerController::clickSeekForwardButton()
{
    seekForwardButton->click();
}

void PlayerController::clickNextButton()
{
    nextButton->click();
}

void PlayerController::clickFullScreenButton()
{
    fullScreenButton->click();
}

void PlayerController::clickPicInPicButton()
{
    picInPicButton->click();
}

void PlayerController::clickExtendedSettingsButton()
{
    extendedSettingsButton->click();
}

void PlayerController::clickPlaylistButton()
{
    playlistButton->click();
}

void PlayerController::clickChapterListButton()
{
    chapterListButton->click();
}

void PlayerController::clickLoopButton()
{
    loopButton->click();
}

void PlayerController::clickRandomButton()
{
    randomButton->click();
}

void PlayerController::clickVolButton()
{
    volButton->click();
}

void PlayerController::mouseMoveEvent(QMouseEvent *event)
{
    event->ignore();
    emit mouseMove();
}
