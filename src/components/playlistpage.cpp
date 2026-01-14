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

#include "playlistpage.h"

#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QFile>
#include <QDragMoveEvent>
#include <QDebug>
#include <QShortcut>
#include <QMenu>
#include <QDir>
#include <QDesktopServices>
#include <algorithm>

#include "../shared.h"

PlaylistPage::PlaylistPage()
{
    this->setAcceptDrops(true);
    this->setDragEnabled(true);
    this->setSelectionMode( QAbstractItemView::ExtendedSelection );
    this->setContextMenuPolicy( Qt::CustomContextMenu );
    this->setDragDropMode( QAbstractItemView::InternalMove );
    this->setSelectionBehavior( QAbstractItemView::SelectRows );
    this->setAlternatingRowColors( true );
    this->setDropIndicatorShown(true);

#ifdef Q_OS_WIN
    connect(this->model(), &QAbstractItemModel::rowsInserted, this, &PlaylistPage::mediaNumberChanged);
    connect(this->model(), &QAbstractItemModel::rowsRemoved, this, &PlaylistPage::mediaNumberChanged);
#endif
    connect(this->model(), &QAbstractItemModel::rowsMoved, this, &PlaylistPage::onRowsMoved);
    connect(this, &QListWidget::customContextMenuRequested, this, &PlaylistPage::popupMenuTableShow);
    connect(this, &QListWidget::activated, this, [this] (QModelIndex index)
    {
        currentFilePosition = index.row();
        currentPlayingIndex = currentPlayingList.indexOf(currentFilePosition);
        playCurrent();

        if(isRandom)
        {
            setRandom(isRandom);
        }
    });

    rowsDifference = 0;

    QShortcut* deleteItemShortcut = new QShortcut(QKeySequence(Qt::Key_Delete), this);
    connect(deleteItemShortcut, &QShortcut::activated, this, &PlaylistPage::removeSelected);
}

void PlaylistPage::addFiles(QList<QFileInfo> files, bool play)
{
    if(files.isEmpty())
        return;

    QList<int> tempPlayList;

    for(int i = currentPlayingList.size(); i < (currentPlayingList.size() + files.size()); ++i)
        tempPlayList << i;

    if(files.size() > 0)
    {
        playlistFiles.append(files);
        for(auto const& file : files)
        {
            this->addItem(file.fileName());
            this->item(this->count() - 1)->setToolTip(file.fileName());
        }
    }

    // when files dropped directly into the player, the first file will be played right away
    int firstItem;

    if(play)
    {
        firstItem = tempPlayList.takeFirst();
    }

    if(isRandom && tempPlayList.size() > 1)
        std::random_shuffle(tempPlayList.begin(), tempPlayList.end());

    if(currentPlayingList.isEmpty())
    {
        currentPlayingIndex = 0;
        currentFilePosition = play ? firstItem : tempPlayList.first();
    }

    if(play)
    {
        for(int i = tempPlayList.size() - 1; i >= 0; --i)
            currentPlayingList.insert(currentPlayingIndex, tempPlayList.at(i));

        currentPlayingList.insert(currentPlayingIndex, firstItem);

        currentFilePosition = currentPlayingList.at(currentPlayingIndex);
        playCurrent();
    }
    else
    {
        currentPlayingList.append(tempPlayList);
    }

    previousSelectedRow = currentFilePosition;
}

void PlaylistPage::playFileAtPosition(int position)
{
    if(position >= 0 && position < playlistFiles.size())
    {
        currentFilePosition = position;
        emit playSelected(fileAt(position));
    }
}

QFileInfo PlaylistPage::fileAt(int position)
{
    if(position >= 0 && position < playlistFiles.size())
        return playlistFiles.at(position);
    else
        return QString();
}

void PlaylistPage::playNext(bool play)
{
    if(! playlistFiles.isEmpty())
    {
        if(currentFilePosition == currentPlayingList.last())
        {
            currentFilePosition = currentPlayingList.first();
            currentPlayingIndex = 0;
        }
        else
            currentFilePosition = currentPlayingList.at(++currentPlayingIndex);

        if(play)
        {
            playCurrent();
        }
    }
}

void PlaylistPage::playPrevious()
{
    if(! playlistFiles.isEmpty())
    {
        if(currentFilePosition == currentPlayingList.first())
        {
            currentFilePosition = currentPlayingList.last();
            currentPlayingIndex = currentPlayingList.size() - 1;
        }
        else
            currentFilePosition = currentPlayingList.at(--currentPlayingIndex);

        playCurrent();
    }
}

void PlaylistPage::playCurrent()
{
    emit playSelected(fileAt(currentFilePosition));
    emit mediaChanged(playlistFiles.at(currentFilePosition).fileName());

    if(previousSelectedRow < this->count())
        this->item(previousSelectedRow)->setBackground( ( (previousSelectedRow % 2) == 0 ) ? QColor() : QColor(27,27,27) );

    this->item(currentFilePosition)->setBackground(QColor(115, 147, 179));
    previousSelectedRow = currentFilePosition;
}

bool PlaylistPage::isAtEnd()
{
    if(! currentPlayingList.isEmpty())
        return (currentFilePosition == currentPlayingList.last());
    else
        return true; // no specific reason, could be either
}

void PlaylistPage::clearPlaylist()
{
    this->clear();
    playlistFiles.clear();
    currentPlayingList.clear();
    emit currentPlayingMediaRemoved();
}

void PlaylistPage::setRandom(bool random)
{
    isRandom = random;

    if(! currentPlayingList.isEmpty())
    {
        if(isRandom)
        {
            if(currentFilePosition < currentPlayingList.size() && currentFilePosition >= 0)
            {
                if(currentPlayingList.size() > 2)
                {
                    currentPlayingList.removeOne(currentFilePosition);

                    std::random_shuffle(currentPlayingList.begin(), currentPlayingList.end());

                    currentPlayingList.prepend(currentFilePosition);
                    currentPlayingIndex = 0;
                }
            }
        }
        else
        {
            int positionBefore = currentPlayingList.at(currentPlayingIndex);

            currentPlayingList.clear();
            for(int i = 0; i < playlistFiles.size(); ++i)
                currentPlayingList << i;

            currentPlayingIndex = currentPlayingList.indexOf(positionBefore);
            currentFilePosition = currentPlayingList.at(currentPlayingIndex);
        }
    }
}

QString PlaylistPage::currentFilePlayingPath()
{
    if(! playlistFiles.isEmpty())
        return playlistFiles.at(currentFilePosition).filePath();
    else
        return QString();
}

bool PlaylistPage::isEmpty()
{
    return playlistFiles.isEmpty();
}

void PlaylistPage::onRowsMoved(const QModelIndex &, int start, int end, const QModelIndex &, int row)
{
    if((start >= 0 && start < playlistFiles.size()) && (row >= 0 && row < playlistFiles.size()))
    {
        if(start < row)
            playlistFiles.move(start, row-1);
        else
            playlistFiles.move(start, row);
    }

    if(! this->selectedIndexes().isEmpty())
        (isRandom ? currentFilePosition : currentPlayingIndex) = this->selectedIndexes().last().row() - rowsDifference;

    setRandom(isRandom);

    previousSelectedRow = currentFilePosition;
}

void PlaylistPage::removeSelected()
{
    auto indexes = this->selectedIndexes();

    std::sort(indexes.begin(), indexes.end(), [] (QModelIndex ind, QModelIndex ind2)->bool
    {
        return (ind.row() > ind2.row());
    } );

    for(auto index : indexes)
    {
        int row = index.row();

        if(row == currentFilePosition)
        {
            playNext(false);
            emit currentPlayingMediaRemoved();
        }
        this->takeItem(row);
        playlistFiles.removeAt(row);
    }
}

void PlaylistPage::popupMenuTableShow(const QPoint &pos)
{
    QListWidgetItem* item = this->itemAt(pos);

    if(item != nullptr)
    {
        QMenu contextMenu;

        QAction* playAction = new QAction(QIcon(":/images/icons/play.png"), tr("Play"), this);

        connect(playAction, &QAction::triggered, this, [this, pos]()
        {
            currentFilePosition = this->indexAt(pos).row();
            playCurrent();
        });

        QAction* openContainingFolderAction = new QAction(QIcon(":/images/icons/play.png"), tr("Open containing folder"), this);

        connect(openContainingFolderAction, &QAction::triggered, this, [this, pos]()
        {
            auto path = playlistFiles.at(this->indexAt(pos).row()).absoluteDir().path();
            QDir dir(path);

            if(dir.exists())
                QDesktopServices::openUrl(QUrl::fromLocalFile(path));
            else
                emit message(tr("File not found"), true);
        });

        QAction* removeSelectedAction = new QAction(tr("Remove selected"), this);

        connect(removeSelectedAction, &QAction::triggered, this, &PlaylistPage::removeSelected);

        QAction* clear = new QAction(tr("Clear playlist"), this);

        connect(clear, &QAction::triggered, this, &PlaylistPage::clearPlaylist);

        contextMenu.addAction(playAction);
        contextMenu.addSeparator();
        contextMenu.addAction(openContainingFolderAction);
        contextMenu.addSeparator();
        contextMenu.addAction(removeSelectedAction);
        contextMenu.addAction(clear);

        contextMenu.exec(QCursor::pos());
    }
}

void PlaylistPage::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls())
    {
        event->acceptProposedAction();
    }
    else
    {
        QListWidget::dragEnterEvent(event);
    }
}

void PlaylistPage::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasUrls())
    {
        event->acceptProposedAction();
    }
    else
    {
        if(! this->selectedIndexes().isEmpty())
        {
            rowsDifference = this->selectedIndexes().last().row() - ( isRandom ? currentFilePosition : currentPlayingIndex );
        }

        QListWidget::dragMoveEvent(event);
    }
}

void PlaylistPage::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasUrls())
    {
        addFiles(filterSupportedMediaFormats(event->mimeData()->urls()));
        event->acceptProposedAction();
    }
    QListWidget::dropEvent(event);
}
