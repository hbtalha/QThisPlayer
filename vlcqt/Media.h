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

#ifndef VLCQT_MEDIA_H_
#define VLCQT_MEDIA_H_

#include <vlc/vlc.h>

#include <QObject>
#include <QString>
#include <QUrl>
#include <QDir>

#include "Enums.h"
#include "Instance.h"
#include "Stats.h"

class VlcInstance;
struct VlcStats;

struct libvlc_event_t;
struct libvlc_event_manager_t;
struct libvlc_media_t;

/*!
    \class VlcMedia Media.h VLCQtCore/Media.h
    \ingroup VLCQtCore
    \brief Media item

    An abstract representation of a playable media.
    It consists of a media location and various optional meta data.
*/
class  VlcMedia : public QObject
{
    Q_OBJECT
private:
    void initMedia(const QString &location,
                   bool localFile,
                   VlcInstance *instance)
    {
        _currentLocation = location;
        QString l = location;
        if (localFile)
            l = QDir::toNativeSeparators(l);

        // Create a new libvlc media descriptor from location
        if (localFile)
            _vlcMedia = libvlc_media_new_path(instance->core(), l.toUtf8().data());
        else
            _vlcMedia = libvlc_media_new_location(instance->core(), l.toUtf8().data());

        _vlcEvents = libvlc_media_event_manager(_vlcMedia);

        createCoreConnections();
    }

    static void libvlc_callback(const libvlc_event_t *event,
                                void *data)
    {
        VlcMedia *core = static_cast<VlcMedia *>(data);

        switch (event->type)
        {
        case libvlc_MediaMetaChanged:
            emit core->metaChanged(Vlc::Meta(event->u.media_meta_changed.meta_type));
            break;
        case libvlc_MediaSubItemAdded:
            emit core->subitemAdded(event->u.media_subitem_added.new_child);
            break;
        case libvlc_MediaDurationChanged:
            emit core->durationChanged(event->u.media_duration_changed.new_duration);
            break;
        case libvlc_MediaParsedChanged:
            emit core->parsedChanged(event->u.media_parsed_changed.new_status);
            emit core->parsedChanged((bool)event->u.media_parsed_changed.new_status);
            break;
        case libvlc_MediaFreed:
            emit core->freed(event->u.media_freed.md);
            break;
        case libvlc_MediaStateChanged:
            emit core->stateChanged(Vlc::State(event->u.media_state_changed.new_state));
            break;
        default:
            break;
        }
    }

    void createCoreConnections()
    {
        QList<libvlc_event_e> list;
        list << libvlc_MediaMetaChanged
             << libvlc_MediaSubItemAdded
             << libvlc_MediaDurationChanged
             << libvlc_MediaParsedChanged
             << libvlc_MediaFreed
             << libvlc_MediaStateChanged;

        foreach (const libvlc_event_e &event, list)
        {
            libvlc_event_attach(_vlcEvents, event, libvlc_callback, this);
        }
    }

    void removeCoreConnections()
    {
        QList<libvlc_event_e> list;
        list << libvlc_MediaMetaChanged
             << libvlc_MediaSubItemAdded
             << libvlc_MediaDurationChanged
             << libvlc_MediaParsedChanged
             << libvlc_MediaFreed
             << libvlc_MediaStateChanged;

        foreach (const libvlc_event_e &event, list)
        {
            libvlc_event_detach(_vlcEvents, event, libvlc_callback, this);
        }
    }

    libvlc_media_t *_vlcMedia;
    libvlc_event_manager_t *_vlcEvents;

    QString _currentLocation;
public:
    /*!
        \brief VlcMedia constructor.

        This constructor creates a new media instance from a media location.

        \param location location of the media (QString)
        \param localFile true, if media is local file (bool)
        \param instance main libvlc instance (VlcInstance *)
    */
    explicit VlcMedia(const QString &location,
                      bool localFile,
                      VlcInstance *instance)
        : QObject(instance)
    {
        initMedia(location, localFile, instance);
    }

    /*!
        \brief VlcMedia constructor.

        This constructor creates a new media instance from a remote media location.
        Provided for convenience.

        \param location remote location of the media (QString)
        \param instance main libvlc instance (VlcInstance *)
    */
    explicit VlcMedia(const QString &location,
                      VlcInstance *instance)
        : QObject(instance)
    {
        initMedia(location, false, instance);
    }

    /*!
        \brief VlcMedia constructor.

        This constructor creates a new media instance from an existing one.

        \param media libvlc media item (libvlc_media_t *)
    */
    VlcMedia(libvlc_media_t *media)
    {
        // Create a new libvlc media descriptor from existing one
        _vlcMedia = libvlc_media_duplicate(media);
    }

    /*!
        \brief VlcMedia destructor
    */
    ~VlcMedia()
    {
        removeCoreConnections();
        libvlc_media_release(_vlcMedia);
    }

    /*!
        \brief libvlc media item
        \return libvlc media item (libvlc_media_t *)
    */
    libvlc_media_t *core()
    {
        return _vlcMedia;
    }

    /*!
        \brief Get parsed status

        Know when it is OK to read metadata and track information

        \return parsed status (const bool)
        \since VLC-Qt 1.1
    */
    bool parsed() const
    {
        int parsed = libvlc_media_is_parsed(_vlcMedia);

        return parsed;
    }

    /*!
        \brief Parse media information

        Parse media information: metadata and track information.
        This call is async.

        \see parsedChanged
        \since VLC-Qt 1.1
    */
    void parse()
    {
        libvlc_media_parse_async(_vlcMedia);
    }

    /*!
        \brief Current media location

        \return QString media location
    */
    QString currentLocation() const
    {
        return _currentLocation;
    }

    /*!
        \brief Get media stats

        \return VlcStats media stats object
    */
    VlcStats *getStats()
    {
        libvlc_media_stats_t *coreStats = new libvlc_media_stats_t;

        VlcStats *stats = new VlcStats;
        stats->valid = libvlc_media_get_stats(_vlcMedia, coreStats);

        stats->read_bytes = coreStats->i_read_bytes;
        stats->input_bitrate = coreStats->f_input_bitrate;
        stats->demux_read_bytes = coreStats->i_demux_read_bytes;
        stats->demux_bitrate = coreStats->f_demux_bitrate;
        stats->demux_corrupted = coreStats->i_demux_corrupted;
        stats->demux_discontinuity = coreStats->i_demux_discontinuity;
        stats->decoded_video = coreStats->i_decoded_video;
        stats->decoded_audio = coreStats->i_decoded_audio;
        stats->displayed_pictures = coreStats->i_displayed_pictures;
        stats->lost_pictures = coreStats->i_lost_pictures;
        stats->played_abuffers = coreStats->i_played_abuffers;
        stats->lost_abuffers = coreStats->i_lost_abuffers;
        stats->sent_packets = coreStats->i_sent_packets;
        stats->sent_bytes = coreStats->i_sent_bytes;
        stats->send_bitrate = coreStats->f_send_bitrate;

        return stats;
    }

    /*!
        \brief Get media state
        \return current media state
        \since VLC-Qt 1.1
    */
    Vlc::State state() const
    {
        libvlc_state_t state;
        state = libvlc_media_get_state(_vlcMedia);

        return Vlc::State(state);
    }

    /*!
        \brief Get media duration
        \return duration
        \since VLC-Qt 1.1
    */
    qint64 duration() const
    {
        libvlc_time_t duration = libvlc_media_get_duration(_vlcMedia);

        return duration;
    }

    /*!
        \brief Duplicate (provided for convenience)

        Apply duplicate options with desired mux but without transcoding.

        \param name output file name (QString)
        \param path output path (QString)
        \param mux output mux (Vlc::Mux)
        \return QString output file
        \sa record()
    */
    QString duplicate(const QString &name,
                      const QString &path,
                      const Vlc::Mux &mux)
    {
        return record(name, path, mux, true);
    }

    /*!
        \brief Duplicate (provided for convenience)

        Apply duplicate options with desired mux and transcoding (experimental).

        \param name output file name (QString)
        \param path output path (QString)
        \param mux output mux (Vlc::Mux)
        \param audioCodec audio codec (Vlc::AudioCodec)
        \param videoCodec video codec (Vlc::VideoCodec)
        \return QString output file
        \sa record()
    */
    QString duplicate(const QString &name,
                      const QString &path,
                      const Vlc::Mux &mux,
                      const Vlc::AudioCodec &audioCodec,
                      const Vlc::VideoCodec &videoCodec)
    {
        return record(name, path, mux, audioCodec, videoCodec, true);
    }

    /*!
        \brief Duplicate (provided for convenience)

        Apply duplicate options with desired mux, transcoding
        and some other options (experimental).

        \param name output file name (QString)
        \param path output path (QString)
        \param mux output mux (Vlc::Mux)
        \param audioCodec audio codec (Vlc::AudioCodec)
        \param videoCodec video codec (Vlc::VideoCodec)
        \param bitrate video bitrate (int)
        \param fps frames per second (int)
        \param scale video scale (int)
        \return QString output file
        \sa record()
    */
    QString duplicate(const QString &name,
                      const QString &path,
                      const Vlc::Mux &mux,
                      const Vlc::AudioCodec &audioCodec,
                      const Vlc::VideoCodec &videoCodec,
                      int bitrate,
                      int fps,
                      int scale)
    {
        return record(name, path, mux, audioCodec, videoCodec, bitrate, fps, scale, true);
    }

    /*!
        \brief Merge

        Apply merge options with desired mux but without transcoding.

        \param name output file name (QString)
        \param path output path (QString)
        \param mux output mux (Vlc::Mux)
        \return QString output file
    */
    QString merge(const QString &name,
                  const QString &path,
                  const Vlc::Mux &mux)
    {
        QString option1, option2, parameters;
        QString l = QDir::toNativeSeparators(path + "/" + name);

        parameters = "gather:std{access=file,mux=%1,dst='%2'}";
        parameters = parameters.arg(Vlc::mux()[mux], l + "." + Vlc::mux()[mux]);

        option1 = ":sout-keep";
        option2 = ":sout=#%1";
        option2 = option2.arg(parameters);

        setOption(option1);
        setOption(option2);

        return l + "." + Vlc::mux()[mux];
    }

    /*!
        \brief Record

        Apply recording options with desired mux but without transcoding.

        \param name output file name (QString)
        \param path output path (QString)
        \param mux output mux (Vlc::Mux)
        \param duplicate also duplicate on screen (bool)
        \return QString output file
    */
    QString record(const QString &name,
                   const QString &path,
                   const Vlc::Mux &mux,
                   bool duplicate = false)
    {
        QString option1, option2, parameters;
        QString l = QDir::toNativeSeparators(path + "/" + name);

        parameters = "std{access=file,mux=%1,dst='%2'}";
        parameters = parameters.arg(Vlc::mux()[mux], l + "." + Vlc::mux()[mux]);

        if (duplicate)
        {
            option2 = ":sout=#duplicate{dst=display,dst=\"%1\"}";
        }
        else
        {
            option2 = ":sout=#%1";
        }

        option1 = ":sout-all";
        option2 = option2.arg(parameters);

        setOption(option1);
        setOption(option2);

        return l + "." + Vlc::mux()[mux];
    }

    /*!
        \brief Record

        Apply recording options with desired mux and transcoding (experimental).

        \param name output file name (QString)
        \param path output path (QString)
        \param mux output mux (Vlc::Mux)
        \param audioCodec audio codec (Vlc::AudioCodec)
        \param videoCodec video codec (Vlc::VideoCodec)
        \param duplicate also duplicate on screen (bool)
        \return QString output file
    */
    QString record(const QString &name,
                   const QString &path,
                   const Vlc::Mux &mux,
                   const Vlc::AudioCodec &audioCodec,
                   const Vlc::VideoCodec &videoCodec,
                   bool duplicate = false)
    {
        QString option1, option2, parameters;
        QString l = QDir::toNativeSeparators(path + "/" + name);

        parameters = "transcode{vcodec=%1,acodec=%2}:std{access=file,mux=%3,dst='%4'}";
        parameters = parameters.arg(Vlc::videoCodec()[videoCodec], Vlc::audioCodec()[audioCodec], Vlc::mux()[mux], l + "." + Vlc::mux()[mux]);

        if (duplicate)
        {
            option2 = ":sout=#duplicate{dst=display,dst=\"%1\"}";
        }
        else
        {
            option2 = ":sout=#%1";
        }

        option1 = ":sout-all";
        option2 = option2.arg(parameters);

        setOption(option1);
        setOption(option2);

        return l + "." + Vlc::mux()[mux];
    }

    /*!
        \brief Record

        Apply recording options with desired mux, transcoding
        and some other options (experimental).

        \param name output file name (QString)
        \param path output path (QString)
        \param mux output mux (Vlc::Mux)
        \param audioCodec audio codec (Vlc::AudioCodec)
        \param videoCodec video codec (Vlc::VideoCodec)
        \param bitrate video bitrate (int)
        \param fps frames per second (int)
        \param scale video scale (int)
        \param duplicate also duplicate on screen (bool)
        \return QString output file
    */
    QString record(const QString &name,
                   const QString &path,
                   const Vlc::Mux &mux,
                   const Vlc::AudioCodec &audioCodec,
                   const Vlc::VideoCodec &videoCodec,
                   int bitrate,
                   int fps,
                   int scale,
                   bool duplicate = false)
    {
        QString option1, option2, parameters;
        QString l = QDir::toNativeSeparators(path + "/" + name);

        parameters = "transcode{vcodec=%1,vb=%2,fps=%3,scale=%4,acodec=%5}:std{access=file,mux=%6,dst='%7'}";
        parameters = parameters.arg(Vlc::videoCodec()[videoCodec], QString::number(bitrate), QString::number(fps), QString::number(scale), Vlc::audioCodec()[audioCodec], Vlc::mux()[mux], l + "." + Vlc::mux()[mux]);

        if (duplicate)
        {
            option2 = ":sout=#duplicate{dst=display,dst=\"%1\"}";
        }
        else
        {
            option2 = ":sout=#%1";
        }

        option1 = ":sout-all";
        option2 = option2.arg(parameters);

        setOption(option1);
        setOption(option2);

        return l + "." + Vlc::mux()[mux];
    }

    /*!
        \brief Set program
        \param program program PID (int)
    */
    void setProgram(int program)
    {
        QString option = "program=%1";
        setOption(option.arg(program));
    }

    /*!
        \brief Set media option
        \param option media option (QString)
    */
    void setOption(const QString &option)
    {
        libvlc_media_add_option(_vlcMedia, option.toUtf8().data());
    }

    /*!
        \brief Set media options
        \param options media options (QStringList)
    */
    void setOptions(const QStringList &options)
    {
        foreach (const QString &option, options)
        {
            libvlc_media_add_option(_vlcMedia, option.toUtf8().data());
        }
    }

signals:
    /*!
        \brief Signal sent on meta change
        \param meta new meta
    */
    void metaChanged(const Vlc::Meta &meta);

    /*!
        \brief Signal sent on subitem added
        \param subitem subitem that was added
    */
    void subitemAdded(libvlc_media_t *subitem);

    /*!
        \brief Signal sent on duration change
        \param duration new duration
    */
    void durationChanged(int duration);

    /*!
        \brief Signal sent on parsed change
        \param status new parsed status
        \deprecated Deprecated since VLC-Qt 1.1, will be removed in 2.0
    */
    Q_DECL_DEPRECATED void parsedChanged(int status);

    /*!
        \brief Signal sent on parsed change
        \param status new parsed status
    */
    void parsedChanged(bool status);

    /*!
        \brief Signal sent on freed
        \param media freed libvlc_media_t object
    */
    void freed(libvlc_media_t *media);

    /*!
        \brief Signal sent on state change
        \param state new state
    */
    void stateChanged(const Vlc::State &state);

};

#endif // VLCQT_MEDIA_H_
