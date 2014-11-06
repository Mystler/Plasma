/*==LICENSE==*

CyanWorlds.com Engine - MMOG client, server and tools
Copyright (C) 2011  Cyan Worlds, Inc.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

Additional permissions under GNU GPL version 3 section 7

If you modify this Program, or any covered work, by linking or
combining it with any of RAD Game Tools Bink SDK, Autodesk 3ds Max SDK,
NVIDIA PhysX SDK, Microsoft DirectX SDK, OpenSSL library, Independent
JPEG Group JPEG library, Microsoft Windows Media SDK, or Apple QuickTime SDK
(or a modified version of those libraries),
containing parts covered by the terms of the Bink SDK EULA, 3ds Max EULA,
PhysX SDK EULA, DirectX SDK EULA, OpenSSL and SSLeay licenses, IJG
JPEG Library README, Windows Media SDK EULA, or QuickTime SDK EULA, the
licensors of this Program grant you additional
permission to convey the resulting work. Corresponding Source for a
non-source form of such a combination shall include the source code for
the parts of OpenSSL and IJG JPEG Library used as well as that of the covered
work.

You can contact Cyan Worlds, Inc. by email legal@cyan.com
 or by snail mail at:
      Cyan Worlds, Inc.
      14617 N Newport Hwy
      Mead, WA   99021

*==LICENSE==*/

#include "plOpus.h"

#include <opus.h>

plOpus::~plOpus()
{
    if (fEncoder)
        opus_encoder_destroy(fEncoder);
    if (fDecoder)
        opus_decoder_destroy(fDecoder);
}

plOpus* plOpus::Create(Mode mode, int channels, Application application) 
{
    plOpus* opus = new plOpus();
    opus->fChannels = channels;
    opus->fEncoderSampleRate = mode;

    int applCode = application == kVoip ? OPUS_APPLICATION_VOIP : OPUS_APPLICATION_AUDIO;

    int error;
    opus->fEncoder = opus_encoder_create(mode, channels, applCode, &error);
    if (error != OPUS_OK) {
        delete opus;
        return nullptr;
    }
    // Opus RFC advises to always decode at 48 kHz
    opus->fDecoder = opus_decoder_create(kFullband, channels, &error);
    if (error != OPUS_OK) {
        delete opus;
        return nullptr;
    }

    return opus;
}

inline int32_t plOpus::EncodeFrame(const int16_t* data, int32_t outSize, uint8_t* out) const
{
    return opus_encode(fEncoder, data, EncoderFrameSamples(), out, outSize);
}

int32_t plOpus::DecodeFrame(const uint8_t* data, int32_t size, int16_t* out) const
{
    int samples = opus_decode(fDecoder, data, size, out, kDecoderFrameSamples, 0);
    if (samples < 0)
        return samples;
    return samples * fChannels;
}

inline void plOpus::VBR(bool enabled)
{
    opus_encoder_ctl(fEncoder, OPUS_SET_VBR(enabled ? 1 : 0));
}

inline void plOpus::SetBitrate(int32_t rate)
{
    opus_encoder_ctl(fEncoder, OPUS_SET_BITRATE(rate));
}

inline void plOpus::SetQuality(uint8_t quality)
{ 
    // This is some simple formula that I came up with. Feel free to change. ~Mystler
    SetBitrate(quality * fEncoderSampleRate * fChannels / 5);
}

inline void plOpus::SetComplexity(int32_t complexity)
{
    opus_encoder_ctl(fEncoder, OPUS_SET_COMPLEXITY(complexity));
}
