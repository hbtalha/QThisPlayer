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

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QSettings>

class QThisPlayerSettings : public QObject
{
    Q_OBJECT

    explicit QThisPlayerSettings(QObject *parent = nullptr) : QObject(parent) { }

public:
    static QThisPlayerSettings& singleton();

    bool isRandom();
    void setRandom(bool random);
    int playlistMode();
    void setPlaylistMode(int loop);
    int volume();
    void SetVolume(int volume);
    bool isMuted();
    void setMute(bool mute);
    bool seeRemainingTime();
    void setSeeRemainingTime(bool rt);
    QString lastOpenFoler();
    void setLastOpenFoler(const QString& folderPath);
    bool quitAtTheEndOfPlaylist();
    void setQuitAtTheEndOfPlaylist(bool checked);
    QSize mainWindowSize();
    void setMainWindowSize(QSize size);
    QPoint mainWindowPosition();
    void setMainWindowPosition(QPoint pos);

private:
    QSettings settings;
};

#define Settings QThisPlayerSettings::singleton()

#endif // SETTINGS_H
