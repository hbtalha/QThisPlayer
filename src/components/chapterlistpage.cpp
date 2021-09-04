#include "chapterlistpage.h"

#include <QMenu>
#include <QAction>

#include "../shared.h"

ChapterListPage::ChapterListPage()
{
    this->setSelectionMode(QAbstractItemView::SingleSelection);
    this->setFocusPolicy(Qt::NoFocus);
    this->setSelectionBehavior(QTableWidget::SelectRows);
    this->setAlternatingRowColors(true);
    this->setContextMenuPolicy(Qt::CustomContextMenu);

    this->setColumnCount(2);
    this->setHorizontalHeaderLabels(QStringList() << tr("Chapter") << tr("Time"));
    this->setColumnWidth(1, 90);
    this->horizontalHeaderItem(1)->setTextAlignment(Qt::AlignLeft);

    connect(this, &QTableWidget::cellDoubleClicked, this, [this] (int row, int )
    {
        emit jumpToChapter(timeStamps.at(row));
    } );

    connect(this, &QTableWidget::customContextMenuRequested, this, &ChapterListPage::popupMenuTableShow);
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

void ChapterListPage::popupMenuTableShow(const QPoint &pos)
{
    QTableWidgetItem* item = this->itemAt(pos);

    if(item != nullptr)
    {
        QMenu contextMenu;

        QAction* jumpToChapterAction = new QAction(tr("Jump to chapter"), this);

        connect(jumpToChapterAction, &QAction::triggered, [this, item]
        {
            emit jumpToChapter(timeStamps.at(item->row()));
        });

        QAction* clearChaptersAction = new QAction(tr("Clear chapter"), this);

        connect(clearChaptersAction, &QAction::triggered, [this]
        {
            emit clearChapters();
            unsetChapters();
        });

        contextMenu.addAction(jumpToChapterAction);
        contextMenu.addAction(clearChaptersAction);

        contextMenu.exec(QCursor::pos());
    }
}

void ChapterListPage::resizeEvent(QResizeEvent *)
{
    this->setColumnWidth(0, this->width() - 90);
}
