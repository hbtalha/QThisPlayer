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
