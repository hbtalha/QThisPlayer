#include "settings.h"

#include <QStandardPaths>
#include <QSize>
#include <QPoint>
#include <QScreen>
#include <QApplication>

QScopedPointer<QThisPlayerSettings> instance;
QThisPlayerSettings &QThisPlayerSettings::singleton()
{
    instance.reset(new QThisPlayerSettings);

    return *instance;
}

bool QThisPlayerSettings::isRandom()
{
    return settings.value("random", false).toBool();
}

void QThisPlayerSettings::setRandom(bool random)
{
    settings.setValue("random", random);
}

int QThisPlayerSettings::playlistMode()
{
    return settings.value("loop", 0).toInt();
}

void QThisPlayerSettings::setPlaylistMode(int loop)
{
    settings.setValue("loop", loop);
}

int QThisPlayerSettings::volume()
{
    return settings.value("volume", 50).toInt();
}

void QThisPlayerSettings::SetVolume(int volume)
{
    settings.setValue("volume", volume);
}

bool QThisPlayerSettings::isMuted()
{
    return settings.value("mute", false).toBool();
}

void QThisPlayerSettings::setMute(bool mute)
{
    settings.setValue("mute", mute);
}

bool QThisPlayerSettings::seeRemainingTime()
{
    return settings.value("see_remaining_time", false).toBool();
}

void QThisPlayerSettings::setSeeRemainingTime(bool rt)
{
    settings.setValue("see_remaining_time", rt);
}

QString QThisPlayerSettings::lastOpenFoler()
{
    return settings.value("last_open_folder", QStandardPaths::writableLocation(QStandardPaths::MoviesLocation)).toString();
}

void QThisPlayerSettings::setLastOpenFoler(const QString &folderPath)
{
    settings.setValue("last_open_folder", folderPath);
}

bool QThisPlayerSettings::quitAtTheEndOfPlaylist()
{
    return settings.value("quit_at_the_end_of_playlist").toBool();
}

void QThisPlayerSettings::setQuitAtTheEndOfPlaylist(bool checked)
{
    settings.setValue("quit_at_the_end_of_playlist", checked);
}

QSize QThisPlayerSettings::mainWindowSize()
{
    return settings.value("mainwindow_size", QSize(600, 500)).toSize();
}

void QThisPlayerSettings::setMainWindowSize(QSize size)
{
    settings.setValue("mainwindow_size", size);
}

QPoint QThisPlayerSettings::mainWindowPosition()
{
    return settings.value("mainwindow_position", QPoint(493, 135)).toPoint();;
}

void QThisPlayerSettings::setMainWindowPosition(QPoint pos)
{
    settings.setValue("mainwindow_position", pos);
}




