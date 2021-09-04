#ifndef PLAYLISTPAGE_H
#define PLAYLISTPAGE_H

#include <QObject>
#include <QListWidget>
#include <QFileInfo>
#include <QList>

class PlaylistPage : public QListWidget
{
    Q_OBJECT
public:
    PlaylistPage();

    void addFiles(QList<QFileInfo> files, bool play = false);
    void playFileAtPosition(int position);
    QString fileAt(int position);
    void playNext(bool play = true);
    void playPrevious();
    void playCurrent();
    bool isAtEnd();
    void clearPlaylist();
    void setRandom(bool random);
    QString currentFilePlayingPath();
    bool isEmpty();

signals:
    void playSelected(QString filePath);
    void mediaChanged(QString filename);
    void currentPlayingMediaRemoved();

private slots:
    void onRowsMoved(const QModelIndex &parent, int start, int end, const QModelIndex &destination, int row);
    void popupMenuTableShow(const QPoint &pos);

private:
    void removeSelected();

    QList<QFileInfo> playlistFiles;
    QList<int> currentPlayingList;
    int currentPlayingIndex;
    int currentFilePosition;
    bool isRandom;

    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;

    void tests();
};

#endif // PLAYLISTPAGE_H
