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

#ifndef VLCQT_EQUALIZER_H_
#define VLCQT_EQUALIZER_H_

#include <QObject>

#include <cmath>
#include <vlc/vlc.h>

/*!
    \class VlcEqualizer Equalizer.h VLCQtCore/Equalizer.h
    \ingroup VLCQtCore
    \brief Audio equalizer

    \since libVLC 2.2
*/
class  VlcEqualizer : public QObject
{
    Q_OBJECT

private:
    libvlc_media_player_t  *_vlcMediaPlayer;
    libvlc_equalizer_t *_vlcEqualizer;

public:
    /*!
        \brief VlcEqualizer constructor
    */
    explicit VlcEqualizer(libvlc_media_player_t *vlcMediaPlayer)
        : _vlcMediaPlayer(vlcMediaPlayer),
          _vlcEqualizer(libvlc_audio_equalizer_new()) {}

    /*!
        \brief VlcEqualizer destructor
    */
    ~VlcEqualizer()
    {
        if (_vlcEqualizer)
        {
            libvlc_audio_equalizer_release(_vlcEqualizer);
        }
    }

    /*!
        \brief Get the amplification value for a particular equalizer frequency band.
        \param bandIndex index, counting from zero, of the frequency band to get
        \return amplification value (Hz); -1.0 if there is no such frequency band
     */
    float amplificationForBandAt(uint bandIndex) const
    {
        if (_vlcEqualizer)
        {
            float ret = libvlc_audio_equalizer_get_amp_at_index(_vlcEqualizer, bandIndex);
            if (!std::isnan(ret))
            {
                return ret;
            }
        }
        return -1.0;
    }

    /*!
        \brief Get the number of distinct frequency bands for an equalizer.
        \return number of frequency bands
     */
    uint bandCount() const
    {
        if (_vlcEqualizer)
        {
            return libvlc_audio_equalizer_get_band_count();
        }
        else
        {
            return 0;
        }
    }

    /*!
        \brief Get a particular equalizer band frequency.

        This value can be used, for example, to create a label for an equalizer band control in a user interface.

        \param bandIndex index of the band, counting from zero
        \return equalizer band frequency (Hz), or -1 if there is no such band
     */
    float bandFrequency(uint bandIndex) const
    {
        if (_vlcEqualizer)
        {
            return libvlc_audio_equalizer_get_band_frequency(bandIndex);
        }
        else
        {
            return -1.0;
        }
    }

    /*!
        \brief Get the current pre-amplification value from an equalizer.
        \return preamp value (Hz)
     */
    float preamplification() const
    {
        if (_vlcEqualizer)
        {
            return libvlc_audio_equalizer_get_preamp(_vlcEqualizer);
        }
        else
        {
            return 0.0;
        }
    }

    /*!
        \brief Get the number of equalizer presets.
        \return number of presets
     */
    uint presetCount() const
    {
        return libvlc_audio_equalizer_get_preset_count();
    }

    /*!
        \brief Get the name of a particular equalizer preset.

        This name can be used, for example, to prepare a preset label or menu in a user interface.

        \param index index of the preset, counting from zero
        \return preset name, or an empty string if there is no such preset
     */
    QString presetNameAt(uint index) const
    {
        const char *name = libvlc_audio_equalizer_get_preset_name(index);
        if (name == NULL)
        {
            return QString();
        }
        else
        {
            return QString(name);
        }
    }

public slots:
    /*!
        \brief Load from preset
        \param index index of the preset, counting from zero
     */
    void loadFromPreset(uint index)
    {
        if (_vlcEqualizer)
        {
            libvlc_audio_equalizer_release(_vlcEqualizer);
        }
        _vlcEqualizer = libvlc_audio_equalizer_new_from_preset(index);

        if (_vlcEqualizer)
        {
            emit presetLoaded();
        }
    }

    /*!
        \brief Set a new amplification value for a particular equalizer frequency band.

        The new equalizer settings are subsequently applied to a media player by invoking libvlc_media_player_set_equalizer().
        The supplied amplification value will be clamped to the -20.0 to +20.0 range.

        \param amp amplification value (-20.0 to 20.0 Hz)
        \param bandIndex counting from zero, of the frequency band to set
     */
    void setAmplificationForBandAt(float amp, uint bandIndex)
    {
        if (!_vlcEqualizer)
        {
            return;
        }
        libvlc_audio_equalizer_set_amp_at_index(_vlcEqualizer, amp, bandIndex);
        libvlc_media_player_set_equalizer(_vlcMediaPlayer, _vlcEqualizer);
    }

    /*!
        \brief Enable or disable equalizer
        \param enabled toggle flag
     */
    void setEnabled(bool enabled)
    {
        if (enabled && _vlcEqualizer != NULL)
        {
            libvlc_media_player_set_equalizer(_vlcMediaPlayer, _vlcEqualizer);
        }
        else
        {
            libvlc_media_player_set_equalizer(_vlcMediaPlayer, NULL);
        }
    }

    /*!
        \brief Set a new pre-amplification value for an equalizer.

        The new equalizer settings are subsequently applied to a media player by invoking libvlc_media_player_set_equalizer().
        The supplied amplification value will be clamped to the -20.0 to +20.0 range.

        \param value preamp value in (-20.0 to 20.0 Hz)
     */
    void setPreamplification(float value)
    {
        if (!_vlcEqualizer)
        {
            return;
        }
        libvlc_audio_equalizer_set_preamp(_vlcEqualizer, value);
        libvlc_media_player_set_equalizer(_vlcMediaPlayer, _vlcEqualizer);

    }

signals:
    /*!
        \brief Emitted when preset is loaded
     */
    void presetLoaded();

};

#endif // VLCQT_EQUALIZER_H_
