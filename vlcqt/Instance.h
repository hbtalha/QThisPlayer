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

#ifndef VLCQT_VLCINSTANCE_H_
#define VLCQT_VLCINSTANCE_H_

#include <QObject>
#include <QDebug>
#include <QStringList>

#include <vlc/vlc.h>

#include "Enums.h"
#include "ModuleDescription.h"

/*!
    \class VlcInstance Instance.h VLCQtCore/Instance.h
    \ingroup VLCQtCore
    \brief Main instance

    A basic Instance manager for VLC-Qt library.
    It provides main instance controls.
*/
class  VlcInstance : public QObject
{
    Q_OBJECT
public:
    /*!
        \brief VlcInstance constructor.

        This is mandatory for using VLC-Qt and all its other classes.

        \param args libvlc arguments (QStringList)
        \param parent Instance's parent object (QObject *)
    */
    explicit VlcInstance(/*const QStringList &args,*/
        QObject *parent = NULL)
        : QObject(parent)
    {
//        // Convert arguments to required format
//#if defined(Q_OS_WIN32) // Will be removed on Windows if confirmed working
//        char **argv = (char **)malloc(sizeof(char **) * args.count());
//        for (int i = 0; i < args.count(); ++i)
//            argv[i] = (char *)qstrdup(args.at(i).toUtf8().data());
//#else
//        char *argv[args.count()];
//        for (int i = 0; i < args.count(); ++i)
//            argv[i] = (char *)qstrdup(args.at(i).toUtf8().data());
//#endif

        // Create new libvlc instance
        _vlcInstance = libvlc_new(0, nullptr);

        qRegisterMetaType<Vlc::Meta>("Vlc::Meta");
        qRegisterMetaType<Vlc::State>("Vlc::State");



        // Check if instance is running
        if (_vlcInstance)
        {
            _status = true;
            qDebug() << "VLC-Qt" << libVersion() << "initialised";
            qDebug() << "Using libvlc version:" << version();
        }
        else
        {
            qCritical() << "VLC-Qt Error: libvlc failed to load!";
        }
    }

    /*!
        VlcInstance destructor
    */
    ~VlcInstance()
    {
        if (_status && _vlcInstance)
        {
            libvlc_release(_vlcInstance);
        }
    }

    /*!
        \brief Returns libvlc instance object.
        \return libvlc instance (libvlc_instance_t *)
    */
    libvlc_instance_t *core()
    {
        return _vlcInstance;
    }

    /*!
        \brief Returns libvlc initialisation status.
        \return libvlc status (bool)
    */
    bool status() const
    {
        return _status;
    }


    /*!
        \brief VLC-Qt version info
        \return a string containing the VLC-Qt version (QString)
    */
    static QString libVersion()
    {
        QString version;
#if defined(LIBVLCQT_VERSION)
        version.append(QString(LIBVLCQT_VERSION));
#else
        version.append(QString("Unknown"));
#endif //LIBVLCQT_VERSION

#if defined(LIBVLCQT_VERSION_VCS)
        if (QString(LIBVLCQT_VERSION_VCS) != "0" && QString(LIBVLCQT_VERSION_VCS) != "")
        {
            version.append("-" + QString(LIBVLCQT_VERSION_VCS));
        }
#endif //LIBVLCQT_VERSION

        return version;
    }

    /*!
        \brief VLC-Qt major version
        \return VLC-Qt major version (int)
    */
    static int libVersionMajor()
    {
        int version = -1;
#if defined(LIBVLCQT_VERSION_MAJOR)
        version = LIBVLCQT_VERSION_MAJOR;
#endif //LIBVLCQT_VERSION

        return version;
    }

    /*!
        \brief VLC-Qt minor version
        \return VLC-Qt minor version (int)
    */
    static int libVersionMinor()
    {
        int version = -1;
#if defined(LIBVLCQT_VERSION_MINOR)
        version = LIBVLCQT_VERSION_MINOR;
#endif //LIBVLCQT_VERSION

        return version;
    }

    /*!
        \brief libvlc version info
        \return a string containing the libvlc version (QString)
    */
    static QString version()
    {
        // Returns libvlc version
        return QString(libvlc_get_version());
    }

    /*!
        \brief libvlc compiler info
        \return a string containing the compiler version (QString)
    */
    static QString compiler()
    {
        // Returns libvlc compiler version
        return QString(libvlc_get_compiler());
    }

    /*!
        \brief libvlc changeset info
        \return a string containing the changeset (QString)
    */
    static QString changeset()
    {
        // Returns libvlc changeset
        return QString(libvlc_get_changeset());
    }

    /*!
        \brief Sets the application name.

        libvlc passes this as the user agent string when a protocol requires it.

        \param application Application name (QString)
        \param version Application version (QString)

        \see setAppId
    */
    void setUserAgent(const QString &application,
                      const QString &version)
    {
        QString applicationOutput = application + " " + version;
        QString httpOutput = application + "/" + version + " " + "VLC-Qt" + "/" + libVersion(); // "AppName/1.2.3 VLC-Qt/1.2.3"
        libvlc_set_user_agent(_vlcInstance, applicationOutput.toUtf8().data(), httpOutput.toUtf8().data());
    }

    /*!
        \brief Sets the application some meta-information.

        \param id Java-style application identifier, e.g. "com.acme.foobar"
        \param version application version numbers, e.g. "1.2.3"
        \param icon application icon name, e.g. "foobar"

        \see setUserAgent
        \since VLC-Qt 1.1
    */
    void setAppId(const QString &id,
                  const QString &version,
                  const QString &icon)
    {
        libvlc_set_app_id(_vlcInstance, id.toUtf8().data(), version.toUtf8().data(), icon.toUtf8().data());
    }


    /*!
        \brief List audio filter modules
        \return audio filter module description list
    */
    QList<VlcModuleDescription *> audioFilterList() const
    {
        libvlc_module_description_t *original = libvlc_audio_filter_list_get(_vlcInstance);
        if (original == NULL)
        {
            return QList<VlcModuleDescription *>(); // LCOV_EXCL_LINE
        }

        libvlc_module_description_t *list = original;
        QList<VlcModuleDescription *> audioFilters;
        do
        {
            VlcModuleDescription *module = new VlcModuleDescription(VlcModuleDescription::AudioFilter, list->psz_name);
            module->setLongName(list->psz_longname);
            module->setShortName(list->psz_shortname);
            module->setHelp(list->psz_help);
            audioFilters << module;

            list = list->p_next;
        }
        while (list->p_next);

        libvlc_module_description_list_release(original);

        return audioFilters;
    }

    /*!
        \brief List video filter modules
        \return video filter module description list
    */
    QList<VlcModuleDescription *> videoFilterList() const
    {
        libvlc_module_description_t *original = libvlc_video_filter_list_get(_vlcInstance);
        if (original == NULL)
        {
            return QList<VlcModuleDescription *>(); // LCOV_EXCL_LINE
        }

        libvlc_module_description_t *list = original;
        QList<VlcModuleDescription *> videoFilters;
        do
        {
            VlcModuleDescription *module = new VlcModuleDescription(VlcModuleDescription::VideoFilter, list->psz_name);
            module->setLongName(list->psz_longname);
            module->setShortName(list->psz_shortname);
            module->setHelp(list->psz_help);
            videoFilters << module;

            list = list->p_next;
        }
        while (list->p_next);

        libvlc_module_description_list_release(original);

        return videoFilters;
    }


private:
    libvlc_instance_t *_vlcInstance;
    bool _status;
};

#endif // VLCQT_VLCINSTANCE_H_

