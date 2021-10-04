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

#include "mainwindow.h"

#include <windows.h>
#include <QAction>
#include <QMenuBar>
#include <QApplication>
#include <QMouseEvent>
#include <QScreen>
#include <QStackedWidget>
#include <QToolButton>
#include <QFileDialog>
#include <QUrl>

#include "components/videoWidget.h"
#include "components/playercontroller.h"
#include "settings.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QString path = qApp->applicationDirPath() + "/lib/vlc";
    if (qEnvironmentVariableIsEmpty("VLC_PLUGIN_PATH"))
    {
        qputenv("VLC_PLUGIN_PATH", path.toLocal8Bit());
    }

    mainPage = new MainPage;

    //  tests();

    createMenuAndActions();

    this->setCentralWidget(mainPage);
    this->setMouseTracking(true);
    settingStyleSheet();

    isPlaylistShown = false;
    isChapterListShown = false;
    isInPicInPicWindow = false;
    shouldSavesSettings = false;

    timerMouse = new QTimer(this);
    connect(timerMouse, &QTimer::timeout, this, &MainWindow::hideMouse);

    connect(mainPage, &MainPage::mouseMove, this, &MainWindow::showMouse);
    connect(mainPage, &MainPage::setFullScreen, this, &MainWindow::setFullScreen);
    connect(mainPage, &MainPage::mediaChanged, this, [this] (const QString& filename)
    {
        QString title = filename.isEmpty() ? qAppName() : (filename + " - " + qAppName());
        this->setWindowTitle(title);
        picInPicWin->setWinTitle(title);
    });

    connect(mainPage, &MainPage::togglePicInPicWindow, this, [this]
    {
        setPicInPicWindow(!isInPicInPicWindow);
    });
    connect(mainPage, &MainPage::toggleFullScreen, this, [this]
    {
        if(! isInPicInPicWindow)
            setFullScreen(!isFullScreen());
    });
    connect(mainPage, &MainPage::togglePlaylistView, this, [this]
    {
        showPlaylist(! playlistDockWidget->isVisible());
    });
    connect(mainPage, &MainPage::toggleChapterListView, this, [this]
    {
        showChapterlist(!chapterDockWidget->isVisible());
    });
    connect(mainPage, &MainPage::message, this, [this] (QString message)
    {
        screenMessage->displayMessage(message);
    });
    connect(&gotoTime, &GoToTime::goToTime, mainPage, &MainPage::setPlayerTime);

    screenMessage = new ScreenMessage();
    screenMessage->setParentWidget(this);
    connect(screenMessage, &ScreenMessage::mouseWheelRolledUp, mainPage, &MainPage::increaseVolume);
    connect(screenMessage, &ScreenMessage::mouseWheelRolledDown, mainPage, &MainPage::decreaseVolume);

    picInPicWin = new PictureInPictureWindow();
    connect(picInPicWin, &PictureInPictureWindow::exitPicInPic, this, &MainWindow::setPicInPicWindow);
    connect(mainPage, &MainPage::mouseMove, picInPicWin, &PictureInPictureWindow::showMouse);

    playlistDockWidget = new QDockWidget("Playlist",this);
    playlistDockWidget->setFeatures(QDockWidget::NoDockWidgetFeatures);
    playlistDockWidget->setWidget(mainPage->playlistPage());
    playlistDockWidget->setMinimumWidth(160);
    playlistDockWidget->hide();

    chapterDockWidget = new QDockWidget(tr("Chapter List"),this);
    chapterDockWidget->setFeatures(QDockWidget::NoDockWidgetFeatures);
    chapterDockWidget->setWidget(mainPage->chpaterPage());
    playlistDockWidget->setMinimumWidth(160);
    chapterDockWidget->hide();

    addDockWidget(Qt::LeftDockWidgetArea, chapterDockWidget);
    addDockWidget(Qt::RightDockWidgetArea, playlistDockWidget);

    // case exists any
    openFilesFromExplorer();
}

void MainWindow::settingStyleSheet()
{
    qApp->setStyleSheet("QMenu::separator{ background-color: rgba(255,255,255,0.3); height: .5px; }"
                        "QMenu::item{ color: white;}"
                        "QMenu::item:selected {color: white;}"
                        "QToolTip { border: .5px solid rgb(24, 24, 24); color: #3498DB; "
                        "background-color: rgb(10, 10, 10); text-align: center; }" );
}

void MainWindow::openFiles(QString caption, bool play)
{
    QFileDialog dialog(this, caption);
    dialog.setFileMode(QFileDialog::ExistingFiles);
    dialog.setNameFilter(tr("Media Files(*.3g2 *.3gp *.3gp2 *.3gpp *.amv *.asf *.avi *.bik *.bin *.divx *.drc *.dv "
                            "*.f4v *.flv *.gvi *.gxf *.iso *.m1v *.m2v *.m2t *.m2ts *.m4v *.mkv *.mov *.mp2 *.mp2V "
                            "*.mp4 *.mp4v *.mpe *.mpeg *.mpeg1 *.mpeg2 *.mpeg4 *.mpg *.mpv2 *.mts *.mtv *.mxf *.mxg "
                            "*.nsv *.nuv *.ogg *.ogm *.ogv *.ogx *.ps *.rec *.rm *.rmvb *.rpl *.thp *.tod *.ts *.tts "
                            "*.txd *.vob *.vro *.webm *.wm *.wmv *.wtv *.xesc *.3ga *.669 *.152 *.aac *.ac3 *.adt "
                            "*.adts *.aif *.aiff *.amr *.aob *.ape *.awb *.caf *.dts *.flac *.it *.kar *.m4a *.m4b "
                            "*.m4p *.m5p *.mid *.mka *.mlp *.mod *.mpa *.mp1 *.mp2 *.mp3 *.mpc *.mpga *.mus *.oga "
                            "*.ogg *oma *.opus *.qcp *.ra *.rmi *.s3m *.sid *.spx *.thd *.tta *.voc *vqf *.w64 *.wav "
                            "*.wma *.wv *.xa *.xm);;Video Files(*.3g2 *.3gp *.3gp2 *.3gpp *.amv *.asf *.avi *.bik "
                            "*.bin *.divx *.drc *.dv *.f4v *.flv *.gvi *.gxf *.iso *.m1v *.m2v *.m2t *.m2ts *.m4v "
                            "*.mkv *.mov *.mp2 *.mp2V *.mp4 *.mp4v *.mpe *.mpeg *.mpeg1 *.mpeg2 *.mpeg4 *.mpg *.mpv2 "
                            "*.mts *.mtv *.mxf *.mxg *.nsv *.nuv *.ogg *.ogm *.ogv *.ogx *.ps *.rec *.rm *.rmvb *.rpl "
                            "*.thp *.tod *.ts *.tts *.txd *.vob *.vro *.webm *.wm *.wmv *.wtv *.xesc);;"
                            "Audio Files(*.3ga *.669 *.152 *.aac *.ac3 *.adt *.adts *.aif *.aiff *.amr *.aob *.ape "
                            "*.awb *.caf *.dts *.flac *.it *.kar *.m4a *.m4b *.m4p *.m5p *.mid *.mka *.mlp *.mod *.mpa "
                            "*.mp1 *.mp2 *.mp3 *.mpc *.mpga *.mus *.oga *.ogg *oma *.opus *.qcp *.ra *.rmi *.s3m *.sid "
                            "*.spx *.thd *.tta *.voc *vqf *.w64 *.wav *.wma *.wv *.xa *.xm);;"
                            "All Files(*)"));
    dialog.setViewMode(QFileDialog::Detail);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setDirectory(Settings.lastOpenFoler());

    if(dialog.exec())
    {
        Settings.setLastOpenFoler(dialog.directoryUrl().toLocalFile());
        mainPage->openFiles(dialog.selectedUrls(), play);
    }
}

void MainWindow::addFilesToPlaylist()
{
    openFiles(tr("Add one or more files to playlist"), false);
}

void MainWindow::addSubtitlesFile()
{
    if(mainPage->isPlayerSeekable())
    {
        QFileDialog dialog(this, tr("Open subtitle file"));
        dialog.setFileMode(QFileDialog::ExistingFiles);
        dialog.setNameFilter(tr("Subtitle Files(*.cdg *.idx *.srt *.sub *.utf *.ass *.ssa *.aqt *.jss *.psb *.rt *.sami "
                                "*.smi *.txt *.smil *.stl *.usf *.dks *.pjs *.mpl2 *.mks *.vtt *.tt *.ttml *.dfxp *.scc);;"
                                "All Files(*)"));
        dialog.setViewMode(QFileDialog::Detail);
        dialog.setAcceptMode(QFileDialog::AcceptOpen);
        dialog.setDirectory(Settings.lastOpenFoler());

        if(dialog.exec())
        {
            Settings.setLastOpenFoler(dialog.directoryUrl().toLocalFile());
            mainPage->addSubtiles(dialog.selectedUrls());
        }
    }
}

void MainWindow::addChapterFile()
{
    if(mainPage->isPlayerSeekable())
    {
        QFileDialog dialog(this, tr("Open chapters file"));
        dialog.setFileMode(QFileDialog::ExistingFile);
        dialog.setNameFilter(tr("Chapters File(*.txt *.ch)"));
        dialog.setViewMode(QFileDialog::Detail);
        dialog.setAcceptMode(QFileDialog::AcceptOpen);
        dialog.setDirectory(Settings.lastOpenFoler());

        if(dialog.exec())
        {
            Settings.setLastOpenFoler(dialog.directoryUrl().toLocalFile());
            mainPage->processChaptersText(dialog.selectedUrls().at(0).toLocalFile());
        }
    }
}

void MainWindow::openFilesFromExplorer()
{
    QList<QUrl> files;
    for(int i = 1; i < qApp->arguments().size(); ++i)
    {
        files << QUrl::fromLocalFile(qApp->arguments().at(i));
    }
    if(! files.isEmpty())
    {
        mainPage->openFiles(files, true/*play*/);
    }
}

void MainWindow::showPlaylist(bool show)
{
    isPlaylistShown = show;
    if(show)
        playlistDockWidget->show();
    else
        playlistDockWidget->hide();
}

void MainWindow::showChapterlist(bool show)
{
    isChapterListShown = show;
    if(show)
        chapterDockWidget->show();
    else
        chapterDockWidget->hide();
}

void MainWindow::restoreWindow()
{
//    if(Settings.isMaximized())
//    {
//        this->showMaximized();
//        return;
//    }
    this->resize(Settings.mainWindowSize());
    this->move(Settings.mainWindowPosition());
}

void MainWindow::setFullScreen(bool fullScreen)
{
    if(fullScreen)
    {
        onFullScreen();
#ifdef Q_OS_LINUX
        this->show();
        this->raise();
        this->setWindowState( windowState() | Qt::WindowFullScreen );
#else
        this->setWindowState( windowState() | Qt::WindowFullScreen );
        this->show();
        this->raise();
        timerMouse->start(1000);
#endif
    }
    else
    {
        if(this->isFullScreen())
        {
            this->setWindowState( windowState() ^ Qt::WindowFullScreen );
            this->show();
            onNormalScreen();
        }
    }
}

void MainWindow::setPicInPicWindow(bool picInPic)
{
    if(this->isFullScreen())
    {
        setFullScreen(false);
    }

    isInPicInPicWindow = picInPic;
    if(picInPic)
    {
        screenMessage->setParentWidget(picInPicWin);
        this->hide();
        mainPage->playerController()->hide();
        mainPage->playerController()->setPicInPicView(true);
        picInPicWin->setCentralWidget(mainPage);
        picInPicWin->takeController(mainPage->playerController());
        picInPicWin->show();
    }
    else
    {
        screenMessage->setParentWidget(this);
        picInPicWin->takeCentralWidget();
        picInPicWin->hide();
        mainPage->playerController()->setPicInPicView(false);
        this->setCentralWidget(mainPage);
        onNormalScreen();
        restoreWindow();
        this->show();
    }
}

void MainWindow::toggleFullScreen()
{
    setFullScreen(! isFullScreen() );
}

void MainWindow::onFullScreen()
{
    if(isPlaylistShown)
    {
        showPlaylist(false);
        isPlaylistShown = true;
    }
    if(isChapterListShown)
    {
        showChapterlist(false);
        isChapterListShown = true;
    }

    mainPage->videoWidget()->onFullScreen(true);
    mainPage->playerController()->setParent(this);
    mainPage->playerController()->setFullScreenButtonIcon(true);

    QRect screenrect = QApplication::primaryScreen()->geometry();
    mainPage->playerController()->move(screenrect.center().x() - (mainPage->playerController()->width() / 2),
                                       screenrect.bottom() - mainPage->playerController()->height());
    menuBar()->hide();
}

void MainWindow::onNormalScreen()
{
    showMouse();

    if(isPlaylistShown)
        showPlaylist(true);
    if(isChapterListShown)
        showChapterlist(true);

    mainPage->videoWidget()->onFullScreen(false);
    mainPage->layout()->addWidget(mainPage->playerController());
    mainPage->playerController()->setFullScreenButtonIcon(false);
    mainPage->playerController()->show();

    menuBar()->show();
}

void MainWindow::showMouse()
{
    this->setCursor(Qt::ArrowCursor);
    if(this->isFullScreen())
    {
        mainPage->playerController()->setWindowFlags(Qt::WindowStaysOnTopHint);
        mainPage->playerController()->show();
        timerMouse->start(1000);
    }
}

void MainWindow::hideMouse()
{
    timerMouse->stop();
    if(this->isFullScreen())
    {
        if( ! mainPage->playerController()->underMouse())
        {
            mainPage->playerController()->hide();
            this->setCursor(Qt::BlankCursor);
        }
    }
}

void MainWindow::createMenuAndActions()
{
    auto mediaMenu = this->menuBar()->addMenu("Media");

    QAction* openFileAction = new QAction(tr("Open File..."), this);
    openFileAction->setIcon(QIcon(":/images/icons/openFile.png"));
    openFileAction->setShortcuts(QKeySequence::Open);
    connect(openFileAction, &QAction::triggered, this, [this]  { openFiles(tr("Select one or more files to open")); });

    QAction* addFilesToPlaylistAction = new QAction(tr("Add files to playlist"), this);
    addFilesToPlaylistAction->setIcon(QIcon(":/images/icons/openFile.png"));
    addFilesToPlaylistAction->setShortcut(QKeySequence(Qt::CTRL|Qt::Key_A));
    connect(addFilesToPlaylistAction, &QAction::triggered, this, &MainWindow::addFilesToPlaylist);

    QAction* quitAtEndOfPlaylistAction = new QAction(tr("Quit at the end of playlist"), this);
    quitAtEndOfPlaylistAction->setCheckable(true);
    quitAtEndOfPlaylistAction->setChecked(Settings.quitAtTheEndOfPlaylist());
    connect(quitAtEndOfPlaylistAction, &QAction::toggled, this, [] (bool checked)
    {
        Settings.setQuitAtTheEndOfPlaylist(checked);
    });

    QAction* quitAction = new QAction(tr("Quit"), this);
    quitAction->setShortcut(QKeySequence::Quit);
    connect(quitAction, &QAction::triggered, this, &QMainWindow::close);

    mediaMenu->addAction(openFileAction);
    mediaMenu->addAction(addFilesToPlaylistAction);
    mediaMenu->addSeparator();
    mediaMenu->addAction(quitAtEndOfPlaylistAction);
    mediaMenu->addAction(quitAction);

    //add actions for video menu

    auto playbackMenu = this->menuBar()->addMenu("Playback");

    //Actions for the playBack menu
    QAction* seekForwardAction = new QAction(tr("Seek Forward"), this);
    seekForwardAction->setIcon(invertedColorIcon(style()->standardIcon(QStyle::SP_MediaSeekForward)));
    connect(seekForwardAction, &QAction::triggered, mainPage->playerController(), &PlayerController::clickSeekForwardButton);

    QAction* seekBackwardAction = new QAction(tr("Seek Backward"), this);
    seekBackwardAction->setIcon(invertedColorIcon(style()->standardIcon(QStyle::SP_MediaSeekBackward)));
    connect(seekBackwardAction, &QAction::triggered, mainPage->playerController(), &PlayerController::clickSeekBackwardButton);

    QAction* seekToSpecificTimeAction = new QAction(tr("Jump to Specific Time"), this);
    seekToSpecificTimeAction->setShortcut(QKeySequence(Qt::CTRL|Qt::Key_T));
    connect(seekToSpecificTimeAction, &QAction::triggered, this, [this]
    {
        gotoTime.exec();
    });

    QAction* playAction = new QAction(tr("Play"), this);
    playAction->setIcon(invertedColorIcon(style()->standardIcon(QStyle::SP_MediaPlay)));
    connect(playAction, &QAction::triggered, mainPage->playerController(), &PlayerController::clickPlayButton);
    connect(mainPage, &MainPage::mediaStateChanged, [this, playAction] (Vlc::State state)
    {
        if(state == Vlc::Opening)
        {
            playAction->setText(tr("Pause"));
            playAction->setIcon(invertedColorIcon(*mainPage->playerController()->pauseIcon()));
        }
        if(state == Vlc::Paused || state == Vlc::Stopped || state == Vlc::Ended)
        {
            playAction->setText(tr("Play"));
            playAction->setIcon(invertedColorIcon(*mainPage->playerController()->playIcon()));
        }
    });

    QAction* stopAction = new QAction(tr("Stop"), this);
    stopAction->setIcon(invertedColorIcon(style()->standardIcon(QStyle::SP_MediaStop)));
    connect(stopAction, &QAction::triggered, mainPage->playerController(), &PlayerController::clickStopButton);

    QAction* previousAction = new QAction(tr("Previous"), this);
    previousAction->setIcon(invertedColorIcon(style()->standardIcon(QStyle::SP_MediaSkipBackward)));
    connect(previousAction, &QAction::triggered, mainPage->playerController(), &PlayerController::clickPreviousButton);

    QAction* nextAction = new QAction(tr("Next"), this);
    nextAction->setIcon(invertedColorIcon(style()->standardIcon(QStyle::SP_MediaSkipForward)));
    connect(nextAction, &QAction::triggered, mainPage->playerController(), &PlayerController::clickNextButton);

//    recordAction = new QAction(tr("Record"), this);
//    recordAction->setIcon(QIcon(":/images/icons/record.png"));
//    connect(recordAction, &QAction::triggered, this, &MainWindow::record);
    //recordAction->setEnabled(false);

    playbackMenu->addAction(seekForwardAction);
    playbackMenu->addAction(seekBackwardAction);
    playbackMenu->addAction(seekToSpecificTimeAction);
    playbackMenu->addSeparator();
    playbackMenu->addAction(playAction);
    playbackMenu->addAction(stopAction);
    playbackMenu->addAction(previousAction);
    playbackMenu->addAction(nextAction);


    //Actions for the audio menu

    auto audioMenu = this->menuBar()->addMenu("Audio");

    //Audio Track submenu

    QAction*  increaseVolumeAction = new QAction(tr("Increase Volume"), this);
    increaseVolumeAction->setIcon(QIcon(":/images/icons/increaseVolume.png"));
    connect(increaseVolumeAction, &QAction::triggered, mainPage, &MainPage::increaseVolume);

    QAction* decreaseVolumeAction = new QAction(tr("Decrease Volume"), this);
    decreaseVolumeAction->setIcon(QIcon(":/images/icons/decreaseVolume.png"));
    connect(decreaseVolumeAction, &QAction::triggered, mainPage, &MainPage::decreaseVolume);

    QAction* muteVolumeAction = new QAction(tr("Mute"), this);
    muteVolumeAction->setIcon(QIcon(":/images/icons/muteVolume.png"));
    muteVolumeAction->setCheckable(true);
    connect(muteVolumeAction, &QAction::triggered, mainPage->playerController(), &PlayerController::clickVolButton);

    audioMenu->addAction(increaseVolumeAction);
    audioMenu->addAction(decreaseVolumeAction);
    audioMenu->addAction(muteVolumeAction);

    //Actions for the video menu

    auto videoMenu = this->menuBar()->addMenu("Video");

    //Crop submenu
//    cropActionGroup = new QActionGroup(this);

//    crop_defaultAction = new QAction(tr("Default"), cropActionGroup);
//    crop_defaultAction->setCheckable(true);

//    crop_16_9_Action = new QAction(tr("16:9"), cropActionGroup);
//    crop_16_9_Action->setCheckable(true);

//    crop_4_3_Action = new QAction(tr("4:3"), cropActionGroup);
//    crop_4_3_Action->setCheckable(true);

//    crop_1_1_Action = new QAction(tr("1:1"), cropActionGroup);
//    crop_1_1_Action->setCheckable(true);

//    crop_16_10_Action = new QAction(tr("16:10"), cropActionGroup);
//    crop_16_10_Action->setCheckable(true);

//    crop_2_21_1_Action = new QAction(tr("2.21:1"), cropActionGroup);
//    crop_2_21_1_Action->setCheckable(true);

//    crop_1_85_1_Action = new QAction(tr("1.85:1"), cropActionGroup);
//    crop_1_85_1_Action->setCheckable(true);

//    crop_2_35_1_Action = new QAction(tr("2.35:1"), cropActionGroup);
//    crop_2_35_1_Action->setCheckable(true);

//    crop_2_39_1_Action = new QAction(tr("2.39:1"), cropActionGroup);
//    crop_2_39_1_Action->setCheckable(true);

//    crop_5_3_Action = new QAction(tr("5:3"), cropActionGroup);
//    crop_5_3_Action->setCheckable(true);

//    crop_5_4_Action = new QAction(tr("5:4"), cropActionGroup);
//    crop_5_4_Action->setCheckable(true);

    QAction* fullScreenAction = new QAction(tr("FullScreen"));
    connect(fullScreenAction, &QAction::triggered, this, [this]
    {
        if(mainPage->isPlayerSeekable())
            setFullScreen(true);
    });

    QAction* takeSnapshotAction = new QAction(tr("Take Snapshot"));
    connect(takeSnapshotAction, &QAction::triggered, mainPage, &MainPage::takeSnapshot);

    videoMenu->addAction(fullScreenAction);
    videoMenu->addSeparator();
    videoMenu->addAction(takeSnapshotAction);

    //Actions for the subtitle menu

    auto subtitleMenu = this->menuBar()->addMenu("Subtitle");

    QAction* addSubtitlesFileAction = new QAction(tr("Add SubTitle File..."));
    connect(addSubtitlesFileAction, &QAction::triggered, this, &MainWindow::addSubtitlesFile);

    subtitleMenu->addAction(addSubtitlesFileAction);

    auto chapterMenu = this->menuBar()->addMenu("Chapters");

    QAction* addChapterFileAction = new QAction(tr("Add Chapters File..."));
    connect(addChapterFileAction, &QAction::triggered, this, &MainWindow::addChapterFile);

    chapterMenu->addAction(addChapterFileAction);

    auto viewMenu = this->menuBar()->addMenu("View");

    //Actions for the view menu
    QAction* togllePlayListAction = new QAction(tr("PlayList"), this);
    togllePlayListAction->setIcon(invertedColorIcon(QIcon(":/images/icons/playlist.png")));
    togllePlayListAction->setShortcut(QKeySequence(Qt::CTRL|Qt::Key_L));
    connect(togllePlayListAction, &QAction::triggered, mainPage->playerController(), &PlayerController::clickPlaylistButton);

    QAction* toggleChapterListAction = new QAction(tr("Chapter List"), this);
    toggleChapterListAction->setIcon(invertedColorIcon(QIcon(":/images/icons/chapterList.png")));
    toggleChapterListAction->setShortcut(QKeySequence(Qt::CTRL|Qt::Key_C));
    connect(toggleChapterListAction, &QAction::triggered, mainPage->playerController(), &PlayerController::clickChapterListButton);

    viewMenu->addAction(togllePlayListAction);
    viewMenu->addAction(toggleChapterListAction);
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    event->ignore();
    showMouse();
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    if(shouldSavesSettings)
    {
        Settings.setMainWindowSize(this->size());
        Settings.setMainWindowPosition(QPoint(this->x(), this->y()));
    }
    event->accept();
}

void MainWindow::moveEvent(QMoveEvent *event)
{
    if(shouldSavesSettings)
    {
        Settings.setMainWindowPosition(QPoint(this->x(), this->y()));
    }
    event->accept();
}

void MainWindow::showEvent(QShowEvent* event)
{
    if(! shouldSavesSettings) // the same as testing if it's on startup
    {
        restoreWindow();
    }
    shouldSavesSettings = true;
    event->accept();
}

//void MainWindow::changeEvent(QEvent *e)
//{
//    if( e->type() == QEvent::WindowStateChange )
//    {
//        QWindowStateChangeEvent* event = static_cast< QWindowStateChangeEvent* >( e );

//        if( event->oldState() == Qt::WindowNoState && this->windowState() == Qt::WindowMaximized )
//        {
//            Settings.SetIsMaximized(this->isMaximized());
//        }
//        else
//        {
//            Settings.SetIsMaximized(this->isMaximized());
//        }
//    }

//    QWidget::changeEvent(e);
//}

void MainWindow::tests()
{
    auto open = new QAction("Open");
    auto testCore = new QAction("Core Test");
    auto testCore1 = new QAction("Playlist");
    auto testCore2 = new QAction("Chapter List");
    connect(testCore1, &QAction::triggered, this, &MainWindow::showPlaylist);
    connect(testCore2, &QAction::triggered, this, &MainWindow::showChapterlist);
    auto open2 = new QAction("Open");
    connect(open, &QAction::triggered, this, [this]
    {
        mainPage->playFile({"D:\\Documents\\School\\IT Development\\Database\\MySQL\\Programming with Mosh\\Video\\MySQL Tutorial for Beginners [Full Course].mp4"});
//        mainPage->playFile("D:\\Documents\\School\\IT Development\\Web Development\\Javascript\\Videos\\Single Videos\\JavaScript Course\\JavaScript Course.mp4");
    });

    connect(open2, &QAction::triggered, this, [this]
    {
//        mainPage->load("D:\\Music\\00Favorite\\Frank Ocean\\Frank Ocean - channel ORANGE (2012)\\11 - Lost.mp3");
        mainPage->playFile({"D:\\Videos\\Videos\\New\\Jay Rock - Vice City feat. Black Hippy_Full-HD.mp4"});
//        mainPage->load("D:\\Videos\\Videos\\New\\Bruno Mars - When I Was Your Man [Official Video]_Full-HD.mp4");
//        mainPage->load("D:\\Documents\\School\\IT Development\\Database\\MySQL\\Programming with Mosh\\Video\\MySQL Tutorial for Beginners [Full Course].mp4");
    });

    connect(testCore, &QAction::triggered, this, [this]
    {
        qDebug() << this->minimumWidth();
        mainPage->testFunction();
    });

    auto fileMenu = this->menuBar()->addMenu("TEST");

    fileMenu->addAction(open);
    fileMenu->addAction(open2);
    fileMenu->addAction(testCore);
    fileMenu->addAction(testCore1);
}
