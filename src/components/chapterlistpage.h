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

#ifndef CHAPTERLISTPAGE_H
#define CHAPTERLISTPAGE_H

#include <QTableWidget>

class QPushButton;

class ChapterListPage : public QTableWidget
{
    Q_OBJECT
public:
    ChapterListPage();

    void setChapters(QStringList chapters, QList<qint64> timestamps);
    void unsetChapters();
    void syncToVideoTime(QString currentChapterTimestamp);
    void updateCurrentChapter(QString currentChapterTimestamp);
    void syncToVideoTimeOnShow();

signals:
    void jumpToChapter(int time);
    void clearChapters();
    void videoTimeSynced();

private slots:
    void popupMenuTableShow(const QPoint &pos);

private:
    QPushButton* syncToVideoTimeButton;
    QList<qint64> timeStamps;
    bool syncOnShow;
    int currentChapterRow;

    void resizeEvent(QResizeEvent *event) override;
    void showEvent(QShowEvent *event) override;
    void mousePressEvent(QMouseEvent* event) override;
};

#endif // CHAPTERLISTPAGE_H
