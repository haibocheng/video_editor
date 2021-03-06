/*
  ==============================================================================

   This file is part of the JUCE library - "Jules' Utility Class Extensions"
   Copyright 2004-10 by Raw Material Software Ltd.

  ------------------------------------------------------------------------------

   JUCE can be redistributed and/or modified under the terms of the GNU General
   Public License (Version 2), as published by the Free Software Foundation.
   A copy of the license is included in the JUCE distribution, or can be found
   online at www.gnu.org/licenses.

   JUCE is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
   A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

  ------------------------------------------------------------------------------

   To release a closed-source product which uses JUCE, commercial licenses are
   available: visit www.rawmaterialsoftware.com/juce for more information.

  ==============================================================================
*/

#include "../../core/juce_StandardHeader.h"

BEGIN_JUCE_NAMESPACE

#include "juce_AudioDataConverters.h"


//==============================================================================
void AudioDataConverters::convertFloatToInt16LE (const float* source, void* dest, int numSamples, const int destBytesPerSample)
{
    const double maxVal = (double) 0x7fff;
    char* intData = static_cast <char*> (dest);

    if (dest != (void*) source || destBytesPerSample <= 4)
    {
        for (int i = 0; i < numSamples; ++i)
        {
            *(uint16*) intData = ByteOrder::swapIfBigEndian ((uint16) (short) roundToInt (jlimit (-maxVal, maxVal, maxVal * source[i])));
            intData += destBytesPerSample;
        }
    }
    else
    {
        intData += destBytesPerSample * numSamples;

        for (int i = numSamples; --i >= 0;)
        {
            intData -= destBytesPerSample;
            *(uint16*) intData = ByteOrder::swapIfBigEndian ((uint16) (short) roundToInt (jlimit (-maxVal, maxVal, maxVal * source[i])));
        }
    }
}

void AudioDataConverters::convertFloatToInt16BE (const float* source, void* dest, int numSamples, const int destBytesPerSample)
{
    const double maxVal = (double) 0x7fff;
    char* intData = static_cast <char*> (dest);

    if (dest != (void*) source || destBytesPerSample <= 4)
    {
        for (int i = 0; i < numSamples; ++i)
        {
            *(uint16*) intData = ByteOrder::swapIfLittleEndian ((uint16) (short) roundToInt (jlimit (-maxVal, maxVal, maxVal * source[i])));
            intData += destBytesPerSample;
        }
    }
    else
    {
        intData += destBytesPerSample * numSamples;

        for (int i = numSamples; --i >= 0;)
        {
            intData -= destBytesPerSample;
            *(uint16*) intData = ByteOrder::swapIfLittleEndian ((uint16) (short) roundToInt (jlimit (-maxVal, maxVal, maxVal * source[i])));
        }
    }
}

void AudioDataConverters::convertFloatToInt24LE (const float* source, void* dest, int numSamples, const int destBytesPerSample)
{
    const double maxVal = (double) 0x7fffff;
    char* intData = static_cast <char*> (dest);

    if (dest != (void*) source || destBytesPerSample <= 4)
    {
        for (int i = 0; i < numSamples; ++i)
        {
            ByteOrder::littleEndian24BitToChars ((uint32) roundToInt (jlimit (-maxVal, maxVal, maxVal * source[i])), intData);
            intData += destBytesPerSample;
        }
    }
    else
    {
        intData += destBytesPerSample * numSamples;

        for (int i = numSamples; --i >= 0;)
        {
            intData -= destBytesPerSample;
            ByteOrder::littleEndian24BitToChars ((uint32) roundToInt (jlimit (-maxVal, maxVal, maxVal * source[i])), intData);
        }
    }
}

void AudioDataConverters::convertFloatToInt24BE (const float* source, void* dest, int numSamples, const int destBytesPerSample)
{
    const double maxVal = (double) 0x7fffff;
    char* intData = static_cast <char*> (dest);

    if (dest != (void*) source || destBytesPerSample <= 4)
    {
        for (int i = 0; i < numSamples; ++i)
        {
            ByteOrder::bigEndian24BitToChars ((uint32) roundToInt (jlimit (-maxVal, maxVal, maxVal * source[i])), intData);
            intData += destBytesPerSample;
        }
    }
    else
    {
        intData += destBytesPerSample * numSamples;

        for (int i = numSamples; --i >= 0;)
        {
            intData -= destBytesPerSample;
            ByteOrder::bigEndian24BitToChars ((uint32) roundToInt (jlimit (-maxVal, maxVal, maxVal * source[i])), intData);
        }
    }
}

void AudioDataConverters::convertFloatToInt32LE (const float* source, void* dest, int numSamples, const int destBytesPerSample)
{
    const double maxVal = (double) 0x7fffffff;
    char* intData = static_cast <char*> (dest);

    if (dest != (void*) source || destBytesPerSample <= 4)
    {
        for (int i = 0; i < numSamples; ++i)
        {
            *(uint32*)intData = ByteOrder::swapIfBigEndian ((uint32) roundToInt (jlimit (-maxVal, maxVal, maxVal * source[i])));
            intData += destBytesPerSample;
        }
    }
    else
    {
        intData += destBytesPerSample * numSamples;

        for (int i = numSamples; --i >= 0;)
        {
            intData -= destBytesPerSample;
            *(uint32*)intData = ByteOrder::swapIfBigEndian ((uint32) roundToInt (jlimit (-maxVal, maxVal, maxVal * source[i])));
        }
    }
}

void AudioDataConverters::convertFloatToInt32BE (const float* source, void* dest, int numSamples, const int destBytesPerSample)
{
    const double maxVal = (double) 0x7fffffff;
    char* intData = static_cast <char*> (dest);

    if (dest != (void*) source || destBytesPerSample <= 4)
    {
        for (int i = 0; i < numSamples; ++i)
        {
            *(uint32*)intData = ByteOrder::swapIfLittleEndian ((uint32) roundToInt (jlimit (-maxVal, maxVal, maxVal * source[i])));
            intData += destBytesPerSample;
        }
    }
    else
    {
        intData += destBytesPerSample * numSamples;

        for (int i = numSamples; --i >= 0;)
        {
            intData -= destBytesPerSample;
            *(uint32*)intData = ByteOrder::swapIfLittleEndian ((uint32) roundToInt (jlimit (-maxVal, maxVal, maxVal * source[i])));
        }
    }
}

void AudioDataConverters::convertFloatToFloat32LE (const float* source, void* dest, int numSamples, const int destBytesPerSample)
{
    jassert (dest != (void*) source || destBytesPerSample <= 4); // This op can't be performed on in-place data!

    char* d = static_cast <char*> (dest);

    for (int i = 0; i < numSamples; ++i)
    {
        *(float*) d = source[i];

#if JUCE_BIG_ENDIAN
        *(uint32*) d = ByteOrder::swap (*(uint32*) d);
#endif

        d += destBytesPerSample;
    }
}

void AudioDataConverters::convertFloatToFloat32BE (const float* source, void* dest, int numSamples, const int destBytesPerSample)
{
    jassert (dest != (void*) source || destBytesPerSample <= 4); // This op can't be performed on in-place data!

    char* d = static_cast <char*> (dest);

    for (int i = 0; i < numSamples; ++i)
    {
        *(float*) d = source[i];

#if JUCE_LITTLE_ENDIAN
        *(uint32*) d = ByteOrder::swap (*(uint32*) d);
#endif

        d += destBytesPerSample;
    }
}

//==============================================================================
void AudioDataConverters::convertInt16LEToFloat (const void* const source, float* const dest, int numSamples, const int srcBytesPerSample)
{
    const float scale = 1.0f / 0x7fff;
    const char* intData = static_cast <const char*> (source);

    if (source != (void*) dest || srcBytesPerSample >= 4)
    {
        for (int i = 0; i < numSamples; ++i)
        {
            dest[i] = scale * (short) ByteOrder::swapIfBigEndian (*(uint16*)intData);
            intData += srcBytesPerSample;
        }
    }
    else
    {
        intData += srcBytesPerSample * numSamples;

        for (int i = numSamples; --i >= 0;)
        {
            intData -= srcBytesPerSample;
            dest[i] = scale * (short) ByteOrder::swapIfBigEndian (*(uint16*)intData);
        }
    }
}

void AudioDataConverters::convertInt16BEToFloat (const void* const source, float* const dest, int numSamples, const int srcBytesPerSample)
{
    const float scale = 1.0f / 0x7fff;
    const char* intData = static_cast <const char*> (source);

    if (source != (void*) dest || srcBytesPerSample >= 4)
    {
        for (int i = 0; i < numSamples; ++i)
        {
            dest[i] = scale * (short) ByteOrder::swapIfLittleEndian (*(uint16*)intData);
            intData += srcBytesPerSample;
        }
    }
    else
    {
        intData += srcBytesPerSample * numSamples;

        for (int i = numSamples; --i >= 0;)
        {
            intData -= srcBytesPerSample;
            dest[i] = scale * (short) ByteOrder::swapIfLittleEndian (*(uint16*)intData);
        }
    }
}

void AudioDataConverters::convertInt24LEToFloat (const void* const source, float* const dest, int numSamples, const int srcBytesPerSample)
{
    const float scale = 1.0f / 0x7fffff;
    const char* intData = static_cast <const char*> (source);

    if (source != (void*) dest || srcBytesPerSample >= 4)
    {
        for (int i = 0; i < numSamples; ++i)
        {
            dest[i] = scale * (short) ByteOrder::littleEndian24Bit (intData);
            intData += srcBytesPerSample;
        }
    }
    else
    {
        intData += srcBytesPerSample * numSamples;

        for (int i = numSamples; --i >= 0;)
        {
            intData -= srcBytesPerSample;
            dest[i] = scale * (short) ByteOrder::littleEndian24Bit (intData);
        }
    }
}

void AudioDataConverters::convertInt24BEToFloat (const void* const source, float* const dest, int numSamples, const int srcBytesPerSample)
{
    const float scale = 1.0f / 0x7fffff;
    const char* intData = static_cast <const char*> (source);

    if (source != (void*) dest || srcBytesPerSample >= 4)
    {
        for (int i = 0; i < numSamples; ++i)
        {
            dest[i] = scale * (short) ByteOrder::bigEndian24Bit (intData);
            intData += srcBytesPerSample;
        }
    }
    else
    {
        intData += srcBytesPerSample * numSamples;

        for (int i = numSamples; --i >= 0;)
        {
            intData -= srcBytesPerSample;
            dest[i] = scale * (short) ByteOrder::bigEndian24Bit (intData);
        }
    }
}

void AudioDataConverters::convertInt32LEToFloat (const void* const source, float* const dest, int numSamples, const int srcBytesPerSample)
{
    const float scale = 1.0f / 0x7fffffff;
    const char* intData = static_cast <const char*> (source);

    if (source != (void*) dest || srcBytesPerSample >= 4)
    {
        for (int i = 0; i < numSamples; ++i)
        {
            dest[i] = scale * (int) ByteOrder::swapIfBigEndian (*(uint32*) intData);
            intData += srcBytesPerSample;
        }
    }
    else
    {
        intData += srcBytesPerSample * numSamples;

        for (int i = numSamples; --i >= 0;)
        {
            intData -= srcBytesPerSample;
            dest[i] = scale * (int) ByteOrder::swapIfBigEndian (*(uint32*) intData);
        }
    }
}

void AudioDataConverters::convertInt32BEToFloat (const void* const source, float* const dest, int numSamples, const int srcBytesPerSample)
{
    const float scale = 1.0f / 0x7fffffff;
    const char* intData = static_cast <const char*> (source);

    if (source != (void*) dest || srcBytesPerSample >= 4)
    {
        for (int i = 0; i < numSamples; ++i)
        {
            dest[i] = scale * (int) ByteOrder::swapIfLittleEndian (*(uint32*) intData);
            intData += srcBytesPerSample;
        }
    }
    else
    {
        intData += srcBytesPerSample * numSamples;

        for (int i = numSamples; --i >= 0;)
        {
            intData -= srcBytesPerSample;
            dest[i] = scale * (int) ByteOrder::swapIfLittleEndian (*(uint32*) intData);
        }
    }
}

void AudioDataConverters::convertFloat32LEToFloat (const void* const source, float* const dest, int numSamples, const int srcBytesPerSample)
{
    const char* s = static_cast <const char*> (source);

    for (int i = 0; i < numSamples; ++i)
    {
        dest[i] = *(float*)s;

#if JUCE_BIG_ENDIAN
        uint32* const d = (uint32*) (dest + i);
        *d = ByteOrder::swap (*d);
#endif

        s += srcBytesPerSample;
    }
}

void AudioDataConverters::convertFloat32BEToFloat (const void* const source, float* const dest, int numSamples, const int srcBytesPerSample)
{
    const char* s = static_cast <const char*> (source);

    for (int i = 0; i < numSamples; ++i)
    {
        dest[i] = *(float*)s;

#if JUCE_LITTLE_ENDIAN
        uint32* const d = (uint32*) (dest + i);
        *d = ByteOrder::swap (*d);
#endif

        s += srcBytesPerSample;
    }
}


//==============================================================================
void AudioDataConverters::convertFloatToFormat (const DataFormat destFormat,
                                                const float* const source,
                                                void* const dest,
                                                const int numSamples)
{
    switch (destFormat)
    {
    case int16LE:
        convertFloatToInt16LE (source, dest, numSamples);
        break;

    case int16BE:
        convertFloatToInt16BE (source, dest, numSamples);
        break;

    case int24LE:
        convertFloatToInt24LE (source, dest, numSamples);
        break;

    case int24BE:
        convertFloatToInt24BE (source, dest, numSamples);
        break;

    case int32LE:
        convertFloatToInt32LE (source, dest, numSamples);
        break;

    case int32BE:
        convertFloatToInt32BE (source, dest, numSamples);
        break;

    case float32LE:
        convertFloatToFloat32LE (source, dest, numSamples);
        break;

    case float32BE:
        convertFloatToFloat32BE (source, dest, numSamples);
        break;

    default:
        jassertfalse;
        break;
    }
}

void AudioDataConverters::convertFormatToFloat (const DataFormat sourceFormat,
                                                const void* const source,
                                                float* const dest,
                                                const int numSamples)
{
    switch (sourceFormat)
    {
    case int16LE:
        convertInt16LEToFloat (source, dest, numSamples);
        break;

    case int16BE:
        convertInt16BEToFloat (source, dest, numSamples);
        break;

    case int24LE:
        convertInt24LEToFloat (source, dest, numSamples);
        break;

    case int24BE:
        convertInt24BEToFloat (source, dest, numSamples);
        break;

    case int32LE:
        convertInt32LEToFloat (source, dest, numSamples);
        break;

    case int32BE:
        convertInt32BEToFloat (source, dest, numSamples);
        break;

    case float32LE:
        convertFloat32LEToFloat (source, dest, numSamples);
        break;

    case float32BE:
        convertFloat32BEToFloat (source, dest, numSamples);
        break;

    default:
        jassertfalse;
        break;
    }
}

//==============================================================================
void AudioDataConverters::interleaveSamples (const float** const source,
                                             float* const dest,
                                             const int numSamples,
                                             const int numChannels)
{
    for (int chan = 0; chan < numChannels; ++chan)
    {
        int i = chan;
        const float* src = source [chan];

        for (int j = 0; j < numSamples; ++j)
        {
            dest [i] = src [j];
            i += numChannels;
        }
    }
}

void AudioDataConverters::deinterleaveSamples (const float* const source,
                                               float** const dest,
                                               const int numSamples,
                                               const int numChannels)
{
    for (int chan = 0; chan < numChannels; ++chan)
    {
        int i = chan;
        float* dst = dest [chan];

        for (int j = 0; j < numSamples; ++j)
        {
            dst [j] = source [i];
            i += numChannels;
        }
    }
}


#if JUCE_UNIT_TESTS

#include "../../utilities/juce_UnitTest.h"
#include "../../maths/juce_Random.h"

class AudioConversionTests  : public UnitTest
{
public:
    AudioConversionTests() : UnitTest ("Audio data conversion") {}

    template <class F1, class E1, class F2, class E2>
    struct Test5
    {
        static void test (UnitTest& unitTest)
        {
            test (unitTest, false);
            test (unitTest, true);
        }

        static void test (UnitTest& unitTest, bool inPlace)
        {
            const int numSamples = 2048;
            int32 original [numSamples], converted [numSamples], reversed [numSamples];

            {
                AudioData::Pointer<F1, E1, AudioData::NonInterleaved, AudioData::NonConst> d (original);
                bool clippingFailed = false;

                for (int i = 0; i < numSamples / 2; ++i)
                {
                    d.setAsFloat (Random::getSystemRandom().nextFloat() * 2.2f - 1.1f);

                    if (! d.isFloatingPoint())
                        clippingFailed = d.getAsFloat() > 1.0f || d.getAsFloat() < -1.0f || clippingFailed;

                    ++d;
                    d.setAsInt32 (Random::getSystemRandom().nextInt());
                    ++d;
                }

                unitTest.expect (! clippingFailed);
            }

            // convert data from the source to dest format..
            ScopedPointer<AudioData::Converter> conv (new AudioData::ConverterInstance <AudioData::Pointer<F1, E1, AudioData::NonInterleaved, AudioData::Const>,
                                                                                        AudioData::Pointer<F2, E2, AudioData::NonInterleaved, AudioData::NonConst> >());
            conv->convertSamples (inPlace ? reversed : converted, original, numSamples);

            // ..and back again..
            conv = new AudioData::ConverterInstance <AudioData::Pointer<F2, E2, AudioData::NonInterleaved, AudioData::Const>,
                                                     AudioData::Pointer<F1, E1, AudioData::NonInterleaved, AudioData::NonConst> >();
            if (! inPlace)
                zerostruct (reversed);

            conv->convertSamples (reversed, inPlace ? reversed : converted, numSamples);

            {
                int biggestDiff = 0;
                AudioData::Pointer<F1, E1, AudioData::NonInterleaved, AudioData::Const> d1 (original);
                AudioData::Pointer<F1, E1, AudioData::NonInterleaved, AudioData::Const> d2 (reversed);

                const int errorMargin = 2 * AudioData::Pointer<F1, E1, AudioData::NonInterleaved, AudioData::Const>::get32BitResolution()
                                            + AudioData::Pointer<F2, E2, AudioData::NonInterleaved, AudioData::Const>::get32BitResolution();

                for (int i = 0; i < numSamples; ++i)
                {
                    biggestDiff = jmax (biggestDiff, std::abs (d1.getAsInt32() - d2.getAsInt32()));
                    ++d1;
                    ++d2;
                }

                unitTest.expect (biggestDiff <= errorMargin);
            }
        }
    };

    template <class F1, class E1, class FormatType>
    struct Test3
    {
        static void test (UnitTest& unitTest)
        {
            Test5 <F1, E1, FormatType, AudioData::BigEndian>::test (unitTest);
            Test5 <F1, E1, FormatType, AudioData::LittleEndian>::test (unitTest);
        }
    };

    template <class FormatType, class Endianness>
    struct Test2
    {
        static void test (UnitTest& unitTest)
        {
            Test3 <FormatType, Endianness, AudioData::Int8>::test (unitTest);
            Test3 <FormatType, Endianness, AudioData::UInt8>::test (unitTest);
            Test3 <FormatType, Endianness, AudioData::Int16>::test (unitTest);
            Test3 <FormatType, Endianness, AudioData::Int24>::test (unitTest);
            Test3 <FormatType, Endianness, AudioData::Int32>::test (unitTest);
            Test3 <FormatType, Endianness, AudioData::Float32>::test (unitTest);
        }
    };

    template <class FormatType>
    struct Test1
    {
        static void test (UnitTest& unitTest)
        {
            Test2 <FormatType, AudioData::BigEndian>::test (unitTest);
            Test2 <FormatType, AudioData::LittleEndian>::test (unitTest);
        }
    };

    void runTest()
    {
        beginTest ("Round-trip conversion");

        Test1 <AudioData::Int8>::test (*this);
        Test1 <AudioData::Int16>::test (*this);
        Test1 <AudioData::Int24>::test (*this);
        Test1 <AudioData::Int32>::test (*this);
        Test1 <AudioData::Float32>::test (*this);
    }
};

static AudioConversionTests audioConversionUnitTests;

#endif


END_JUCE_NAMESPACE
