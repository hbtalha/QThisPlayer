#ifndef PICTUREINPICTUREWINDOW_H
#define PICTUREINPICTUREWINDOW_H

#include <QMainWindow>
#include <QTimer>

class PictureInPictureWindow : public QMainWindow
{
    Q_OBJECT
public:
    PictureInPictureWindow(QWidget* parent = 0);

    void takeController(QWidget* controller);
    void showMouse();
    void setWinTitle(QString title);

signals:
    void exitPicInPic(bool);

private:
    void hideMouse();

    QWidget* playerController;
    QTimer* timerMouse;

    bool playerControllerPositionLocked;

    void showEvent(QShowEvent *) override;
    void closeEvent(QCloseEvent *event) override;
    void hideEvent(QHideEvent *event) override;
};

#endif // PICTUREINPICTUREWINDOW_H
