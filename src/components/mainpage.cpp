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

#include "mainpage.h"
#include <QtWidgets>
#include <QtMultimedia>
#include <QGraphicsVideoItem>
#include <QtDebug>
#include <QShortcut>
#include <QTableWidget>
#include <algorithm>
#include <vlcqt/vlcqt.h>

#include "videoWidget.h"
#include "../shared.h"

MainPage::MainPage(QWidget *parent)
    : QWidget(parent)
{
    clipboard = QApplication::clipboard();
    playerHasMedia = false;

    mVideoWidget = new VideoWidget(this);
    mVideoWidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    mVideoWidget->setMinimumHeight(0);
    QPalette pal= mVideoWidget->palette();
    pal.setBrush(QPalette::Window,QBrush(QPixmap(":/images/icons/windowIcon.png")));
    mVideoWidget->setAutoFillBackground(false);
    mVideoWidget->setPalette(pal);

    instance = new VlcInstance(this);
    mPlayer = new VlcMediaPlayer(instance);
    mPlayer->setPlaybackRate(1);
    mPlayer->setVideoWidget(mVideoWidget->winId());

    setAcceptDrops(true);
    mPlayerController = new PlayerController;
    mPlayerController->setMediProgressSliderMediaPlayer(mPlayer);
    setPlaylistMode(mPlayerController->loopOption());
    mPlayer->setVolume(mPlayerController->mediaVolume());
    mPlayer->setMute(mPlayerController->isMuted());

    playlist = new PlaylistPage;
    playlist->setRandom(mPlayerController->isRandom());
    chapterListPage = new ChapterListPage;

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(mVideoWidget);
    layout->addWidget(mPlayerController);
    layout->setSpacing(0);
    setLayout(layout);

    connect(mPlayer, &VlcMediaPlayer::stateChanged, this, [this] { mPlayerController->mediaStateChanged(mPlayer->state()); });
    connect(mPlayer, &VlcMediaPlayer::stateChanged, this, [this] { emit mediaStateChanged(mPlayer->state()); });
    connect(mPlayer, &VlcMediaPlayer::stopped, this, [this] { emit mediaChanged(""); emit setFullScreen(false); });
    connect(mPlayer, &VlcMediaPlayer::end, this, &MainPage::onEndOfMedia);
    connect(mPlayer, &VlcMediaPlayer::mediaChanged, chapterListPage, &ChapterListPage::unsetChapters);
    connect(mPlayer, &VlcMediaPlayer::stopped, chapterListPage, &ChapterListPage::unsetChapters);
    connect(mPlayerController, &PlayerController::play, mPlayer, &VlcMediaPlayer::play);
    connect(mPlayerController, &PlayerController::playWithNoMedia, this, &MainPage::play);
    connect(mPlayerController, &PlayerController::pause, mPlayer, &VlcMediaPlayer::pause);
    connect(mPlayerController, &PlayerController::stop, mPlayer, &VlcMediaPlayer::stop);
    connect(mPlayerController, &PlayerController::muteVolume, mPlayer,&VlcMediaPlayer::setMute);
    connect(mPlayerController, &PlayerController::volumeChanged, mPlayer,&VlcMediaPlayer::setVolume);
    connect(mPlayerController, &PlayerController::seekForward, this, [this] { jumpForward(10); });
    connect(mPlayerController, &PlayerController::seekBackward, this,  [this] { jumpBackward(10); });
    connect(mPlayerController, &PlayerController::next, this, &MainPage::next);
    connect(mPlayerController, &PlayerController::previous, this, &MainPage::previous);
    connect(mPlayerController, &PlayerController::loopToggled, this, &MainPage::setPlaylistMode);
    connect(mPlayerController, &PlayerController::toggleFullScreen, this, &MainPage::toggleFullScreen);
    connect(mPlayerController, &PlayerController::togglePicInPicWindow, this, &MainPage::togglePicInPicWindow);
    connect(mPlayerController, &PlayerController::togglePlaylist, this, &MainPage::togglePlaylistView);
    connect(mPlayerController, &PlayerController::toggleChapterList, this, &MainPage::toggleChapterListView);
    connect(mPlayerController, &PlayerController::randomToggled, playlist, &PlaylistPage::setRandom);
    connect(mPlayerController, &PlayerController::mouseMove, this, &MainPage::mouseMove);
    connect(mVideoWidget, &VideoWidget::mouseMove, this, &MainPage::mouseMove);
    connect(playlist, &PlaylistPage::playSelected, this, &MainPage::playFile);
    connect(playlist, &PlaylistPage::mediaChanged, this, &MainPage::mediaChanged);
    connect(playlist, &PlaylistPage::message, this, &MainPage::message);
    connect(playlist, &PlaylistPage::currentPlayingMediaRemoved, this, &MainPage::resetPlayer);
    connect(chapterListPage, &ChapterListPage::jumpToChapter, this, &MainPage::onJumpToChapter);
    connect(chapterListPage, &ChapterListPage::clearChapters,
            mPlayerController->mediaProgressSlider(), &MediaProgressSlider::unSetChapters);

    setupShortcuts();
}

void MainPage::setupShortcuts()
{
    QShortcut* playPauseShortcut = new QShortcut(QKeySequence(Qt::Key_Space), this);
    connect(playPauseShortcut, &QShortcut::activated, this, [this]
    {
        if(isPlayerSeekable())
        {
            emit message(mPlayer->isPaused() ? "Play" : "Pause");
        }
        mPlayerController->clickPlayButton();
    });

    QShortcut* previousShortcut = new QShortcut(QKeySequence(Qt::Key_P), this);
    connect(previousShortcut, &QShortcut::activated, this, [this]
    {
        if(isPlayerSeekable())
        {
            emit message("Previous");
        }
        mPlayerController->clickPreviousButton();
    });

    QShortcut* nextShortcut = new QShortcut(QKeySequence(Qt::Key_N), this);
    connect(nextShortcut, &QShortcut::activated, this, [this]
    {
        if(isPlayerSeekable())
        {
            emit message("Next");
        }
        mPlayerController->clickNextButton();
    });

    QShortcut* jump10SecondsForwardShortcut = new QShortcut(QKeySequence(Qt::Key_Right), this);
    connect(jump10SecondsForwardShortcut, &QShortcut::activated, this, [this] { jumpForward(10); });

    QShortcut* jump10SecondsBackwardShortcut = new QShortcut(QKeySequence(Qt::Key_Left), this);
    connect(jump10SecondsBackwardShortcut, &QShortcut::activated, this, [this] { jumpBackward(10); });

    QShortcut* jump4SecondsForwardShortcut = new QShortcut(QKeySequence(Qt::SHIFT|Qt::Key_Right), this);
    connect(jump4SecondsForwardShortcut, &QShortcut::activated, this, [this] { jumpForward(4); });

    QShortcut* jump4SecondsBackwardShortcut = new QShortcut(QKeySequence(Qt::SHIFT|Qt::Key_Left), this);
    connect(jump4SecondsBackwardShortcut, &QShortcut::activated, this, [this] { jumpBackward(4); });

    QShortcut* jump1MinuteForwardShortcut = new QShortcut(QKeySequence(Qt::CTRL|Qt::Key_Right), this);
    connect(jump1MinuteForwardShortcut, &QShortcut::activated, this, [this] { jumpForward(60); });

    QShortcut* jump1MinuteBackwardShortcut = new QShortcut(QKeySequence(Qt::CTRL|Qt::Key_Left), this);
    connect(jump1MinuteBackwardShortcut, &QShortcut::activated, this, [this] { jumpBackward(60); });

    QShortcut* stopShortcut = new QShortcut(QKeySequence(Qt::Key_S), this);
    connect(stopShortcut, &QShortcut::activated, this, [this] { mPlayerController->clickStopButton(); });

    QShortcut* muteShortcut = new QShortcut(QKeySequence(Qt::Key_M), this);
    connect(muteShortcut, &QShortcut::activated, this, [this]
    {
        if(isPlayerSeekable())
        {
            mPlayerController->clickVolButton();
            QString mute = mPlayer->isMuted() ? "On" : "Off";
            emit message("Mute: " + mute);
        }
    });
    QShortcut* volumeUpShortcut = new QShortcut(QKeySequence(Qt::Key_Up), this);
    connect(volumeUpShortcut, &QShortcut::activated, this, [this]
    {
        if(isPlayerSeekable())
            increaseVolume();
    });
    QShortcut* volumeDownShortcut = new QShortcut(QKeySequence(Qt::Key_Down), this);
    connect(volumeDownShortcut, &QShortcut::activated, this, [this]
    {
        if(isPlayerSeekable())
            decreaseVolume();
    });
    QShortcut* toggleThroughSubitles = new QShortcut(QKeySequence(Qt::Key_V), this);
    connect(toggleThroughSubitles, &QShortcut::activated, this, [this]
    {
        if(isPlayerSeekable())
        {
            static int sub = mPlayer->subtitleCount();

            ++sub;

            if(sub == 0 || sub == 1) // these two seem to have no effect
                sub = 2;

            if(sub > mPlayer->subtitleCount())
                sub = -1;

            if(sub == -1)
                emit message("Subtitle track: N/A");
            else
                emit message("Subtitle track: " + mPlayer->subtitles().value(sub));

            mPlayer->setSubtitle(sub);
        }
    });
    QShortcut* fullScreenShortcut = new QShortcut(QKeySequence(Qt::Key_F), this);
    connect(fullScreenShortcut, &QShortcut::activated, this, [this]
    {
        if(isPlayerSeekable())
            emit toggleFullScreen();
    });
    QShortcut* exitFullScreenShortcut = new QShortcut(QKeySequence(Qt::Key_Escape), this);
    connect(exitFullScreenShortcut, &QShortcut::activated, this, [this] { emit setFullScreen(false); });

    QShortcut* randomShortcut = new QShortcut(QKeySequence(Qt::Key_R), this);
    connect(randomShortcut, &QShortcut::activated, this, [this]
    {
        mPlayerController->clickRandomButton();
        QString isRandom =  mPlayerController->isRandom() ? "On" : "Off";
        emit message("Random: " + isRandom);
    });

    QShortcut* loopShortcut = new QShortcut(QKeySequence(Qt::Key_L), this);
    connect(loopShortcut, &QShortcut::activated, this, [this]
    {
        mPlayerController->clickLoopButton();
        QStringList loops =  {"Off", "All", "One"};
        emit message("Loop: " + loops.at(mPlayerController->loopOption()));
    });
    QShortcut* pasteToAddChaptersShortcut = new QShortcut(QKeySequence(QKeySequence::Paste), this);
    connect(pasteToAddChaptersShortcut, &QShortcut::activated, this, [this]
    {
        if(isPlayerSeekable())
            copyFromClipboard();
    });
    QShortcut* setInPicInPicWinShortcut = new QShortcut(QKeySequence(Qt::CTRL|Qt::ALT|Qt::Key_P), this);
    connect(setInPicInPicWinShortcut, &QShortcut::activated, this, [this]
    {
        playerController()->clickPicInPicButton();
    });
}

void MainPage::checkForChapterFile(QString filePath)
{
    QString fileTxt = filePath + ".txt";
    QString fileCh = filePath + ".ch";
    if(QFile::exists(fileTxt))
    {
        addChapterFile(fileTxt);
    }
    else if(QFile::exists(fileCh))
    {
        addChapterFile(fileCh);
    }
}

void MainPage::addChapterFile(const QString &filePath)
{
    QFile file(filePath);
    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream text(&file);
        processChaptersText(text.readAll());
    }
}

void MainPage::processChaptersText(QString text)
{
    QStringList lines = text.split("\n", Qt::SkipEmptyParts);

    // ignore lines starting with ';'
    for(int i = lines.size() - 1; i >= 0; --i)
    {
        if(lines.at(i).startsWith(";"))
        {
            lines.removeAt(i);
        }
    }

    QStringList chapters;
    QList<qint64> timestamps;
    for(int i = 0; i < lines.size(); ++i)
    {
        QString line = lines.at(i);
        line.prepend(' '); // force timestamp capture when they are at the beginning of the line

        QRegularExpression timestampRegex("(?:[^a-zA-Z0-9_=:])((?:(\\d{1,2}):)?(\\d{1,2}):(\\d{1,2}))(?:[^a-zA-Z0-9_=:])?");
        QRegularExpression deletionRE("^[^a-zA-Z0-9!'\"_`\\[{(\\?]*|[^a-zA-Z0-9!)'\"_`}\\]\\.\\?]*$");
        QRegularExpressionMatch match = timestampRegex.match(line);

        if(match.hasMatch())
        {
            int hours   =  match.captured(2).isEmpty() ? 0 : match.captured(2).toInt();
            int minutes =  match.captured(3).isEmpty() ? 0 : match.captured(3).toInt();
            int seconds  = match.captured(4).isEmpty() ? 0 : match.captured(4).toInt();
            QString chapterTitle;

            QString part1 = line.mid(0, line.indexOf(match.captured(1)));
            QString part2 = line.mid(line.lastIndexOf(match.captured(1)) + match.captured(1).size(), line.size() - 1);
            line.remove(match.captured(1));
            part1.remove(deletionRE);
            part2.remove(deletionRE);

            if(! part1.isEmpty() && ! part2.isEmpty())
                chapterTitle =   part1 + "." + part2;
            else
                chapterTitle = part1.isEmpty() ? part2 : part1;

            chapterTitle.remove(deletionRE);

            double milliseconds = ((hours * 3600000) + (minutes * 60000) + (seconds * 1000));

            if(milliseconds == 0.0)
            {
                timestamps.clear();
                chapters.clear();
            }

            {
                timestamps.append(milliseconds);
                chapters.append(chapterTitle);
            }
        }
    }

    if(! timestamps.isEmpty())
    {
        qint64 mediaLength = mPlayerController->mediaProgressSlider()->mediaLength();

        // don't add chapter with a timestamp higher than the actuall media length
        for(qint64 i = timestamps.size() - 1; i >= 0; --i)
        {
            if(timestamps.at(i) > mediaLength)
            {
                chapters.removeAt(i);
                timestamps.removeAt(i);
            }
            else
            {
                break;
            }
        }

        if( ! timestamps.isEmpty() )
        {
            mPlayerController->mediaProgressSlider()->setChapters(chapters, timestamps);
            chapterListPage->setChapters(chapters, timestamps);
            emit message("Chapter list added");
        }
    }
}

void MainPage::copyFromClipboard()
{
    const QMimeData *mimeData = clipboard->mimeData();

    if (mimeData->hasText())
    {
        processChaptersText(mimeData->text());
    }
}

void MainPage::addSubtiles(const QList<QUrl> &urls)
{
    for(auto const& subtitle : urls)
        mPlayer->setSubtitleFile(subtitle.toString());

    emit message("Subtitle track added");
}

void MainPage::openFiles(const QList<QUrl> &urls, bool play)
{
    playlist->addFiles(filterSupportedMediaFormats(urls), play);
}

void MainPage::takeSnapshot()
{
    if(isPlayerSeekable())
    {
        mPlayer->takeSnapshot(playlist->currentFilePlayingPath() + "-" +
                              QDateTime::currentDateTime().toString(Qt::ISODateWithMs).remove(":") + ".jpg");
    }
}

void MainPage::setPlayerTime(qint64 time)
{
    if(isPlayerSeekable())
    {
        mPlayer->setTime(time);
    }
}

bool MainPage::isPlayerSeekable()
{
    Vlc::State playerState = mPlayer->state();
    return (playerState == Vlc::Playing || playerState == Vlc::Paused || playerState == Vlc::Opening);
}

VideoWidget *MainPage::videoWidget() const
{
    return mVideoWidget;
}

PlaylistPage *MainPage::playlistPage() const
{
    return playlist;
}

ChapterListPage *MainPage::chpaterPage() const
{
    return chapterListPage;
}

PlayerController *MainPage::playerController() const
{
    return mPlayerController;
}

VlcMediaPlayer *MainPage::player() const
{
    return mPlayer;
}

void MainPage::playFile(const QFileInfo &file)
{
    if(! file.filePath().isEmpty())
    {
        if(QFile::exists(file.filePath()))
        {
            VlcMedia* _media = new VlcMedia(file.filePath(), true, instance);
            mPlayer->setMedia(_media);
            mPlayer->play();

            QTimer::singleShot(500, this, [this, file]
            {
                checkForChapterFile(file.absolutePath() + "/" + file.completeBaseName());
            });
        }
        else
        {
            emit message("File not found", true);
        }
    }
}

void MainPage::increaseVolume()
{
    mPlayerController->setVolume(mPlayerController->mediaVolume() + 5);
    emit message(QString("Volume %1%").arg(mPlayerController->mediaVolume()));
}

void MainPage::decreaseVolume()
{
    mPlayerController->setVolume(mPlayerController->mediaVolume() - 5);
    emit message(QString("Volume %1%").arg(mPlayerController->mediaVolume()));
}

void MainPage::play()
{
    if(mPlayer->state() == Vlc::Idle or mPlayer->state() == Vlc::Ended)
    {
        if(! playlist->isEmpty())
            playlist->playCurrent();
    }
}

void MainPage::pause()
{
    mPlayer->pause();
}

void MainPage::next()
{
    playlist->playNext();
}

void MainPage::previous()
{
    playlist->playPrevious();
}

void MainPage::jumpForward(int sec)
{
    if(isPlayerSeekable())
    {
        if(sec > 0)
        {
            qint64 currentTime = mPlayer->time();
            qint64 time =  currentTime + (sec * 1000);

            if(time > mPlayerController->mediaProgressSlider()->mediaLength())
            {
                time = mPlayerController->mediaProgressSlider()->mediaLength();
            }
            mPlayer->setTime(time);
        }
    }
}

void MainPage::jumpBackward(int sec)
{
    if(isPlayerSeekable())
    {
        if(sec > 0)
        {
            qint64 currentTime = mPlayer->time();
            qint64 time =  currentTime - (sec * 1000);

            if(time < 0)
            {
                time = 0;
            }
            mPlayer->setTime(time);
        }
    }
}

void MainPage::setPlaylistMode(int mode)
{
    playlistMode = mode;
}

void MainPage::onEndOfMedia()
{
    if(Settings.quitAtTheEndOfPlaylist() && playlist->isAtEnd())
    {
        qApp->quit();
    }

    emit mediaChanged("");
    resetPlayer(); // clear the view

    if(playlistMode == PlayerController::NO_LOOP)
    {
        if(! playlist->isAtEnd())
        {
            playlist->playNext();
        }
    }
    else if(playlistMode==PlayerController::LOOP_ALL)
    {
        playlist->playNext();
    }
    else if(playlistMode ==PlayerController::LOOP_CURRENT)
    {
        playlist->playCurrent();
    }
}

void MainPage::resetPlayer()
{
    playerController()->clickStopButton(); // clear the view
    mPlayer->setMedia(nullptr);
    playerController()->mediaStateChanged(mPlayer->state());
}

void MainPage::onJumpToChapter(qint64 time)
{
    mPlayer->setTime(time);
    mPlayerController->mediaProgressSlider()->updatePostionIfPlayerPaused();
}

void MainPage::dragEnterEvent(QDragEnterEvent *event)
{
    if( event->mimeData()->hasFormat("text/plain") || event->mimeData()->hasUrls())
        event->acceptProposedAction();
}

void MainPage::dropEvent(QDropEvent *event)
{
    auto mimeData = event->mimeData();

    if(mimeData->hasFormat("text/plain"))
    {
        if(isPlayerSeekable())
            processChaptersText(mimeData->text());
    }
    else if(mimeData->hasUrls())
    {
        auto urls = mimeData->urls();

        if(isPlayerSeekable())
        {
            if(urls.size() == 1 && (urls.at(0).toLocalFile().endsWith(".txt") || urls.at(0).toLocalFile().endsWith(".ch")))
            {
                addChapterFile(urls.at(0).toLocalFile());
                return;
            }

            // will look for subtitle files
            if(areAllSubtitleFiles(urls))
            {
                addSubtiles(urls);
                return;
            }
        }

        openFiles(mimeData->urls(), /*play*/ true);
    }
}

void MainPage::wheelEvent(QWheelEvent *event)
{
    if(! mPlayerController->shouldAllowWheelEventOperation() || ! mPlayer)
        return;

    if( mVideoWidget->underMouse() && ! isPlayerSeekable() )
        return;

    if (event->angleDelta().y() > 0)
        increaseVolume();
    else
        decreaseVolume();
}

void MainPage::mouseDoubleClickEvent(QMouseEvent *)
{
    if(mVideoWidget->underMouse())
    {
        if(isPlayerSeekable())
            emit toggleFullScreen();
    }
}

void MainPage::testFunction()
{
    qDebug() << mPlayer->state();
}
