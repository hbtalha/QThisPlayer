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

#ifndef VLCQT_ENUMS_H_
#define VLCQT_ENUMS_H_

#include <QtCore/QObject>
#include <QtCore/QSizeF>
#include <QtCore/QStringList>
/*!
    \class Vlc Enums.h VLCQtCore/Enums.h
    \ingroup VLCQtCore
    \brief Common enumerations
*/
class  Vlc : public QObject // LCOV_EXCL_LINE
{
    Q_OBJECT

    Q_ENUMS(LogLevel)
    Q_ENUMS(ActionsType)
    Q_ENUMS(AudioChannel)
    Q_ENUMS(AudioCodec)
    Q_ENUMS(AudioOutput)
    Q_ENUMS(Deinterlacing)
    Q_ENUMS(FillMode)
    Q_ENUMS(Meta)
    Q_ENUMS(Mux)
    Q_ENUMS(PlaybackMode)
    Q_ENUMS(Ratio)
    Q_ENUMS(Scale)
    Q_ENUMS(State)
    Q_ENUMS(VideoCodec)
    Q_ENUMS(VideoOutput)

public:
    /*!
        \brief Dummy enums constructor.

        This constructor creates a new dummy enums object.

        \param parent object parent (QObject *)
    */
    explicit Vlc(QObject *parent = 0)
        : QObject(parent) {}

    /*!
        \enum RenderFormat
        \brief Frame format used for custom rendering
        \since VLC-Qt 1.1
    */
    enum RenderFormat
    {
        YUVFormat
    };

    /*!
        \enum ActionsType
        \brief Actions types identifiers
    */
    enum ActionsType
    {
        AudioTrack,
        Subtitles,
        VideoTrack,
        Other
    };

    /*!
        \enum AudioChannel
        \brief Audio channel options
    */
    enum AudioChannel
    {
        AudioChannelError = -1,
        Stereo = 1,
        RStereo = 2,
        Left = 3,
        Right = 4,
        Dolbys = 5
    };

    /*!
        \enum AudioCodec
        \brief Audio codecs list
    */
    enum AudioCodec
    {
        NoAudio,
        MPEG2Audio,
        MP3,
        MPEG4Audio,
        Vorbis,
        Flac
    };

    /*!
        \enum AudioOutput
        \brief Audio outputs list
    */
    enum AudioOutput
    {
        DefaultAout
    };

    /*!
        \enum Deinterlacing
        \brief Supported deinterlacing modes
    */
    enum Deinterlacing
    {
        Disabled,
        Discard,
        Blend,
        Mean,
        Bob,
        Linear,
        X,
        Yadif,
        Yadif2x,
        Phospor,
        IVTC
    };

    /*!
        \enum FillMode
        \brief Supported fill modes (QML only)
    */
    enum FillMode
    {
        PreserveAspectFit = Qt::KeepAspectRatio,
        PreserveAspectCrop = Qt::KeepAspectRatioByExpanding,
        Stretch = Qt::IgnoreAspectRatio
    };

    /*!
        \enum Meta
        \brief Supported meta types
    */
    enum Meta
    {
        Title,
        Artist,
        Genre,
        Copyright,
        Album,
        TrackNumber,
        Description,
        Rating,
        Date,
        Setting,
        URL,
        Language,
        NowPlaying,
        Publisher,
        EncodedBy,
        ArtworkURL,
        TrackID
    };

    /*!
        \enum Mux
        \brief Recording output file mux
    */
    enum Mux
    {
        TS,
        PS,
        MP4,
        OGG,
        AVI
    };

    /*!
        \enum PlaybackMode
        \brief Playlist playback mode
    */
    enum PlaybackMode
    {
        DefaultPlayback,
        Loop,
        Repeat
    };

    /*!
        \enum Ratio
        \brief Supported aspect and crop ratios
    */
    enum Ratio
    {
        Original,
        Ignore, /*!< QML aspect ratio only */
        R_16_9,
        R_16_10,
        R_185_100,
        R_221_100,
        R_235_100,
        R_239_100,
        R_4_3,
        R_5_4,
        R_5_3,
        R_1_1
    };

    /*!
        \enum Scale
        \brief Supported scales
    */
    enum Scale
    {
        NoScale,
        S_1_05,
        S_1_1,
        S_1_2,
        S_1_3,
        S_1_4,
        S_1_5,
        S_1_6,
        S_1_7,
        S_1_8,
        S_1_9,
        S_2_0
    };

    /*!
        \enum State
        \brief VLC-Qt playback states
    */
    enum State
    {
        Idle,
        Opening,
        Buffering,
        Playing,
        Paused,
        Stopped,
        Ended,
        Error
    };

    /*!
        \enum VideoCodec
        \brief Video codecs list
    */
    enum VideoCodec
    {
        NoVideo,
        MPEG2Video,
        MPEG4Video,
        H264,
        Theora
    };

    /*!
        \enum VideoOutput
        \brief Video outputs list
    */
    enum VideoOutput
    {
#if defined(Q_OS_LINUX)
        X11,
        XVideo,
        GLX,
#elif defined(Q_OS_WIN32)
        DirectX,
        Direct3D,
        OpenGL,
#endif
        DefaultVout
    };

    // Functions
    /*!
        \brief Log level strings
        \return log level strings (QStringList)
    */

    /*!
        \brief Audio codecs strings
        \return audio codecs strings (QStringList)
    */
    static QStringList audioCodec()
    {
        QStringList list;
        list << "none"
             << "mpga"
             << "mp3"
             << "mp4a"
             << "vorb"
             << "flac";

        return list;
    }

    /*!
        \brief Audio outputs strings
        \return audio outputs strings (QStringList)
    */
    static QStringList audioOutput()
    {
        QStringList list;
        list << "default";

        return list;
    }

    /*!
        \brief Audio outputs strings in readable form
        \return audio outputs strings (QStringList)
    */
    static QStringList audioOutputHuman()
    {
        QStringList list;
        list << "default";

        return list;
    }

    /*!
        \brief Deinterlacing modes strings
        \return deinterlacing strings (QStringList)
    */
    static QStringList deinterlacing()
    {
        QStringList list;
        list << ""
             << "discard"
             << "blend"
             << "man"
             << "bob"
             << "linear"
             << "x"
             << "yadif"
             << "yadif2x"
             << "phosphor"
             << "ivtc";

        return list;
    }

    /*!
        \brief Mux strings
        \return mux strings (QStringList)
    */
    static QStringList mux()
    {
        QStringList list;
        list << "ts"
             << "ps"
             << "mp4"
             << "ogg"
             << "avi";

        return list;
    }

    /*!
        \brief Aspect and crop ratios strings
        \return ratios strings (QStringList)
    */
    static QStringList ratio()
    {
        QStringList list;
        list << ""
             << "ignore"
             << "16:9"
             << "16:10"
             << "185:100"
             << "221:100"
             << "235:100"
             << "239:100"
             << "4:3"
             << "5:4"
             << "5:3"
             << "1:1";

        return list;
    }

    /*!
        \brief Aspect and crop ratios strings in readable form
        \return ratios strings (QStringList)
    */
    static QStringList ratioHuman()
    {
        QStringList list;
        list << ""
             << ""
             << "16:9"
             << "16:10"
             << "1.85:1"
             << "2.21:1"
             << "2.35:1"
             << "2.39:1"
             << "4:3"
             << "5:4"
             << "5:3"
             << "1:1";

        return list;
    }

    /*!
        \brief Aspect and crop ratios converter to QSizeF
        \param ratio aspect or crop ratio (Vlc::Ratio)
        \return two-dimensional object using floating point precision (QSizeF)
    */
    static QSizeF ratioSize(const Vlc::Ratio &ratio)
    {
        switch (ratio)
        {
        case Vlc::R_16_9:
            return QSizeF(16, 9);
            break;
        case Vlc::R_16_10:
            return QSizeF(16, 10);
            break;
        case Vlc::R_185_100:
            return QSizeF(185, 100);
            break;
        case Vlc::R_221_100:
            return QSizeF(221, 100);
            break;
        case Vlc::R_235_100:
            return QSizeF(235, 100);
            break;
        case Vlc::R_239_100:
            return QSizeF(239, 100);
            break;
        case Vlc::R_4_3:
            return QSizeF(4, 3);
            break;
        case Vlc::R_5_4:
            return QSizeF(5, 4);
            break;
        case Vlc::R_5_3:
            return QSizeF(5, 3);
            break;
        case Vlc::R_1_1:
            return QSizeF(1, 1);
            break;
        case Vlc::Original:
        case Vlc::Ignore:
        default:
            return QSizeF(0, 0);
            break;
        }

        return QSizeF(0, 0);
    }

    /*!
        \brief Scale floats
        \return scale floats (QList<float>)
    */
    static QList<float> scale()
    {
        QList<float> list;
        list << (float)0
             << (float)1.05
             << (float)1.1
             << (float)1.2
             << (float)1.3
             << (float)1.4
             << (float)1.5
             << (float)1.6
             << (float)1.7
             << (float)1.8
             << (float)1.9
             << (float)2.0;

        return list;
    }

    /*!
        \brief Video codecs strings
        \return video codecs strings (QStringList)
    */
    static QStringList videoCodec()
    {
        QStringList list;
        list << "none"
             << "mpgv"
             << "mp4v"
             << "h264"
             << "theora";

        return list;
    }

    /*!
        \brief Video outputs strings
        \return video outputs strings (QStringList)
    */
    static QStringList videoOutput()
    {
        QStringList list;
        list << "default";

#if defined(Q_OS_LINUX)
        list << "x11"
             << "xvideo"
             << "glx";
#elif defined(Q_OS_WIN32)
        list << "directx"
             << "direct3d"
             << "opengl";
#endif

        return list;
    }

    /*!
        \brief Video outputs strings in readable form
        \return video outputs strings (QStringList)
    */
    static QStringList videoOutputHuman()
    {
        QStringList list;
        list << "default";

#if defined(Q_OS_LINUX)
        list << "X11"
             << "XVideo"
             << "OpenGL";
#elif defined(Q_OS_WIN32)
        list << "DirectX"
             << "Direct3D"
             << "OpenGL";
#endif

        return list;
    }
};

#endif // VLCQT_ENUMS_H_
