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

#ifndef plOpus_h
#define plOpus_h

#include <cstdint>

/** A wrapper for libOpus. 
  * This class manages an Opus encoder and decoder. It has easy-to-use
  * functions for encoding, decoding and configuration.
  */
class plOpus
{
public:
    enum Application {
        kAudio,
        kVoip
    };

    enum Mode {
        kNarrowband = 8000,
        kWideband = 16000,
        kSuperWideband = 24000,
        kFullband = 48000
    };

    virtual ~plOpus();

    /** Use this function to create a new plOpus instance.
      * @param mode Determines bandwith as well as sampling rate
      * @param channels 1 - Mono, 2 - Stereo
      * @param application Choose between Opus Voice and general Audio modes.
      * @returns A new plOpus instance. It's your responsibility to delete it.
      */
    static plOpus* Create(Mode mode, int channels, Application application);

    /** Encodes an uncompressed audio frame into the given out vector.
      * @param [in] data The audio frame buffer. Length must be
      *  EncoderFrameSize() 16-bit integers.
      * @param [in] outSize Size of the allocated output buffer space.
      * @param [out] out Encoded frame bytes will be written to this buffer.
      * @returns The number of bytes that were written. Negative when
      *  something went wrong.
      */
    int32_t EncodeFrame(const int16_t* data, int32_t outSize, uint8_t* out) const;

    /** Decodes an Opus frame into the given out vector.
      * @param [in] data The Opus frame buffer
      * @param [in] size The Opus frame size in bytes
      * @param [out] out Decoded samples will be written to this buffer.
      *  Length should be DecoderFrameSize() 16-bit integers.
      * @returns The total number of decoded elements that were written.
      *  Negative when something went wrong.
      */
    int32_t DecodeFrame(const uint8_t* data, int32_t size, int16_t* out) const;

    /** Enable or disable variable bitrate for the Opus encoder. (default: on) */
    void VBR(bool enabled);

    /** Set the bitrate for the Opus encoder.
      * @param rate Number of bits per second
      *
      * For setting a quality level @see SetQuality().
      */
    void SetBitrate(int32_t rate);

    /** Set an audio quality level for the Opus encoder.
      * The quality level scales the bitrate automatically.
      * @param quality Quality level (from 1 to 10)
      *
      * For setting the bitrate @see SetBitrate().
      */
    void SetQuality(uint8_t quality);

    /** Set the computational complexity for the Opus encoder.
      * @param complexity Complexity level (from 0 to 10)
      */
    void SetComplexity(int32_t complexity);

    /** Returns the maximum number of buffer elements per raw
      * audio frame. It is samples per frame * channels.
      */
    size_t EncoderFrameSize() const { return EncoderFrameSamples() * fChannels; }

    /** Returns the maximum number of buffer elements per decoded
      * audio frame. It is samples per frame * channels.
      */
    size_t DecoderFrameSize() const { return kDecoderFrameSamples * fChannels; }

protected:
    plOpus() { };

    // Since the frame duration can be variable, we assume a max of 60 ms.
    size_t EncoderFrameSamples() const { return static_cast<size_t>(fEncoderSampleRate * 0.06f); }

    // Max values: 48 kHz * 60 ms
    static const size_t kDecoderFrameSamples = 2880;

    class OpusEncoder* fEncoder;
    class OpusDecoder* fDecoder;
    int fChannels;
    int fEncoderSampleRate;
};

#endif //plOpus_h
