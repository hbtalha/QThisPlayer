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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDockWidget>
#include <QTimer>
#include <QResizeEvent>

#include "components/mainpage.h"
#include "components/pictureinpicturewindow.h"
#include "components/screenmessage.h"
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
    void setPicInPicWindow(bool picInPic, bool exitByClosing = false);
    void toggleFullScreen();
    void onFullScreen();
    void onNormalScreen();
    void showMouse();
    void hideMouse();
    void createMenuAndActions();
    void openFiles(QString caption, bool play = true);
    void addFilesToPlaylist();
    void addSubtitlesFile();
    void addChapterFile();
    void openFilesFromExplorer();

    GoToTime gotoTime;
    PictureInPictureWindow* picInPicWin;
    MainPage* mainPage;
    QDockWidget* playlistDockWidget;
    QDockWidget* chapterDockWidget;
    ScreenMessage *screenMessage;
    QTimer* timerMouse;

    bool isPlaylistShown;
    bool isChapterListShown;
    bool isInPicInPicWindow;

    bool shouldSaveSettings;

    void tests();

    void mousePressEvent(QMouseEvent*) override;
    void resizeEvent(QResizeEvent*) override;
    void moveEvent(QMoveEvent*) override;
    void showEvent(QShowEvent*) override;
//    void changeEvent(QEvent*) override;
};

#endif // MAINWINDOW_H
