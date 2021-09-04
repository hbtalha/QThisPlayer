#ifndef PLAYERPAGE_H
#define PLAYERPAGE_H

#include <QWidget>

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
    PlaylistPage *playlistPage() const;
    ChapterListPage *chpaterPage() const;
    PlayerController* playerController() const;
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
    void message(QString);
    void mediaStateChanged(Vlc::State state);

public slots:
    void playFile(const QString& fileName);
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

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
private:
    void setupShortcuts();
    void processDroppedChaptersText(QString txt);
//    void chapterizeProgressSlider(QStringList chapters, QList<qint64> timestamps);

    PlayerController *mPlayerController;
    VideoWidget *videoWidget;
    VlcInstance* instance;
    VlcMediaPlayer *player;
    PlaylistPage *playlist;
    ChapterListPage *chapterListPage;
    QStackedWidget *stackedWidget;

    bool isDockedPlaylist;
    bool playerHasMedia;

    int playlistMode;

    friend class MainWindow;
};

#endif // PLAYERPAGE_H
