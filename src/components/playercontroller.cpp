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
#ifdef Q_OS_WIN
#include <QWinThumbnailToolButton>
#include <QWinThumbnailToolBar>
#endif
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

#ifdef Q_OS_WIN
    playThumbnailButton = nullptr;
#endif

    playButton = new QToolButton;
    setPlayButtonIcon(true);
    playButton->setCursor(Qt::PointingHandCursor);
    playButton->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    playButton->setToolTip(tr("Play\nIf the playlist is empty, open a medium"));
    playButton->setMaximumSize(QSize(36,36));

    QHBoxLayout *playBackLayout = new QHBoxLayout;

    previousButton = new QToolButton;
    setUpCommonToolButton(previousButton, style()->standardIcon(QStyle::SP_MediaSkipBackward), tr("Previous media in the playlist"));

    seekBackwardButton = new QToolButton;
    setUpCommonToolButton(seekBackwardButton,style()->standardIcon(QStyle::SP_MediaSeekBackward), tr("Seek backward 10 seconds"), true);

    stopButton = new QToolButton;    
    setUpCommonToolButton(stopButton, style()->standardIcon(QStyle::SP_MediaStop), tr("Stop playback"));

    seekForwardButton = new QToolButton;
    setUpCommonToolButton(seekForwardButton, style()->standardIcon(QStyle::SP_MediaSeekForward), tr("eek forward 10 seconds"), true);

    nextButton = new QToolButton;
    setUpCommonToolButton(nextButton, style()->standardIcon(QStyle::SP_MediaSkipForward), tr("Next media in the playlist"));

    playBackLayout->addWidget(previousButton);
    playBackLayout->addWidget(seekBackwardButton);
    playBackLayout->addWidget(stopButton);
    playBackLayout->addWidget(seekForwardButton);
    playBackLayout->addWidget(nextButton);
    playBackLayout->setSpacing(1);

    fullScreenButton = new QToolButton;
    setUpCommonToolButton(fullScreenButton, QIcon(":/images/icons/fullscreen.png"), tr("Toggle the video in full screen"));
    fullScreenButton->setEnabled(false);

    picInPicButton = new QToolButton;
    setUpCommonToolButton(picInPicButton, QIcon(":/images/icons/picture-in-picture.png"), tr("Toggle the video in picture in picture"));

    extendedSettingsButton = new QToolButton;
    setUpCommonToolButton(extendedSettingsButton, QIcon(":/images/icons/extendedSettings.png"), tr("how extended settings"));
    extendedSettingsButton->hide();

    QHBoxLayout *settingsLayout = new QHBoxLayout;
    settingsLayout->addWidget(fullScreenButton);
    settingsLayout->addWidget(picInPicButton);
    settingsLayout->addWidget(extendedSettingsButton);
    settingsLayout->setSpacing(0);

    playlistButton = new QToolButton;
    setUpCommonToolButton(playlistButton, QIcon(":/images/icons/playlist.png"), tr("Show/hide playlist"));

    loopButton = new QToolButton;
    setUpCommonToolButton(loopButton, QIcon(":/images/icons/toggleRepeat.png"), tr("Click to toggle between loop all, loop one and no loop"));
    loopButton->setCheckable(true);
    setupLoopButton(loop);

    randomButton = new QToolButton;
    setUpCommonToolButton(randomButton, QIcon(":/images/icons/random.png"), tr("Random"));
    randomButton->setCheckable(true);
    randomButton->setChecked(random);

    QHBoxLayout *playListLayout = new QHBoxLayout;
    playListLayout->addWidget(loopButton);
    playListLayout->addWidget(randomButton);
    playListLayout->setSpacing(0);

    chapterListButton = new QToolButton;
    setUpCommonToolButton(chapterListButton, QIcon(":/images/icons/chapterList.png"), tr("how/hide chapter list"));

    QToolButton* nextChapterButton = new QToolButton;
    setUpCommonToolButton(nextChapterButton, QIcon(":/images/icons/nextChapter.png"), tr("ext Chapte"));
    nextChapterButton->hide();
    connect(nextChapterButton, &QToolButton::clicked, mediaProgress, &MediaProgressSlider::goToNextChapter);

    QToolButton* previousChapterButton = new QToolButton;
    setUpCommonToolButton(previousChapterButton, QIcon(":/images/icons/previousChapter.png"), tr("revious Chapter"));
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
    connect(mediaProgress, &MediaProgressSlider::videoTimeSynced, this, &PlayerController::videoTimeSynced);
    connect(mediaProgress, &MediaProgressSlider::currentChapterUpdated, this, &PlayerController::currentChapterUpdated);
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

void PlayerController::setPicInPicView(bool picInPicView)
{
    fullScreenButton->setVisible(! picInPicView);
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

void PlayerController::onPlaylistMediaNumberChanged(int number)
{
#ifdef Q_OS_WIN
    bool nextPreviousButtonVisible = !(number <= 1);
    previousThumbnailButton->setVisible(nextPreviousButtonVisible);
    nextThumbnailButton->setVisible(nextPreviousButtonVisible);
    playThumbnailButton->setVisible((number != 0));
#endif
}

void PlayerController::setFullScreenButtonIcon(bool isInFullscreen)
{
    fullScreenButton->setIcon( isInFullscreen ? QIcon(":/images/icons/exit-fullscreen.png") : QIcon(":/images/icons/fullscreen.png"));
}

void PlayerController::syncToVideoTime()
{
    mediaProgress->syncToVideoTime();
}

#ifdef Q_OS_WIN
void PlayerController::createWinThumbnailToolBar(QWidget* widget)
{
    thumbnailToolBar = new QWinThumbnailToolBar(widget);
    thumbnailToolBar->setWindow(widget->windowHandle());

    playThumbnailButton = new QWinThumbnailToolButton(thumbnailToolBar);
    connect(playThumbnailButton, &QWinThumbnailToolButton::clicked, this, &PlayerController::onPlayClicked);

    previousThumbnailButton = new QWinThumbnailToolButton(thumbnailToolBar);
    previousThumbnailButton->setIcon(invertedColorIcon(style()->standardIcon(QStyle::SP_MediaSkipBackward)));
    connect(previousThumbnailButton, &QWinThumbnailToolButton::clicked, this, &PlayerController::previous);

    nextThumbnailButton = new QWinThumbnailToolButton(thumbnailToolBar);
    nextThumbnailButton->setIcon(invertedColorIcon(style()->standardIcon(QStyle::SP_MediaSkipForward)));
    connect(nextThumbnailButton, &QWinThumbnailToolButton::clicked, this, &PlayerController::next);

    thumbnailToolBar->addButton(previousThumbnailButton);
    thumbnailToolBar->addButton(playThumbnailButton);
    thumbnailToolBar->addButton(nextThumbnailButton);

    onPlaylistMediaNumberChanged(0);
    setPlayButtonIcon(true);
}
#endif

void PlayerController::setPlayButtonIcon(bool playButtonIcon)
{
    if(playButtonIcon)
    {
        playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
        playButton->setToolTip(tr("Play"));
#ifdef Q_OS_WIN
            playThumbnailButton->setIcon(invertedColorIcon(style()->standardIcon(QStyle::SP_MediaPlay)));
#endif
    }
    else
    {
        playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
        playButton->setToolTip(tr("Pause the playback"));
#ifdef Q_OS_WIN
            playThumbnailButton->setIcon(invertedColorIcon(style()->standardIcon(QStyle::SP_MediaPause)));
#endif
    }
}

void PlayerController::setUpCommonToolButton(QToolButton *button, const QIcon &icon, const QString &tooltip, bool autoRepeat)
{
    button->setCursor(Qt::PointingHandCursor);
    button->setIcon(icon);
    button->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    button->setMaximumSize(QSize(28,28));
    button->setToolTip(tooltip);
    button->setAutoRepeat(autoRepeat);
    button->setAutoRepeatDelay(500);
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
