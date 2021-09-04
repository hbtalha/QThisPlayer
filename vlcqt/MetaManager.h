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

#ifndef VLCQT_METAMANAGER_H_
#define VLCQT_METAMANAGER_H_

#include <QDate>
#include <QString>

#include <vlc/vlc.h>

#include "Media.h"

class VlcMedia;

/*!
    \class VlcMetaManager MetaManager.h VLCQtCore/MetaManager.h
    \ingroup VLCQtCore
    \brief Media meta information manager

    A manager and editor of media's meta information.
*/
class  VlcMetaManager
{
public:
    /*!
        \brief VlcMetaManager constructor.

        This constructor creates a new media instance from a media location.

        \param media selected media for meta information (VlcMedia *)
    */
    explicit VlcMetaManager(VlcMedia *media)
        : _media(media)
    {
        libvlc_media_parse(media->core());
    }

    /*!
        \brief VlcMetaManager destructor
    */
    ~VlcMetaManager() {}

    /*!
        \brief Get current track title.
        \return current track title (const QString)
    */
    QString title() const
    {
        QString meta(libvlc_media_get_meta(_media->core(), libvlc_meta_Title));
        return meta;
    }

    /*!
        \brief Set track title.
        \param title new track title (QString)
    */
    void setTitle(const QString &title)
    {
        libvlc_media_set_meta(_media->core(), libvlc_meta_Title, title.toUtf8().data());
    }

    /*!
        \brief Get current track artist.
        \return current track artist (const QString)
    */
    QString artist() const
    {
        QString meta(libvlc_media_get_meta(_media->core(), libvlc_meta_Artist));
        return meta;
    }


    /*!
        \brief Set track artist.
        \param artist new track artist (QString)
    */
    void setArtist(const QString &artist)
    {
        libvlc_media_set_meta(_media->core(), libvlc_meta_Artist, artist.toUtf8().data());
    }

    /*!
        \brief Get current track genre.
        \return current track genre (const QString)
    */
    QString genre() const
    {
        QString meta(libvlc_media_get_meta(_media->core(), libvlc_meta_Genre));
        return meta;
    }

    /*!
        \brief Set track genre.
        \param genre new track genre (QString)
    */
    void setGenre(const QString &genre)
    {
        libvlc_media_set_meta(_media->core(), libvlc_meta_Genre, genre.toUtf8().data());
    }

    /*!
        \brief Get current track copyright.
        \return current track copyright (const QString)
    */
    QString copyright() const
    {
        QString meta(libvlc_media_get_meta(_media->core(), libvlc_meta_Copyright));
        return meta;
    }

    /*!
        \brief Set track copyright.
        \param copyright new track copyright (QString)
    */
    void setCopyright(const QString &copyright)
    {
        libvlc_media_set_meta(_media->core(), libvlc_meta_Copyright, copyright.toUtf8().data());
    }

    /*!
        \brief Get current track album.
        \return current track album (const QString)
    */
    QString album() const
    {
        QString meta(libvlc_media_get_meta(_media->core(), libvlc_meta_Album));
        return meta;
    }

    /*!
        \brief Set track album.
        \param album new track album (QString)
    */
    void setAlbum(const QString &album)
    {
        libvlc_media_set_meta(_media->core(), libvlc_meta_Album, album.toUtf8().data());
    }

    /*!
        \brief Get current track number.
        \return current track number (const int)
    */
    int number() const
    {
        QString meta(libvlc_media_get_meta(_media->core(), libvlc_meta_TrackNumber));
        return meta.toInt();
    }

    /*!
        \brief Set track number.
        \param number new track number (int)
    */
    void setNumber(int number)
    {
        libvlc_media_set_meta(_media->core(), libvlc_meta_TrackNumber, QString().number(number).toUtf8().data());
    }

    /*!
        \brief Get current track description.
        \return current track description (const QString)
    */
    QString description() const
    {
        QString meta(libvlc_media_get_meta(_media->core(), libvlc_meta_Description));
        return meta;
    }

    /*!
        \brief Set track description.
        \param description new track description (QString)
    */
    void setDescription(const QString &description)
    {
        libvlc_media_set_meta(_media->core(), libvlc_meta_Description, description.toUtf8().data());
    }

    /*!
        \brief Get current track rating.
        \return current track rating (const QString)
    */
    QString rating() const
    {
        QString meta(libvlc_media_get_meta(_media->core(), libvlc_meta_Rating));
        return meta;
    }

    /*!
        \brief Get current track year.
        \return current track year (const int)
    */
    int year() const
    {
        QString meta(libvlc_media_get_meta(_media->core(), libvlc_meta_Date));
        return meta.toInt();
    }

    /*!
        \brief Set track year.
        \param year new track year (int)
    */
    void setYear(int year)
    {
        libvlc_media_set_meta(_media->core(), libvlc_meta_Date, QString().number(year).toUtf8().data());
    }

    /*!
        \brief Get current track setting.
        \return current track setting (const QString)
    */
    QString setting() const
    {
        QString meta(libvlc_media_get_meta(_media->core(), libvlc_meta_Setting));
        return meta;
    }

    /*!
        \brief Get current track URL.
        \return current track URL (const QString)
    */
    QString url() const
    {
        QString meta(libvlc_media_get_meta(_media->core(), libvlc_meta_URL));
        return meta;
    }

    /*!
        \brief Get current track language.
        \return current track language (const QString)
    */
    QString language() const
    {
        QString meta(libvlc_media_get_meta(_media->core(), libvlc_meta_Language));
        return meta;
    }

    /*!
        \brief Set track language.
        \param language new track language (QString)
    */
    void setLanguage(const QString &language)
    {
        libvlc_media_set_meta(_media->core(), libvlc_meta_Language, language.toUtf8().data());
    }

    /*!
        \brief Get current track publisher.
        \return current track publisher (const QString)
    */
    QString publisher() const
    {
        QString meta(libvlc_media_get_meta(_media->core(), libvlc_meta_Publisher));
        return meta;
    }

    /*!
        \brief Set track publisher.
        \param publisher new track publisher (QString)
    */
    void setPublisher(const QString &publisher)
    {
        libvlc_media_set_meta(_media->core(), libvlc_meta_Publisher, publisher.toUtf8().data());
    }

    /*!
        \brief Get current track encoder.
        \return current track encoder (const QString)
    */
    QString encoder() const;

    /*!
        \brief Set track encoder.
        \param encoder new track encoder (QString)
    */
    void setEncoder(const QString &encoder)
    {
        libvlc_media_set_meta(_media->core(), libvlc_meta_EncodedBy, encoder.toUtf8().data());
    }

    /*!
        \brief Get current track artwork URL.
        \return current track artwork URL (const QString)
    */
    QString artwork() const
    {
        QString meta(libvlc_media_get_meta(_media->core(), libvlc_meta_ArtworkURL));
        return meta;
    }

    /*!
        \brief Get current track ID.
        \return current track ID (const QString)
    */
    QString id() const
    {
        QString meta(libvlc_media_get_meta(_media->core(), libvlc_meta_TrackID));
        return meta;
    }

    /*!
        \brief Save the meta previously set.
        \return true if the write operation was successfull (const bool)
    */
    bool saveMeta() const
    {
        return libvlc_media_save_meta(_media->core());
    }

private:
    VlcMedia *_media;
};

#endif // VLCQT_METAMANAGER_H_
