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
    QFileInfo fileAt(int position);
    void playNext(bool play = true);
    void playPrevious();
    void playCurrent();
    bool isAtEnd();
    void clearPlaylist();
    void setRandom(bool random);
    QString currentFilePlayingPath();
    bool isEmpty();

signals:
    void playSelected(QFileInfo file);
    void mediaChanged(QString filename);
    void message(QString, bool = false);
    void currentPlayingMediaRemoved();
    void mediaNumberChanged();

private slots:
    void onRowsMoved(const QModelIndex &parent, int start, int end, const QModelIndex &destination, int row);
    void popupMenuTableShow(const QPoint &pos);

private:
    void removeSelected();

    QList<QFileInfo> playlistFiles;
    QList<int> currentPlayingList;
    int currentPlayingIndex;
    int currentFilePosition;
    int rowsDifference;
    bool isRandom;
    int previousSelectedRow;

    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
};

#endif // PLAYLISTPAGE_H
