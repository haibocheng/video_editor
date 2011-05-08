#ifndef SOUNDS_H
#define SOUNDS_H
#include "config.h"
#include "stream.h"
#include "juce/juce.h"



extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavcore/audioconvert.h>
}

class Sound: public Stream
{

    public :
    FileInputStream *fs;
    AVProbeData *probeData;
    static const long lSize = 32768;
    unsigned char* pDataBuffer;
    ByteIOContext* ByteIOCtx;


    bool Load(String &filename);


    AVPacket* ReadFrame();
    bool IsKeyFrame();
    void DecodeFrame();

    bool loaded;
    void Dispose();
    Sound();
    ~Sound();
    short *sound_buff;
    int sound_buff_size;

    int current_buffer_length;
    int current_buffer_position;
    bool ReadNextByte(short *data);
};


#endif
