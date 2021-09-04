/****************************************************************************
* VLC-Qt - Qt and libvlc connector library
* Copyright (C) 2015 Tadej Novak <tadej@tano.si>
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

#ifndef VLCQT_MODULEDESCRIPTION_H_
#define VLCQT_MODULEDESCRIPTION_H_

#include <QtCore/QString>

/*!
    \class VlcModuleDescription ModuleDescription.h VLCQtCore/ModuleDescription.h
    \ingroup VLCQtCore
    \brief VLC module description
*/
class  VlcModuleDescription
{
private:
    QString _name;
    QString _shortName;
    QString _longName;
    QString _help;

public:
    /*!
        \enum Type
        \brief Module type
    */
    enum Type
    {
        AudioFilter,
        VideoFilter
    };

    /*!
        \brief Basic constructor
        \param type module type
        \param name module name
    */
    explicit VlcModuleDescription(Type type,
                                  const QString &name)
        : _type(type),
          _name(name) {}

    /*!
        \brief VlcModuleDescription destructor
    */
    ~VlcModuleDescription() {}

    /*!
        \brief Get module type
        \return module type
    */
    inline Type type() const
    {
        return _type;
    }

    /*!
        \brief Get module name
        \return module name
    */
    inline QString name() const
    {
        return _name;
    }

    /*!
        \brief Get short module readable name
        \return short module human readable name
    */
    inline QString shortName() const
    {
        return _shortName;
    }

    /*!
        \brief Set short module readable name
        \param name short module human readable name
    */
    void setShortName(const QString &name)
    {
        if (_shortName != name)
        {
            _shortName = name;
        }
    }

    /*!
        \brief Get long module readable name
        \return long module human readable name
    */
    inline QString longName() const
    {
        return _longName;
    }

    /*!
        \brief Set long module readable name
        \param name long module human readable name
    */
    void setLongName(const QString &name)
    {
        if (_longName != name)
        {
            _longName = name;
        }
    }

    /*!
        \brief Get module help
        \return module help
    */
    inline QString help() const
    {
        return _help;
    }

    /*!
        \brief Set module help
        \param help new module help
    */
    void setHelp(const QString &help)
    {
        if (_help != help)
        {
            _help = help;
        }
    }

private:
    Type _type;

};

#endif // VLCQT_MODULEDESCRIPTION_H_
