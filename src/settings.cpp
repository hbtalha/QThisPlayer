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

#include "settings.h"

#include <QStandardPaths>
#include <QSize>
#include <QPoint>
#include <QScreen>
#include <QApplication>

QScopedPointer<QThisPlayerSettings> instance;
QThisPlayerSettings &QThisPlayerSettings::singleton()
{
    if(!instance)
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

void QThisPlayerSettings::setVolume(int volume)
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




