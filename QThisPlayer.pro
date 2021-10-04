QT       += core gui
QT       += multimedia
QT       += multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

LIBS += -L C:\msys64\mingw64\lib\vlc
LIBS += -lvlc

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/components/chapterlistpage.cpp \
    src/components/mainpage.cpp \
    src/components/pictureinpicturewindow.cpp \
    src/components/playercontroller.cpp \
    src/components/playlistpage.cpp \
    src/components/screenmessage.cpp \
    src/dialogs/about.cpp \
    src/dialogs/gototime.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/settings.cpp \
    src/shared.cpp

HEADERS += \
    src/components/chapterlistpage.h \
    src/components/mainpage.h \
    src/components/mediaprogressslider.h \
    src/components/mediavolumeslider.h \
    src/components/pictureinpicturewindow.h \
    src/components/playercontroller.h \
    src/components/playlistdock.h \
    src/components/playlistpage.h \
    src/components/screenmessage.h \
    src/components/videoWidget.h \
    src/dialogs/about.h \
    src/dialogs/gototime.h \
    src/mainwindow.h \
    src/settings.h \
    src/shared.h \
    vlcqt/Enums.h \
    vlcqt/Equalizer.h \
    vlcqt/Instance.h \
    vlcqt/Media.h \
    vlcqt/MediaPlayer.h \
    vlcqt/MetaManager.h \
    vlcqt/ModuleDescription.h \
    vlcqt/Stats.h \
    vlcqt/vlcqt.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    src/resources.qrc

FORMS += \
    src/dialogs/about.ui \
    src/dialogs/gototime.ui

DISTFILES += \
    src/images/icons/chapterList.png \
    src/images/icons/decreaseVolume.png \
    src/images/icons/exit-fullscreen.png \
    src/images/icons/exit-picture-in-picture.png \
    src/images/icons/fullscreen.png \
    src/images/icons/increaseVolume.png \
    src/images/icons/loop_current.png \
    src/images/icons/muteVolume.png \
    src/images/icons/nextChapter.png \
    src/images/icons/openFile.png \
    src/images/icons/picture-in-picture.png \
    src/images/icons/playlist.png \
    src/images/icons/previousChapter.png \
    src/images/icons/random.png \
    src/images/icons/record.png \
    src/images/icons/refresh.png \
    src/images/icons/toggleRepeat.png

RC_ICONS = src/images/icons/app_icon.ico

QMAKE_TARGET_DESCRIPTION = "QThisPlayer"
