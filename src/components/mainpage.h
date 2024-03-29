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

#ifndef PLAYERPAGE_H
#define PLAYERPAGE_H

#include <QWidget>
#include <QClipboard>
#include <QElapsedTimer>

class VideoWidget;
class QStackedWidget;
class QTableWidget;

#include "vlcqt/vlcqt.h"
#include "playercontroller.h"
#include "playlistpage.h"
#include "chapterlistpage.h"

class MainPage : public QWidget
{
    Q_OBJECT
public:
    explicit MainPage(QWidget *parent = nullptr);

    bool isPlayerSeekable();
    VideoWidget* videoWidget() const;
    PlaylistPage *playlistPage() const;
    ChapterListPage *chpaterPage() const;
    PlayerController* playerController() const;
    VlcMediaPlayer * player() const;
    void addSubtiles(const QList<QUrl> &urls);
    void openFiles(const QList<QUrl>& urls, bool play);
    void takeSnapshot();
    void setPlayerTime(qint64 time);

    void testFunction();

signals:
    void muted(bool);
    void mediaChanged(const QString&);
    void togglePlaylistView();
    void toggleChapterListView();
    void toggleFullScreen();
    void togglePicInPicWindow();
    void setFullScreen(bool);
    void mouseMove();
    void message(QString, bool = false);
    void mediaStateChanged(Vlc::State state);

public slots:
    void playFile(const QFileInfo& file);
    void increaseVolume();
    void decreaseVolume();
    void play();
    void pause();
    void next();
    void previous();
    void jumpForward(int sec);
    void jumpBackward(int sec);
    void setPlaylistMode(int);
    void onEndOfMedia();
    void resetPlayer();
    void onJumpToChapter(qint64 time);
    void addChapterFile(const QString& filePath);

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event) override;
private:
    void setupShortcuts();
    void processChaptersText(QString text);
    void copyFromClipboard();
    void checkForChapterFile(QString filePath);

    int volumeAdjuster(int vol, int incrementOrDecrement);

    PlayerController *mPlayerController;
    VideoWidget *mVideoWidget;
    VlcInstance* instance;
    VlcMediaPlayer *mPlayer;
    PlaylistPage *playlist;
    ChapterListPage *chapterListPage;
    QClipboard* clipboard;
    QElapsedTimer clickElapsedTimer;

    bool rightMouseButtonPressed;
    bool shouldCancelSingleClick;
    bool isDockedPlaylist;
    bool playerHasMedia;

    int playlistMode;
};

#endif // PLAYERPAGE_H
