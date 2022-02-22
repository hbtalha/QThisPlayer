/****************************************************************************
* VLC-Qt - Qt and libvlc connector library
* Copyright (C) 2012 Tadej Novak <tadej@tano.si>
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

#ifndef VLCQT_MEDIAPLAYER_H_
#define VLCQT_MEDIAPLAYER_H_

#include <QObject>
#include <QString>

#include "Enums.h"

#include <vlc/vlc.h>

#include "Instance.h"
#include "Media.h"
#include "VideoDelegate.h"
#include "Equalizer.h"

class TrackDescription
{
public:
    ///
    /// \brief id The track id
    ///
    int id() const
    {
        return m_id;
    }

    ///
    /// \brief name The track name
    ///
    const std::string& name() const
    {
        return m_name;
    }

    explicit TrackDescription( libvlc_track_description_t* c )
        : m_id( c->i_id )
    {
        if ( c->psz_name != nullptr )
            m_name = c->psz_name;
    }

private:
    int m_id;
    std::string m_name;
};

/*!
    \class VlcMediaPlayer MediaPlayer.h VLCQtCore/MediaPlayer.h
    \ingroup VLCQtCore
    \brief Media Player

    A basic MediaPlayer manager for VLC-Qt library.
    It provides main playback controls.
*/
class  VlcMediaPlayer : public QObject
{
    Q_OBJECT

private:
    static void libvlc_callback(const libvlc_event_t *event,
                                void *data)
    {
        VlcMediaPlayer *core = static_cast<VlcMediaPlayer *>(data);

        switch (event->type)
        {
        case libvlc_MediaPlayerMediaChanged:
            emit core->mediaChanged();
            break;
        case libvlc_MediaPlayerNothingSpecial:
            emit core->nothingSpecial();
            break;
        case libvlc_MediaPlayerOpening:
            emit core->opening();
            break;
        case libvlc_MediaPlayerBuffering:
            emit core->buffering(event->u.media_player_buffering.new_cache);
            emit core->buffering(qRound(event->u.media_player_buffering.new_cache));
            break;
        case libvlc_MediaPlayerPlaying:
            emit core->playing();
            break;
        case libvlc_MediaPlayerPaused:
            emit core->paused();
            break;
        case libvlc_MediaPlayerStopped:
            emit core->stopped();
            break;
        case libvlc_MediaPlayerForward:
            emit core->forward();
            break;
        case libvlc_MediaPlayerBackward:
            emit core->backward();
            break;
        case libvlc_MediaPlayerEndReached:
            emit core->end();
            break;
        case libvlc_MediaPlayerEncounteredError:
            emit core->error();
            break;
        case libvlc_MediaPlayerTimeChanged:
            emit core->timeChanged(event->u.media_player_time_changed.new_time);
            break;
        case libvlc_MediaPlayerPositionChanged:
            emit core->positionChanged(event->u.media_player_position_changed.new_position);
            break;
        case libvlc_MediaPlayerSeekableChanged:
            emit core->seekableChanged(event->u.media_player_seekable_changed.new_seekable);
            break;
        case libvlc_MediaPlayerPausableChanged:
            emit core->pausableChanged(event->u.media_player_pausable_changed.new_pausable);
            break;
        case libvlc_MediaPlayerTitleChanged:
            emit core->titleChanged(event->u.media_player_title_changed.new_title);
            break;
        case libvlc_MediaPlayerSnapshotTaken:
            emit core->snapshotTaken(event->u.media_player_snapshot_taken.psz_filename);
            break;
        case libvlc_MediaPlayerLengthChanged:
            emit core->lengthChanged(event->u.media_player_length_changed.new_length);
            break;
        case libvlc_MediaPlayerVout:
            emit core->vout(event->u.media_player_vout.new_count);
            break;

        case libvlc_MediaPlayerAudioVolume:
            emit core->volumeChanged(event->u.media_player_audio_volume.volume);
            break;
        default:
            break;
        }

        if (event->type >= libvlc_MediaPlayerNothingSpecial
                && event->type <= libvlc_MediaPlayerEncounteredError)
        {
            emit core->stateChanged();
        }
    }

    void createCoreConnections()
    {
        QList<libvlc_event_e> list;
        list << libvlc_MediaPlayerMediaChanged
             << libvlc_MediaPlayerNothingSpecial
             << libvlc_MediaPlayerOpening
             << libvlc_MediaPlayerBuffering
             << libvlc_MediaPlayerPlaying
             << libvlc_MediaPlayerPaused
             << libvlc_MediaPlayerStopped
             << libvlc_MediaPlayerForward
             << libvlc_MediaPlayerBackward
             << libvlc_MediaPlayerEndReached
             << libvlc_MediaPlayerEncounteredError
             << libvlc_MediaPlayerTimeChanged
             << libvlc_MediaPlayerPositionChanged
             << libvlc_MediaPlayerSeekableChanged
             << libvlc_MediaPlayerPausableChanged
             << libvlc_MediaPlayerTitleChanged
             << libvlc_MediaPlayerSnapshotTaken
             << libvlc_MediaPlayerLengthChanged
             << libvlc_MediaPlayerVout
             << libvlc_MediaPlayerAudioVolume;

        foreach (const libvlc_event_e &event, list)
        {
            libvlc_event_attach(_vlcEvents, event, libvlc_callback, this);
        }
    }

    void removeCoreConnections()
    {
        QList<libvlc_event_e> list;
        list << libvlc_MediaPlayerMediaChanged
             << libvlc_MediaPlayerNothingSpecial
             << libvlc_MediaPlayerOpening
             << libvlc_MediaPlayerBuffering
             << libvlc_MediaPlayerPlaying
             << libvlc_MediaPlayerPaused
             << libvlc_MediaPlayerStopped
             << libvlc_MediaPlayerForward
             << libvlc_MediaPlayerBackward
             << libvlc_MediaPlayerEndReached
             << libvlc_MediaPlayerEncounteredError
             << libvlc_MediaPlayerTimeChanged
             << libvlc_MediaPlayerPositionChanged
             << libvlc_MediaPlayerSeekableChanged
             << libvlc_MediaPlayerPausableChanged
             << libvlc_MediaPlayerTitleChanged
             << libvlc_MediaPlayerSnapshotTaken
             << libvlc_MediaPlayerLengthChanged
             << libvlc_MediaPlayerVout;

        foreach (const libvlc_event_e &event, list)
        {
            libvlc_event_detach(_vlcEvents, event, libvlc_callback, this);
        }
    }

    libvlc_media_player_t *_vlcMediaPlayer;
    libvlc_event_manager_t *_vlcEvents;

    VlcMedia *_media;

    VlcEqualizer *_vlcEqualizer;

    VlcVideoDelegate *_videoWidget;
    WId _currentWId;

public:
    /*!
        \brief VlcMediaPlayer constructor.

        This is mandatory to use libvlc playback functions.

        \param instance instance object (VlcInstance *)
    */
    explicit VlcMediaPlayer(VlcInstance *instance)
        : QObject(instance)
    {
        _vlcMediaPlayer = libvlc_media_player_new(instance->core());
        _vlcEvents = libvlc_media_player_event_manager(_vlcMediaPlayer);

        /* Disable mouse and keyboard events */
        libvlc_video_set_key_input(_vlcMediaPlayer, false);
        libvlc_video_set_mouse_input(_vlcMediaPlayer, false);

        _vlcEqualizer = new VlcEqualizer(this->core());

        _videoWidget = 0;
        _media = 0;

        createCoreConnections();
    }

    /*!
        VlcMediaPlayer destructor
    */
    ~VlcMediaPlayer()
    {
        removeCoreConnections();

        delete _vlcEqualizer;

        libvlc_media_player_release(_vlcMediaPlayer);
    }

    /*!
        \brief Returns libvlc media player object.
        \return libvlc media player (libvlc_media_player_t *)
    */
    libvlc_media_player_t *core() const
    {
        return _vlcMediaPlayer;
    }


    /*!
        \brief Returns equalizer object.
        \return equalizer (VlcEqualizer *)
        \version LibVLC 2.0.0 or later
    */
    VlcEqualizer *equalizer() const
    {
        return _vlcEqualizer;
    }


    /*!
        \brief Get the current movie length (in ms).
        \return the movie length (in ms), or -1 if there is no media (const int)
    */
    int length() const
    {
        libvlc_time_t length = libvlc_media_player_get_length(_vlcMediaPlayer);

        return length;
    }

    /*!
        \brief Get current media object
        \return media object (VlcMedia *)
    */
    VlcMedia *currentMedia() const
    {
        return _media;
    }

    /*!
        \brief Get current media core object
        \return media core object (libvlc_media_t *)
    */
    libvlc_media_t *currentMediaCore()
    {
        libvlc_media_t *media = libvlc_media_player_get_media(_vlcMediaPlayer);

        return media;
    }

    /*!
        \brief Open media file or stream. Any media should be playable and opened.
        \param media object (VlcMedia *)
    */
    void open(VlcMedia *media)
    {
        _media = media;
        libvlc_media_player_set_media(_vlcMediaPlayer, media->core());
        play();
    }

    /*!
        \brief Just opens the media file or stream, do not starts playing the media.
        \param media object (VlcMedia *)
    */
    void setMedia(VlcMedia *media)
    {
        _media = media;
        libvlc_media_player_set_media(_vlcMediaPlayer, (_media == nullptr) ? nullptr : media->core());
    }

    int volume() const
    {
        int volume = -1;

        if (_vlcMediaPlayer)
        {
            volume = libvlc_audio_get_volume(_vlcMediaPlayer);
        }

        return volume;
    }

    void setVolume(int newVolume)
    {
        if (_vlcMediaPlayer)
        {
            // Don't change if volume is the same
            if (newVolume != volume())
            {
                libvlc_audio_set_volume(_vlcMediaPlayer, newVolume);
                emit volumeChanged(newVolume);
            }
        }
    }

    void setTrack(int track)
    {
        if (_vlcMediaPlayer)
        {
            libvlc_audio_set_track(_vlcMediaPlayer, track);
        }
    }

    bool isMuted() const
    {
        bool mute = false;
        if (_vlcMediaPlayer)
        {
            mute = libvlc_audio_get_mute(_vlcMediaPlayer);
        }

        return mute;
    }

    bool toggleMute() const
    {
        if (_vlcMediaPlayer)
        {
            libvlc_audio_toggle_mute(_vlcMediaPlayer);
        }

        return isMuted();
    }

    void setMute(bool mute) const
    {
        if (_vlcMediaPlayer && mute != isMuted())
        {
            libvlc_audio_set_mute(_vlcMediaPlayer, mute);
        }
    }

    void mute()
    {
        setMute(true);
    }

    void unmute()
    {
        if(isMuted())
            setMute(false);
    }

    Vlc::Ratio aspectRatio() const
    {
        QString ratio = "";
        if (_vlcMediaPlayer && libvlc_media_player_has_vout(_vlcMediaPlayer))
        {
            ratio = libvlc_video_get_aspect_ratio(_vlcMediaPlayer);
        }

        return Vlc::Ratio(Vlc::ratio().indexOf(ratio));
    }

    Vlc::Ratio cropGeometry() const
    {
        QString crop = "";
        if (_vlcMediaPlayer && libvlc_media_player_has_vout(_vlcMediaPlayer))
        {
            crop = libvlc_video_get_crop_geometry(_vlcMediaPlayer);

        }

        return Vlc::Ratio(Vlc::ratio().indexOf(crop));
    }

    void hideLogo()
    {
        if (_vlcMediaPlayer && libvlc_media_player_has_vout(_vlcMediaPlayer))
        {
            libvlc_video_set_logo_int(_vlcMediaPlayer, libvlc_logo_enable, 0);

        }
    }

    void hideMarquee()
    {
        if (_vlcMediaPlayer && libvlc_media_player_has_vout(_vlcMediaPlayer))
        {
            libvlc_video_set_marquee_int(_vlcMediaPlayer, libvlc_marquee_Enable, 0);

        }
    }

    void setAspectRatio(const Vlc::Ratio &ratio)
    {
        if (_vlcMediaPlayer && libvlc_media_player_has_vout(_vlcMediaPlayer))
        {
            QString ratioOut = ratio == Vlc::Ignore ? "" : Vlc::ratio()[ratio];
            libvlc_video_set_aspect_ratio(_vlcMediaPlayer, ratioOut.toUtf8().data());

        }
    }

    void setCropGeometry(const Vlc::Ratio &ratio)
    {
        if (_vlcMediaPlayer && libvlc_media_player_has_vout(_vlcMediaPlayer))
        {
            QString ratioOut = ratio == Vlc::Ignore ? "" : Vlc::ratio()[ratio];
            libvlc_video_set_crop_geometry(_vlcMediaPlayer, ratioOut.toUtf8().data());

        }
    }

    void setDeinterlace(const Vlc::Deinterlacing &filter)
    {
        if (_vlcMediaPlayer)
        {
            libvlc_video_set_deinterlace(_vlcMediaPlayer, Vlc::deinterlacing()[filter].toUtf8().data());

        }
    }

    Vlc::Scale scale() const
    {
        float scale = 0;
        if (_vlcMediaPlayer && libvlc_media_player_has_vout(_vlcMediaPlayer))
        {
            scale = libvlc_video_get_scale(_vlcMediaPlayer);

        }

        return Vlc::Scale(Vlc::scale().indexOf(scale));
    }

    void setScale(const Vlc::Scale &scale)
    {
        if (_vlcMediaPlayer && libvlc_media_player_has_vout(_vlcMediaPlayer))
        {
            libvlc_video_set_scale(_vlcMediaPlayer, Vlc::scale()[scale]);

        }
    }

    void setTeletextPage(int page)
    {
        if (_vlcMediaPlayer && libvlc_media_player_has_vout(_vlcMediaPlayer))
        {
            libvlc_video_set_teletext(_vlcMediaPlayer, page);
        }
    }

    void showLogo(const QString &file,
                  int x,
                  int y,
                  int opacity)
    {
        if (_vlcMediaPlayer && libvlc_media_player_has_vout(_vlcMediaPlayer))
        {
            libvlc_video_set_logo_string(_vlcMediaPlayer, libvlc_logo_file, file.toUtf8().data());
            libvlc_video_set_logo_int(_vlcMediaPlayer, libvlc_logo_x, x);
            libvlc_video_set_logo_int(_vlcMediaPlayer, libvlc_logo_y, y);
            libvlc_video_set_logo_int(_vlcMediaPlayer, libvlc_logo_opacity, opacity);

            libvlc_video_set_logo_int(_vlcMediaPlayer, libvlc_logo_enable, 1);
        }
    }

    void showMarquee(const QString &text,
                     int x,
                     int y,
                     int timeout,
                     int opacity,
                     int size,
                     int color,
                     int refresh)
    {
        if (_vlcMediaPlayer && libvlc_media_player_has_vout(_vlcMediaPlayer))
        {
            libvlc_video_set_marquee_string(_vlcMediaPlayer, libvlc_marquee_Text, text.toUtf8().data());
            libvlc_video_set_marquee_int(_vlcMediaPlayer, libvlc_marquee_X, x);
            libvlc_video_set_marquee_int(_vlcMediaPlayer, libvlc_marquee_Y, y);
            libvlc_video_set_marquee_int(_vlcMediaPlayer, libvlc_marquee_Timeout, timeout);
            libvlc_video_set_marquee_int(_vlcMediaPlayer, libvlc_marquee_Opacity, opacity);
            libvlc_video_set_marquee_int(_vlcMediaPlayer, libvlc_marquee_Size, size);
            libvlc_video_set_marquee_int(_vlcMediaPlayer, libvlc_marquee_Color, color);
            libvlc_video_set_marquee_int(_vlcMediaPlayer, libvlc_marquee_Refresh, refresh);

            libvlc_video_set_marquee_int(_vlcMediaPlayer, libvlc_marquee_Enable, 1);
        }
    }

    QSize size() const
    {
        unsigned x = 640;
        unsigned y = 480;

        if (_vlcMediaPlayer && libvlc_media_player_has_vout(_vlcMediaPlayer))
        {
            libvlc_video_get_size(_vlcMediaPlayer, 0, &x, &y);
        }

        return QSize(x, y);
    }

    void setSubtitle(int subtitle)
    {
        if (_vlcMediaPlayer && libvlc_media_player_has_vout(_vlcMediaPlayer))
        {
            libvlc_video_set_spu(_vlcMediaPlayer, subtitle);
        }
    }

    void setSubtitleFile(const QString &subtitle)
    {
        if (_vlcMediaPlayer && libvlc_media_player_has_vout(_vlcMediaPlayer))
        {
            libvlc_media_player_add_slave(_vlcMediaPlayer, libvlc_media_slave_type_subtitle,
                                          subtitle.toUtf8().data(), true);
        }
    }


    int setSubititleDelay(int64_t i_delay)
    {
        return libvlc_video_set_spu_delay(_vlcMediaPlayer, i_delay);
    }

    int subtitle() const
    {
        int subtitle = -1;
        if (_vlcMediaPlayer && libvlc_media_player_has_vout(_vlcMediaPlayer))
        {
            subtitle = libvlc_video_get_spu(_vlcMediaPlayer);
        }

        return subtitle;
    }

    int subtitleCount() const
    {
        int count = -1;
        if (_vlcMediaPlayer && libvlc_media_player_has_vout(_vlcMediaPlayer))
        {
            count = libvlc_video_get_spu_count(_vlcMediaPlayer);
        }

        return count;
    }

    QStringList subtitleDescription() const
    {
        QStringList descriptions;

        if (_vlcMediaPlayer && libvlc_media_player_has_vout(_vlcMediaPlayer))
        {
            libvlc_track_description_t *desc;
            desc = libvlc_video_get_spu_description(_vlcMediaPlayer);

            descriptions << QString().fromUtf8(desc->psz_name);
            if (subtitleCount() > 1)
            {
                for (int i = 1; i < subtitleCount(); i++)
                {
                    desc = desc->p_next;
                    descriptions << QString().fromUtf8(desc->psz_name);
                }
            }
        }

        return descriptions;
    }

    QList<int> subtitleIds() const
    {
        QList<int> ids;

        if (_vlcMediaPlayer && libvlc_media_player_has_vout(_vlcMediaPlayer))
        {
            libvlc_track_description_t *desc;
            desc = libvlc_video_get_spu_description(_vlcMediaPlayer);

            ids << desc->i_id;
            if (subtitleCount() > 1)
            {
                for (int i = 1; i < subtitleCount(); i++)
                {
                    desc = desc->p_next;
                    ids << desc->i_id;
                }
            }
        }

        return ids;
    }

    QMap<int, QString> subtitles() const
    {
        QMap<int, QString> tracks;

        if (_vlcMediaPlayer)
        {
            libvlc_track_description_t *desc, *first;
            first = desc = libvlc_video_get_spu_description(_vlcMediaPlayer);

            if (desc != NULL)
            {
                tracks.insert(desc->i_id, QString().fromUtf8(desc->psz_name));
                if (subtitleCount() > 1)
                {
                    for (int i = 1; i < subtitleCount(); i++)
                    {
                        desc = desc->p_next;
                        tracks.insert(desc->i_id, QString().fromUtf8(desc->psz_name));
                    }
                }
                libvlc_track_description_list_release(first);
            }
        }

        return tracks;
    }

    bool takeSnapshot(const QString &path) const
    {
        bool success = false;
        if (_vlcMediaPlayer && libvlc_media_player_has_vout(_vlcMediaPlayer))
        {
            success = libvlc_video_take_snapshot(_vlcMediaPlayer, 0, path.toUtf8().data(), 0, 0) + 1;
        }

        return success;
    }

    int teletextPage() const
    {
        int page = -1;
        if (_vlcMediaPlayer && libvlc_media_player_has_vout(_vlcMediaPlayer))
        {
            page = libvlc_video_get_teletext(_vlcMediaPlayer);
        }

        return page;
    }

    void toggleTeletextTransparency()
    {
        if (_vlcMediaPlayer && libvlc_media_player_has_vout(_vlcMediaPlayer))
        {
            libvlc_toggle_teletext(_vlcMediaPlayer);
        }
    }

    void toggleFullscreen()
    {
        libvlc_toggle_fullscreen(_vlcMediaPlayer);
    }

    int track() const
    {
        int track = -1;
        if (_vlcMediaPlayer)
        {
            track = libvlc_video_get_track(_vlcMediaPlayer);

        }

        return track;
    }

    int trackCount() const
    {
        int count = -1;
        if (_vlcMediaPlayer)
        {
            count = libvlc_video_get_track_count(_vlcMediaPlayer);

        }

        return count;
    }

    QStringList trackDescription() const
    {
        QStringList descriptions;

        if (_vlcMediaPlayer)
        {
            libvlc_track_description_t *desc;
            desc = libvlc_video_get_track_description(_vlcMediaPlayer);


            descriptions << QString().fromUtf8(desc->psz_name);
            if (trackCount() > 1)
            {
                for (int i = 1; i < trackCount(); i++)
                {
                    desc = desc->p_next;
                    descriptions << QString().fromUtf8(desc->psz_name);
                }
            }
        }

        return descriptions;
    }

    QList<int> trackIds() const
    {
        QList<int> ids;

        if (_vlcMediaPlayer)
        {
            libvlc_track_description_t *desc;
            desc = libvlc_video_get_track_description(_vlcMediaPlayer);


            ids << desc->i_id;
            if (trackCount() > 1)
            {
                for (int i = 1; i < trackCount(); i++)
                {
                    desc = desc->p_next;
                    ids << desc->i_id;
                }
            }
        }

        return ids;
    }

    QMap<int, QString> tracks() const
    {
        QMap<int, QString> tracks;

        if (_vlcMediaPlayer)
        {
            libvlc_track_description_t *desc, *first;
            first = desc = libvlc_video_get_track_description(_vlcMediaPlayer);


            if (desc != NULL)
            {
                tracks.insert(desc->i_id, QString().fromUtf8(desc->psz_name));
                if (trackCount() > 1)
                {
                    for (int i = 1; i < trackCount(); i++)
                    {
                        desc = desc->p_next;
                        tracks.insert(desc->i_id, QString().fromUtf8(desc->psz_name));
                    }
                }
                libvlc_track_description_list_release(first);
            }
        }

        return tracks;
    }

public slots:

    void setTime(qint64 time)
    {
        if (!(state() == Vlc::Buffering
                || state() == Vlc::Playing
                || state() == Vlc::Paused))
            return;

        libvlc_media_player_set_time(_vlcMediaPlayer, time);

        if (state() == Vlc::Paused)
            emit timeChanged(time);
    }

public:
    /*!
        \brief Get the current movie time (in ms).
        \return the movie time (in ms), or -1 if there is no media (const int)
    */
    int time() const
    {
        libvlc_time_t time = libvlc_media_player_get_time(_vlcMediaPlayer);

        return time;
    }

    /*! \brief Set video widget.

        Set video widget to be able to play video inside Qt interface.

        \param widget video widget (VlcVideoDelegate *)
    */
#ifdef Q_OS_LINUX
    void setVideoWidget(uint32_t drawable)
#elif defined Q_OS_WIN
    void setVideoWidget(WId drawable)
#endif
    {
#ifdef Q_OS_LINUX
        libvlc_media_player_set_xwindow(_vlcMediaPlayer, drawable);
#elif defined Q_OS_WIN
        libvlc_media_player_set_hwnd(_vlcMediaPlayer, reinterpret_cast<HWND *>(drawable));
#endif
    }

    /*!
        \brief Get video output status
        \return video output status (const bool)
    */
    bool hasVout() const
    {
        bool status = false;
        if (_vlcMediaPlayer)
        {
            status = libvlc_media_player_has_vout(_vlcMediaPlayer);
        }

        return status;
    }

    /*!
        \brief Get seekable status
        \return seekable status (const bool)
        \since VLC-Qt 1.1
    */
    bool isSeekable() const
    {
        if (!libvlc_media_player_get_media(_vlcMediaPlayer))
            return false;

        bool seekable = libvlc_media_player_is_seekable(_vlcMediaPlayer);

        return seekable;
    }

    /*!
        \brief Get current player state.
        \return current player state (const Vlc::State)
    */
    Vlc::State state() const
    {
        // It's possible that the vlc doesn't play anything
        // so check before
        if (!libvlc_media_player_get_media(_vlcMediaPlayer))
            return Vlc::Idle;

        libvlc_state_t state;
        state = libvlc_media_player_get_state(_vlcMediaPlayer);

        return Vlc::State(state);
    }

    /*!
        \brief Get current video widget.
        \return current video widget (VlcVideoWidget *)
    */
    VlcVideoDelegate *videoWidget() const
    {
        return _videoWidget;
    }

    /*!
        \brief Get media content position.
        \return current media position (float)
    */
    float position()
    {
        if (!_vlcMediaPlayer)
            return -1;

        return libvlc_media_player_get_position(_vlcMediaPlayer);
    }

    /*!
        \brief Get sample aspect ratio for current video track( vlc >= 2.1.0 ).
        \return sample aspect ratio (float)
    */
    float sampleAspectRatio()
    {
        if (!_vlcMediaPlayer)
            return 0.0;
        float sar = 0.0;

        libvlc_media_track_t **tracks;
        unsigned tracksCount;
        tracksCount = libvlc_media_tracks_get(_media->core(), &tracks);
        if (tracksCount > 0)
        {
            for (unsigned i = 0; i < tracksCount; i++)
            {
                libvlc_media_track_t *track = tracks[i];
                if (track->i_type == libvlc_track_video && track->i_id == 0)
                {
                    libvlc_video_track_t *videoTrack = track->video;
                    if (videoTrack->i_sar_num > 0)
                        sar = (float)videoTrack->i_sar_den / (float)videoTrack->i_sar_num;
                }
            }
            libvlc_media_tracks_release(tracks, tracksCount);
        }

        return sar;
    }

    /*!
        \brief Get current media playback rate( vlc >= 2.1.0 ).
        \return current media playback rate (float)
    */
    float playbackRate()
    {
        if (!_vlcMediaPlayer)
            return -1;

        return libvlc_media_player_get_rate(_vlcMediaPlayer);
    }


    bool isInIdleState()
    {
        return (state() == Vlc::Idle);
    }
    bool isOpening()
    {
        return (state() == Vlc::Opening);
    }
    bool isBuffering()
    {
        return (state() == Vlc::Buffering);
    }
    bool isPlaying()
    {
        return (state() == Vlc::Playing);
    }
    bool isPaused()
    {
        return (state() == Vlc::Paused);
    }
    bool isStopped()
    {
        return (state() == Vlc::Stopped);
    }
    bool isEnded()
    {
        return (state() == Vlc::Ended);
    }
    bool isInErrorState()
    {
        return (state() == Vlc::Error);
    }

public slots:
    /*! \brief Set the media position.

        This has no effect if no media is being played. Not all formats and protocols support this.

        \param pos the media position (float)
    */
    void setPosition(float pos)
    {
        libvlc_media_player_set_position(_vlcMediaPlayer, pos);
    }

    /*! \brief Set the media playback rate.

        \param rate the media playback rate (float)
    */
    void setPlaybackRate(float rate)
    {
        libvlc_media_player_set_rate(_vlcMediaPlayer, rate);
    }

    /**
     * @brief play Start playback
     *
     * If playback was already started, this method has no effect
     */
    bool play()
    {
        return libvlc_media_player_play(_vlcMediaPlayer) == 0;
    }

    /*!
        \brief Pauses the playback of current media if possible
    */
    void pause()
    {
        if (!_vlcMediaPlayer)
            return;

        if (libvlc_media_player_can_pause(_vlcMediaPlayer))
            libvlc_media_player_set_pause(_vlcMediaPlayer, true);
    }

    /*!
        \brief Toggles pause of the playback of current media if possible
    */
    void togglePause()
    {
        if (!_vlcMediaPlayer)
            return;

        if (libvlc_media_player_can_pause(_vlcMediaPlayer))
            libvlc_media_player_pause(_vlcMediaPlayer);
    }

    /*!
        \brief Resumes the playback of current media if possible
    */
    void resume()
    {
        if (!_vlcMediaPlayer)
            return;

        if (libvlc_media_player_can_pause(_vlcMediaPlayer))
            libvlc_media_player_set_pause(_vlcMediaPlayer, false);
    }

    /*!
        \brief Stops playing current media
    */
    void stop()
    {
        if (!_vlcMediaPlayer)
            return;

        if (_videoWidget)
            _videoWidget->release();
        _currentWId = 0;

        libvlc_media_player_stop(_vlcMediaPlayer);
    }

signals:
    /*!
        \brief Signal sent on backward
    */
    void backward();

    /*!
        \brief Signal sent on buffering
        \param buffer buffer status in percent
    */
    void buffering(float buffer);

    /*!
        \brief Signal sent on buffering
        \param buffer buffer status in percent
    */
    void buffering(int buffer);

    /*!
        \brief Signal sent when end reached
    */
    void end();

    /*!
        \brief Signal sent on error
    */
    void error();

    /*!
        \brief Signal sent on forward
    */
    void forward();

    /*!
        \brief Signal sent on length change
        \param length new length
    */
    void lengthChanged(int length);

    /*!
        \brief Signal sent on media change
        \param media new media object
    */
    void mediaChanged();

    /*!
        \brief Signal sent nothing speciall happened
    */
    void nothingSpecial();

    /*!
        \brief Signal sent when opening
    */
    void opening();

    /*!
        \brief Signal sent on pausable change
        \param pausable pausable status
    */
    void pausableChanged(bool pausable);

    /*!
        \brief Signal sent when paused
    */
    void paused();

    /*!
        \brief Signal sent when playing
    */
    void playing();

    /*!
        \brief Signal sent on volume change
        \param volume new volume
    */
    void volumeChanged(int volume);

    /*!
        \brief Signal sent on position change
        \param position new position
    */
    void positionChanged(float position);

    /*!
        \brief Signal sent on seekable change
        \param seekable seekable status
    */
    void seekableChanged(bool seekable);

    /*!
        \brief Signal sent on snapshot taken
        \param filename filename of the snapshot
    */
    void snapshotTaken(const QString &filename);

    /*!
        \brief Signal sent when stopped
    */
    void stopped();

    /*!
        \brief Signal sent on time change
        \param time new time
    */
    void timeChanged(int time);

    /*!
        \brief Signal sent on title change
        \param title new title
    */
    void titleChanged(int title);

    /*!
        \brief Signal sent when video output is available
        \param count number of video outputs available
    */
    void vout(int count);

    /*!
        \brief Signal sent when state of the player changed
    */
    void stateChanged();

};

#endif // VLCQT_MEDIAPLAYER_H_
