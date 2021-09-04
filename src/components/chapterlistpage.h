#ifndef CHAPTERLISTPAGE_H
#define CHAPTERLISTPAGE_H

#include <QTableWidget>

class ChapterListPage : public QTableWidget
{
    Q_OBJECT
public:
    ChapterListPage();

    void setChapters(QStringList chapters, QList<qint64> timestamps);
    void unsetChapters();

signals:
    void jumpToChapter(int time);
    void clearChapters();

private slots:
    void popupMenuTableShow(const QPoint &pos);

private:

    QList<qint64> timeStamps;

    void resizeEvent(QResizeEvent *event) override;
};

#endif // CHAPTERLISTPAGE_H
