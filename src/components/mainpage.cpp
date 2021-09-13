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

    videoWidget = new VideoWidget(this);
    videoWidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    videoWidget->setMinimumHeight(0);
    QPalette pal= videoWidget->palette();
    pal.setBrush(QPalette::Window,QBrush(QPixmap(":/images/icons/windowIcon.png")));
    videoWidget->setAutoFillBackground(false);
    videoWidget->setPalette(pal);

    instance = new VlcInstance(this);
    player = new VlcMediaPlayer(instance);
    player->setPlaybackRate(1);
    player->setVideoWidget(reinterpret_cast<HWND *>(videoWidget->winId()));

    setAcceptDrops(true);
    mPlayerController = new PlayerController;
    mPlayerController->setMediProgressSliderMediaPlayer(player);
    setPlaylistMode(mPlayerController->loopOption());
    player->setVolume(mPlayerController->mediaVolume());
    player->setMute(mPlayerController->isMuted());

    playlist = new PlaylistPage;
    playlist->setRandom(mPlayerController->isRandom());
    chapterListPage = new ChapterListPage;

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(videoWidget);
    layout->addWidget(mPlayerController);
    layout->setSpacing(0);
    setLayout(layout);

    connect(player, &VlcMediaPlayer::stateChanged, [this] { mPlayerController->mediaStateChanged(player->state()); });
    connect(player, &VlcMediaPlayer::stateChanged, [this] { emit mediaStateChanged(player->state()); });
    connect(player, &VlcMediaPlayer::stopped, [this] { emit mediaChanged(""); emit setFullScreen(false); });
    connect(player, &VlcMediaPlayer::end, this, &MainPage::onEndOfMedia);
    connect(player, &VlcMediaPlayer::mediaChanged, chapterListPage, &ChapterListPage::unsetChapters);
    connect(player, &VlcMediaPlayer::stopped, chapterListPage, &ChapterListPage::unsetChapters);
    connect(mPlayerController, &PlayerController::play, player, &VlcMediaPlayer::play);
    connect(mPlayerController, &PlayerController::playWithNoMedia, this, &MainPage::play);
    connect(mPlayerController, &PlayerController::pause, player, &VlcMediaPlayer::pause);
    connect(mPlayerController, &PlayerController::stop, player, &VlcMediaPlayer::stop);
    connect(mPlayerController, &PlayerController::muteVolume, player,&VlcMediaPlayer::setMute);
    connect(mPlayerController, &PlayerController::volumeChanged, player,&VlcMediaPlayer::setVolume);
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
    connect(videoWidget, &VideoWidget::mouseMove, this, &MainPage::mouseMove);
    connect(playlist, &PlaylistPage::playSelected, this, &MainPage::playFile);
    connect(playlist, &PlaylistPage::mediaChanged, this, &MainPage::mediaChanged);
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
        mPlayerController->clickPlayButton();
        if(isPlayerSeekable())
        {
            emit message(player->isPaused() ? "Play" : "Pause");
        }
    });

    QShortcut* previousShortcut = new QShortcut(QKeySequence(Qt::Key_P), this);
    connect(previousShortcut, &QShortcut::activated, this, [this]
    {
        mPlayerController->clickPreviousButton();
        if(isPlayerSeekable())
        {
            emit message("Previous");
        }
    });

    QShortcut* nextShortcut = new QShortcut(QKeySequence(Qt::Key_N), this);
    connect(nextShortcut, &QShortcut::activated, this, [this]
    {
        mPlayerController->clickNextButton();
        if(isPlayerSeekable())
        {
            emit message("Next");
        }
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
            QString mute = player->isMuted() ? "On" : "Off";
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
            static int sub = player->subtitleCount();

            ++sub;

            if(sub == 0 || sub == 1) // these two seem to have no effect
                sub = 2;

            if(sub > player->subtitleCount())
                sub = -1;

            if(sub == -1)
                emit message("Subtitle track: N/A");
            else
                emit message("Subtitle track: " + player->subtitles().value(sub));

            player->setSubtitle(sub);
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
    QShortcut* pasteToAddChapters = new QShortcut(QKeySequence(QKeySequence::Paste), this);
    connect(pasteToAddChapters, &QShortcut::activated, this, [this]
    {
        if(isPlayerSeekable())
            copyFromClipboard();
    });
}

void MainPage::processDroppedChaptersText(QString txt)
{
    QStringList lines = txt.split("\n", Qt::SkipEmptyParts);

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
        QRegularExpression r("(?:(\\d+):)?(\\d+):(\\d+)\\)?(?:(.*))?");
        QRegularExpressionMatch match = r.match(lines.at(i));
        int hours   =  match.captured(1).isEmpty() ? 0 : match.captured(1).toInt();
        int minutes =  match.captured(2).isEmpty() ? 0 : match.captured(2).toInt();
        int seconds  = match.captured(3).isEmpty() ? 0 : match.captured(3).toInt();
        QString chapterText = match.captured(4).trimmed();

        double milliseconds = ((hours * 3600000) + (minutes * 60000) + (seconds * 1000));

        if((i != 0 && milliseconds != 0.0) || timestamps.isEmpty())
        {
            timestamps.append(milliseconds);
            chapters.append(chapterText);
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
        processDroppedChaptersText(mimeData->text());
    }
}

/*void MainPage::chapterizeProgressSlider(QStringList chapters, QList<qint64> timestamps)
{
    long double total = mplayerController->mediaProgressSlider()->mediaLength();
    double chapterGapWidth =  (total * 0.158747) / 100; // 0.158747 percent out of the length of the media(in ms) ( e.g 450 out of 283470)
    QString baseColor = "rgb(42, 130, 218)";
    QString gapColor = "#000080";
    QString stops = "stop:0.0 " + baseColor + ", stop:";

    QList<qint64> modifiedTimestamps(timestamps);

    if(modifiedTimestamps.first() == 0)
        modifiedTimestamps.pop_front();

    // try to adjust the position of the chapter division on the progress slider to macth
    std::transform(modifiedTimestamps.begin(), modifiedTimestamps.end(), modifiedTimestamps.begin(), [total](qint64 i)
    {
        /// this formula because I made a test with a media of length 283000 and
        /// when I added one second to every chapter timestamp it stayed closer to
        /// the actual position on progress slide (lazy solution I know)
        return i + ((total * 1000) / 283000);
    });

    for(int i = 0; i < modifiedTimestamps.size(); ++i)
    {
        double timeMilSec = (modifiedTimestamps[i] - (chapterGapWidth * 2)) / total;

        stops.append(QString::number(timeMilSec) + " " + baseColor + ",");

        timeMilSec  = (modifiedTimestamps[i] - chapterGapWidth) / total;

        stops.append("stop:" + QString::number(timeMilSec) + " " + gapColor + ", stop:");

        timeMilSec  = modifiedTimestamps[i] / total;

        stops.append(QString::number(timeMilSec) + " " + gapColor + ", stop:" +
                     QString::number(timeMilSec) + " " + baseColor + ", stop:");
    }

    stops.append("1 " + baseColor + ");");

    chapterListPage->setChapters(chapters, timestamps);
    QString backgroundWithStops = "background: qlineargradient(x1:0, y1:0, x2:1, y2:0," + stops;

    emit message("Chapter list added");
}*/

void MainPage::addSubtiles(const QList<QUrl> &urls)
{
    for(auto const& subtitle : urls)
        player->setSubtitleFile(subtitle.toString());

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
        player->takeSnapshot(playlist->currentFilePlayingPath() + "-" +
                             QDateTime::currentDateTime().toString(Qt::ISODateWithMs).remove(":") + ".jpg");
    }
}

void MainPage::setPlayerTime(qint64 time)
{
    if(isPlayerSeekable())
    {
        player->setTime(time);
    }
}

bool MainPage::isPlayerSeekable()
{
    Vlc::State playerState = player->state();
    return (playerState == Vlc::Playing || playerState == Vlc::Paused || playerState == Vlc::Opening);
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

void MainPage::playFile(const QString &fileName)
{
    if(! fileName.isEmpty())
    {
        VlcMedia* _media = new VlcMedia(fileName, true, instance);
        player->setMedia(_media);
        player->play();
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
    if(player->state() == Vlc::Idle or player->state() == Vlc::Ended)
    {
        if(! playlist->isEmpty())
            playlist->playCurrent();
    }
}

void MainPage::pause()
{
    player->pause();
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
            qint64 currentTime = player->time();
            qint64 time =  currentTime + (sec * 1000);

            if(time > mPlayerController->mediaProgressSlider()->mediaLength())
            {
                time = mPlayerController->mediaProgressSlider()->mediaLength();
            }
            player->setTime(time);
        }
    }
}

void MainPage::jumpBackward(int sec)
{
    if(isPlayerSeekable())
    {
        if(sec > 0)
        {
            qint64 currentTime = player->time();
            qint64 time =  currentTime - (sec * 1000);

            if(time < 0)
            {
                time = 0;
            }
            player->setTime(time);
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
    player->setMedia(nullptr);
    playerController()->mediaStateChanged(player->state());
}

void MainPage::onJumpToChapter(qint64 time)
{
    player->setTime(time);
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
            processDroppedChaptersText(mimeData->text());
    }
    else if(mimeData->hasUrls())
    {
        auto urls = mimeData->urls();

        if(isPlayerSeekable())
        {
            if(urls.size() == 1 && (urls.at(0).toLocalFile().endsWith(".txt") || urls.at(0).toLocalFile().endsWith(".ch")))
            {
                auto txtFile = urls.at(0).toLocalFile();

                QFile file(txtFile);
                file.open(QIODevice::ReadOnly | QIODevice::Text);
                QTextStream text(&file);
                processDroppedChaptersText(text.readAll());

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
    if(! mPlayerController->shouldAllowWheelEventOperation() || ! player)
        return;

    if( videoWidget->underMouse() && ! isPlayerSeekable() )
        return;

    if (event->angleDelta().y() > 0)
        increaseVolume();
    else
        decreaseVolume();
}

void MainPage::mouseDoubleClickEvent(QMouseEvent *)
{
    if(videoWidget->underMouse())
    {
        if(isPlayerSeekable())
            emit toggleFullScreen();
    }
}

void MainPage::testFunction()
{
    qDebug() << player->state();
}
