#ifndef MOVIE_H
#define	MOVIE_H
#include "juce/juce.h"
extern CriticalSection avcodec_critical;
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>

}


#include <vector>
using namespace std;
class Movie
{
private:
    AVProbeData *probeData;
    static const long lSize = 32768;
    unsigned char* pDataBuffer;
    ByteIOContext* ByteIOCtx;


    uint8_t         *buffer;
    SwsContext *img_convert_ctx;

    int videoStream;

    int FindKeyFrame(double back, double dest, bool accurate = true);
    double ratio_to_internal;
    double ratio_to_seconds;
    bool SeekToInternal(int frame);

public:
    AVFrame         *pFrame;
    AVFrame         *pFrameRGB;
    AVCodecContext  *pCodecCtx;
    AVCodec         *pCodec;
    AVStream        *pStream;

    AVFormatContext *pFormatCtx;
    bool loaded;
    Image *image;

    Image *image_preview;
    Image::BitmapData *bitmapData;

    double duration;
    double current;
    double fps;

    int width;
    int height;

    String filename;

    FileInputStream *fs;

    double file_size;

    Movie();

    int ToInternalTime(double seconds);
    double ToSeconds(int internals);

    bool Load(String &filename, bool soft);
    void Dispose();
    ~Movie();
    AVPacket* ReadFrame();
    bool SkipFrame();
    void DecodeFrame();
    bool ReadAndDecodeFrame();
    bool GotoRatioAndRead(double ratio,bool decode = true, bool accurate = true);
    bool GotoSecondAndRead(double dest,bool decode = true, bool accurate = true);
    bool GoBack(int frames);
    Image * GeneratePreview();

    class VideoInfo
    {
        public:
        int bit_rate;
        bool is_bitrate_or_crf;
        String codec_short;
        String codec_tag;
        String codec_long;
        int width;
        int height;
        double fps;
        String language;
        String title;
        PixelFormat pix_fmt;
        int gop;
        int compressionPreset;
        int pass;
        VideoInfo(){}
        VideoInfo(const VideoInfo& copy_info)
        {
            this->is_bitrate_or_crf = copy_info.is_bitrate_or_crf;
            this->compressionPreset = copy_info.compressionPreset;
            this->pass = copy_info.pass;
            this->bit_rate = copy_info.bit_rate;
            this->gop = copy_info.gop;
            this->codec_tag = copy_info.codec_tag;
            this->codec_short = copy_info.codec_short;
            this->codec_long = copy_info.codec_long;
            this->width = copy_info.width;
            this->height = copy_info.height;
            this->fps = copy_info.fps;
            this->language = copy_info.language;
            this->title = copy_info.title;
            this->pix_fmt = copy_info.pix_fmt;
        }
    };
    class AudioInfo
    {
        public:
        int bit_rate;
        String codec_short;
        String codec_long;
        int sample_rate;
        String codec_tag;
        int channels;
        String language;
        String title;
        AudioInfo(){}
        AudioInfo(const AudioInfo& copy_info)
        {
            this->bit_rate = copy_info.bit_rate;
            this->codec_tag = copy_info.codec_tag;
            this->codec_short = copy_info.codec_short;
            this->codec_long = copy_info.codec_long;
            this->sample_rate = copy_info.sample_rate;
            this->channels = copy_info.channels;
            this->language = copy_info.language;
            this->title = copy_info.title;
        }
    };
    class SubInfo
    {
        public:
        String language;
        String title;
        SubInfo(){}
        SubInfo(const SubInfo& copy_info)
        {
            this->language = copy_info.language;
            this->title = copy_info.title;
        }
    };
    class Info
    {
        public:
        String filename;
        double duration;
        int64 size;
        int bit_rate;
        String format_short;
        String format_long;
        vector<VideoInfo> videos;
        vector<AudioInfo> audios;
        vector<SubInfo> subs;
        Info(){}
        Info(const Info& copy_info)
        {
            this->filename = copy_info.filename;
            this->duration = copy_info.duration;
            this->size = copy_info.size;
            this->bit_rate = copy_info.bit_rate;
            this->format_short = copy_info.format_short;
            this->format_long = copy_info.format_long;
            copy(copy_info.videos.begin(), copy_info.videos.end(), back_inserter(this->videos));
            copy(copy_info.audios.begin(), copy_info.audios.end(), back_inserter(this->audios));
            copy(copy_info.subs.begin(), copy_info.subs.end(), back_inserter(this->subs));
        }
    } * info;
    Info* GetMovieInfo();
    String PrintMovieInfo();

};


#endif	/* MOVIE_H */

