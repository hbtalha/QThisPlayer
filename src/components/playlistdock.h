#ifndef PLAYLISTDOCK_H
#define PLAYLISTDOCK_H

#include <QDockWidget>

class PlaylistDock : public QDockWidget
{
public:
    PlaylistDock();

private:


    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
};

#endif // PLAYLISTDOCK_H
