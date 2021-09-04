#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDockWidget>
#include <QTimer>
#include <QResizeEvent>

#include "components/mainpage.h"
#include "components/pictureinpicturewindow.h"
#include "components/popup.h"
#include "dialogs/gototime.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow() {};

private:
    void settingStyleSheet();
    void showPlaylist(bool show);
    void showChapterlist(bool show);
    void restoreWindow();
    void setFullScreen(bool fullScreen);
    void setPicInPicWindow(bool picInPic);
    void toggleFullScreen();
    void onFullScreen();
    void onNormalScreen();
    void showMouse();
    void hideMouse();
    void createMenuAndActions();
    void openFiles(bool play = true);
    void addFilesToPlaylist();
    void addSubtitlesFile();
    void openFilesFromExplorer();

    GoToTime gotoTime;
    PictureInPictureWindow* picInPicWin;
    MainPage* mainPage;
    QDockWidget* playlistDockWidget;
    QDockWidget* chapterDockWidget;
    PopUp *popup;
    QTimer* timerMouse;

    bool isPlaylistShown;
    bool isChapterListShown;
    bool isInPicInPicWindow;

    bool shouldSavesSettings;

    void tests();

    void mousePressEvent(QMouseEvent*) override;
    void resizeEvent(QResizeEvent*) override;
    void moveEvent(QMoveEvent*) override;
    void showEvent(QShowEvent*) override;
//    void changeEvent(QEvent*) override;
};

#endif // MAINWINDOW_H
