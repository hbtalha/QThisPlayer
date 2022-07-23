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

#include "chapterlistpage.h"

#include <QMenu>
#include <QAction>
#include <QPushButton>
#include <QTimer>
#include <QScrollBar>
#include <QHeaderView>

#include "../shared.h"

ChapterListPage::ChapterListPage()
{
    this->setColumnCount(2);
    this->setHorizontalHeaderLabels(QStringList() << tr("Chapter") << tr("Time"));
    this->setColumnWidth(1, 65);
    this->horizontalHeaderItem(1)->setTextAlignment(Qt::AlignLeft);
    this->setSelectionMode(QAbstractItemView::SingleSelection);
    this->setFocusPolicy(Qt::NoFocus);
    this->setSelectionBehavior(QTableWidget::SelectRows);
    this->setAlternatingRowColors(true);
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    this->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    this->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);

    connect(this->verticalScrollBar(), &QScrollBar::valueChanged, this, [this]
    {
        syncToVideoTimeButton->setFixedSize(QSize(114, 25));
    });

    connect(this, &QTableWidget::cellClicked, this, [this] (int row, int )
    {
        emit jumpToChapter(timeStamps.at(row));
    } );

    connect(this, &QTableWidget::customContextMenuRequested, this, &ChapterListPage::popupMenuTableShow);

    syncToVideoTimeButton = new QPushButton(tr("Sync to video time"), this);
    syncToVideoTimeButton->setCursor(Qt::PointingHandCursor);
    syncToVideoTimeButton->setFixedSize(0, 0);
    connect(syncToVideoTimeButton, &QPushButton::clicked, this, &ChapterListPage::videoTimeSynced);
    syncToVideoTimeStyleSheetSet = false;
    syncOnShow = false;
}

void ChapterListPage::setSyncToVideTimeStyleSheet()
{
    syncToVideoTimeButton->setStyleSheet("QPushButton {"
                                         "color: black;"
                                         "background-color: #3498DB;"
                                         "border-width: 2px;"
                                         "border-radius: 10px;"
                                         "border-color: beige;"
                                         "}");
}

void ChapterListPage::setChapters(QStringList chapters, QList<qint64> timestamps)
{
    unsetChapters();

    timeStamps = timestamps;

    for(int i = 0; i < chapters.size(); ++i)
    {
        int row = this->rowCount();
        this->setRowCount(row + 1);
        auto chapterItem = new QTableWidgetItem(chapters.at(i));
        chapterItem->setToolTip(chapters.at(i));
        this->setItem(row, 0, chapterItem);
        this->setItem(row, 1, new QTableWidgetItem(formattedTime(timeStamps.at(i))));

        for(int j = 0; j < 2; ++j)
        {
            QTableWidgetItem* item = this->item(row, j);
            item->setFlags(item->flags() & ~ Qt::ItemIsEditable);
        }
    }
}

void ChapterListPage::unsetChapters()
{
    this->setRowCount(0);
}
#include<QDebug>
void ChapterListPage::syncToVideoTime(QString currentChapterTimestamp)
{
    auto items = this->findItems(currentChapterTimestamp, Qt::MatchExactly);

    if(!items.isEmpty())
    {
        this->scrollToItem(items.first(), QAbstractItemView::PositionAtTop);
        this->selectRow(items.first()->row());

        // if I set it invisible like I wanted it messes up the stylesheet when setting it visible agin idk why
        syncToVideoTimeButton->setFixedSize(0,0);
    }
}

void ChapterListPage::syncToVideoTimeOnShow()
{
    syncOnShow = true;
}

void ChapterListPage::popupMenuTableShow(const QPoint &pos)
{
    QTableWidgetItem* item = this->itemAt(pos);

    if(item != nullptr)
    {
        QMenu contextMenu;

        QAction* jumpToChapterAction = new QAction(tr("Jump to chapter"), this);

        connect(jumpToChapterAction, &QAction::triggered, this, [this, item]
        {
            emit jumpToChapter(timeStamps.at(item->row()));
        });

        QAction* clearChaptersAction = new QAction(tr("Clear chapter"), this);

        connect(clearChaptersAction, &QAction::triggered, this, [this]
        {
            emit clearChapters();
            unsetChapters();
        });

        contextMenu.addAction(jumpToChapterAction);
        contextMenu.addAction(clearChaptersAction);

        contextMenu.exec(QCursor::pos());
    }
}

void ChapterListPage::resizeEvent(QResizeEvent *e)
{
    // setting the stylesheet in the constructor didn't work quite well
    if( ! syncToVideoTimeStyleSheetSet)
    {
        QTimer::singleShot(0, [this]
        {
            setSyncToVideTimeStyleSheet();
            syncToVideoTimeStyleSheetSet = true;
        });
    }

    syncToVideoTimeButton->move(this->geometry().center().x() - 50, this->height() - 50);
    QTableWidget::resizeEvent(e);
}

void ChapterListPage::showEvent(QShowEvent *event)
{
    if(syncOnShow)
        emit videoTimeSynced();
    syncOnShow = false;

    QTableWidget::showEvent(event);
}
