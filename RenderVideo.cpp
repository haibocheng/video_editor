#include "timeline.h"
#include "movie.h"
#include "localization.h"
extern "C" {
#include <libavcodec/opt.h>
}
#define INT64_C __INT64_C

AVFrame *picture;
uint8_t *video_outbuf;
int frame_count, video_outbuf_size;
bool end_writing;
float t, tincr, tincr2;
int16_t *samples;
uint8_t *audio_outbuf;
int audio_outbuf_size;
int audio_input_frame_size;
int64_t pts;
int current_pass;
int all_pass;
String pass_info;
bool is_codec_x264;


static int sws_flags = SWS_GAUSS;


extern "C" {
#include <libavutil/intreadwrite.h>
}


SwsContext *img_convert_ctx = NULL;

int srcW = 0;
int srcH = 0;
int dstW = 0;
int dstH = 0;
PixelFormat srcFormat;
PixelFormat dstFormat;


static void setCompressionPreset(AVCodecContext *c, int preset,int pass_number)
{
    if(is_codec_x264)
    {
        if(pass_number == 1)
        {
            switch(preset)
            {
            case 1:
            {
                av_set_string3(c,"coder","1",1,NULL);
                av_set_string3(c,"flags","+loop",1,NULL);
                av_set_string3(c,"cmp","+chroma",1,NULL);
                av_set_string3(c,"partitions","+parti8x8+parti4x4+partp8x8+partp4x4+partb8x8",1,NULL);
                av_set_string3(c,"me_method","tesa",1,NULL);
                av_set_string3(c,"subq","10",1,NULL);
                av_set_string3(c,"me_range","24",1,NULL);
                av_set_string3(c,"g","250",1,NULL);
                av_set_string3(c,"keyint_min","25",1,NULL);
                av_set_string3(c,"sc_threshold","40",1,NULL);
                av_set_string3(c,"i_qfactor","0.71",1,NULL);
                av_set_string3(c,"b_strategy","2",1,NULL);
                av_set_string3(c,"qcomp","0.6",1,NULL);
                av_set_string3(c,"qmin","10",1,NULL);
                av_set_string3(c,"qmax","51",1,NULL);
                av_set_string3(c,"qdiff","4",1,NULL);
                av_set_string3(c,"bf","16",1,NULL);
                av_set_string3(c,"refs","16",1,NULL);
                av_set_string3(c,"directpred","3",1,NULL);
                av_set_string3(c,"trellis","2",1,NULL);
                av_set_string3(c,"flags2","+bpyramid+mixed_refs+wpred+dct8x8-fastpskip",1,NULL);
                av_set_string3(c,"wpredp","2",1,NULL);
                av_set_string3(c,"rc_lookahead","60",1,NULL);
            }
            break;
            case 2:
            {
                av_set_string3(c,"coder","1",1,NULL);
                av_set_string3(c,"flags","+loop",1,NULL);
                av_set_string3(c,"cmp","+chroma",1,NULL);
                av_set_string3(c,"partitions","-parti8x8-parti4x4-partp8x8-partb8x8",1,NULL);
                av_set_string3(c,"me_method","dia",1,NULL);
                av_set_string3(c,"subq","2",1,NULL);
                av_set_string3(c,"me_range","24",1,NULL);
                av_set_string3(c,"g","250",1,NULL);
                av_set_string3(c,"keyint_min","25",1,NULL);
                av_set_string3(c,"sc_threshold","40",1,NULL);
                av_set_string3(c,"i_qfactor","0.71",1,NULL);
                av_set_string3(c,"b_strategy","2",1,NULL);
                av_set_string3(c,"qcomp","0.6",1,NULL);
                av_set_string3(c,"qmin","10",1,NULL);
                av_set_string3(c,"qmax","51",1,NULL);
                av_set_string3(c,"qdiff","4",1,NULL);
                av_set_string3(c,"bf","8",1,NULL);
                av_set_string3(c,"refs","1",1,NULL);
                av_set_string3(c,"directpred","3",1,NULL);
                av_set_string3(c,"trellis","0",1,NULL);
                av_set_string3(c,"flags2","+bpyramid-mixed_refs+wpred-dct8x8+fastpskip",1,NULL);
                av_set_string3(c,"wpredp","2",1,NULL);
                av_set_string3(c,"rc_lookahead","60",1,NULL);
            }
            break;
            case 3:
            {
                av_set_string3(c,"coder","1",1,NULL);
                av_set_string3(c,"flags","+loop",1,NULL);
                av_set_string3(c,"cmp","+chroma",1,NULL);
                av_set_string3(c,"partitions","-parti8x8-parti4x4-partp8x8-partb8x8",1,NULL);
                av_set_string3(c,"me_method","dia",1,NULL);
                av_set_string3(c,"subq","2",1,NULL);
                av_set_string3(c,"me_range","16",1,NULL);
                av_set_string3(c,"g","250",1,NULL);
                av_set_string3(c,"keyint_min","25",1,NULL);
                av_set_string3(c,"sc_threshold","40",1,NULL);
                av_set_string3(c,"i_qfactor","0.71",1,NULL);
                av_set_string3(c,"b_strategy","2",1,NULL);
                av_set_string3(c,"qcomp","0.6",1,NULL);
                av_set_string3(c,"qmin","10",1,NULL);
                av_set_string3(c,"qmax","51",1,NULL);
                av_set_string3(c,"qdiff","4",1,NULL);
                av_set_string3(c,"bf","3",1,NULL);
                av_set_string3(c,"refs","1",1,NULL);
                av_set_string3(c,"directpred","3",1,NULL);
                av_set_string3(c,"trellis","0",1,NULL);
                av_set_string3(c,"flags2","+bpyramid-mixed_refs+wpred-dct8x8+fastpskip",1,NULL);
                av_set_string3(c,"wpredp","2",1,NULL);
                av_set_string3(c,"rc_lookahead","60",1,NULL);
            }
            break;
            case 4:
            {
                av_set_string3(c,"coder","1",1,NULL);
                av_set_string3(c,"flags","+loop",1,NULL);
                av_set_string3(c,"cmp","+chroma",1,NULL);
                av_set_string3(c,"partitions","-parti8x8-parti4x4-partp8x8-partb8x8",1,NULL);
                av_set_string3(c,"me_method","dia",1,NULL);
                av_set_string3(c,"subq","2",1,NULL);
                av_set_string3(c,"me_range","16",1,NULL);
                av_set_string3(c,"g","250",1,NULL);
                av_set_string3(c,"keyint_min","25",1,NULL);
                av_set_string3(c,"sc_threshold","40",1,NULL);
                av_set_string3(c,"i_qfactor","0.71",1,NULL);
                av_set_string3(c,"b_strategy","2",1,NULL);
                av_set_string3(c,"qcomp","0.6",1,NULL);
                av_set_string3(c,"qmin","10",1,NULL);
                av_set_string3(c,"qmax","51",1,NULL);
                av_set_string3(c,"qdiff","4",1,NULL);
                av_set_string3(c,"bf","3",1,NULL);
                av_set_string3(c,"refs","1",1,NULL);
                av_set_string3(c,"directpred","3",1,NULL);
                av_set_string3(c,"trellis","0",1,NULL);
                av_set_string3(c,"flags2","+bpyramid-mixed_refs+wpred-dct8x8+fastpskip",1,NULL);
                av_set_string3(c,"wpredp","2",1,NULL);
                av_set_string3(c,"rc_lookahead","50",1,NULL);
            }
            break;
            case 5:
            {
                av_set_string3(c,"coder","1",1,NULL);
                av_set_string3(c,"flags","+loop",1,NULL);
                av_set_string3(c,"cmp","+chroma",1,NULL);
                av_set_string3(c,"partitions","-parti8x8-parti4x4-partp8x8-partb8x8",1,NULL);
                av_set_string3(c,"me_method","dia",1,NULL);
                av_set_string3(c,"subq","2",1,NULL);
                av_set_string3(c,"me_range","16",1,NULL);
                av_set_string3(c,"g","250",1,NULL);
                av_set_string3(c,"keyint_min","25",1,NULL);
                av_set_string3(c,"sc_threshold","40",1,NULL);
                av_set_string3(c,"i_qfactor","0.71",1,NULL);
                av_set_string3(c,"b_strategy","1",1,NULL);
                av_set_string3(c,"qcomp","0.6",1,NULL);
                av_set_string3(c,"qmin","10",1,NULL);
                av_set_string3(c,"qmax","51",1,NULL);
                av_set_string3(c,"qdiff","4",1,NULL);
                av_set_string3(c,"bf","3",1,NULL);
                av_set_string3(c,"refs","1",1,NULL);
                av_set_string3(c,"directpred","1",1,NULL);
                av_set_string3(c,"trellis","0",1,NULL);
                av_set_string3(c,"flags2","+bpyramid-mixed_refs+wpred-dct8x8+fastpskip",1,NULL);
                av_set_string3(c,"wpredp","2",1,NULL);
            }
            break;
            case 6:
            {
                av_set_string3(c,"coder","1",1,NULL);
                av_set_string3(c,"flags","+loop",1,NULL);
                av_set_string3(c,"cmp","+chroma",1,NULL);
                av_set_string3(c,"partitions","-parti8x8-parti4x4-partp8x8-partb8x8",1,NULL);
                av_set_string3(c,"me_method","dia",1,NULL);
                av_set_string3(c,"subq","2",1,NULL);
                av_set_string3(c,"me_range","16",1,NULL);
                av_set_string3(c,"g","250",1,NULL);
                av_set_string3(c,"keyint_min","25",1,NULL);
                av_set_string3(c,"sc_threshold","40",1,NULL);
                av_set_string3(c,"i_qfactor","0.71",1,NULL);
                av_set_string3(c,"b_strategy","1",1,NULL);
                av_set_string3(c,"qcomp","0.6",1,NULL);
                av_set_string3(c,"qmin","10",1,NULL);
                av_set_string3(c,"qmax","51",1,NULL);
                av_set_string3(c,"qdiff","4",1,NULL);
                av_set_string3(c,"bf","3",1,NULL);
                av_set_string3(c,"refs","1",1,NULL);
                av_set_string3(c,"directpred","1",1,NULL);
                av_set_string3(c,"trellis","0",1,NULL);
                av_set_string3(c,"flags2","+bpyramid-mixed_refs+wpred-dct8x8+fastpskip",1,NULL);
                av_set_string3(c,"wpredp","2",1,NULL);
                av_set_string3(c,"rc_lookahead","30",1,NULL);
            }
            break;
            case 7:
            {
                av_set_string3(c,"coder","1",1,NULL);
                av_set_string3(c,"flags","+loop",1,NULL);
                av_set_string3(c,"cmp","+chroma",1,NULL);
                av_set_string3(c,"partitions","-parti8x8-parti4x4-partp8x8-partb8x8",1,NULL);
                av_set_string3(c,"me_method","dia",1,NULL);
                av_set_string3(c,"subq","2",1,NULL);
                av_set_string3(c,"me_range","16",1,NULL);
                av_set_string3(c,"g","250",1,NULL);
                av_set_string3(c,"keyint_min","25",1,NULL);
                av_set_string3(c,"sc_threshold","40",1,NULL);
                av_set_string3(c,"i_qfactor","0.71",1,NULL);
                av_set_string3(c,"b_strategy","1",1,NULL);
                av_set_string3(c,"qcomp","0.6",1,NULL);
                av_set_string3(c,"qmin","10",1,NULL);
                av_set_string3(c,"qmax","51",1,NULL);
                av_set_string3(c,"qdiff","4",1,NULL);
                av_set_string3(c,"bf","3",1,NULL);
                av_set_string3(c,"refs","1",1,NULL);
                av_set_string3(c,"directpred","1",1,NULL);
                av_set_string3(c,"trellis","0",1,NULL);
                av_set_string3(c,"flags2","+bpyramid-mixed_refs+wpred-dct8x8+fastpskip",1,NULL);
                av_set_string3(c,"wpredp","1",1,NULL);
                av_set_string3(c,"rc_lookahead","20",1,NULL);
            }
            break;
            case 8:
            {
                av_set_string3(c,"coder","1",1,NULL);
                av_set_string3(c,"flags","+loop",1,NULL);
                av_set_string3(c,"cmp","+chroma",1,NULL);
                av_set_string3(c,"partitions","-parti8x8-parti4x4-partp8x8-partb8x8",1,NULL);
                av_set_string3(c,"me_method","dia",1,NULL);
                av_set_string3(c,"subq","2",1,NULL);
                av_set_string3(c,"me_range","16",1,NULL);
                av_set_string3(c,"g","250",1,NULL);
                av_set_string3(c,"keyint_min","25",1,NULL);
                av_set_string3(c,"sc_threshold","40",1,NULL);
                av_set_string3(c,"i_qfactor","0.71",1,NULL);
                av_set_string3(c,"b_strategy","1",1,NULL);
                av_set_string3(c,"qcomp","0.6",1,NULL);
                av_set_string3(c,"qmin","10",1,NULL);
                av_set_string3(c,"qmax","51",1,NULL);
                av_set_string3(c,"qdiff","4",1,NULL);
                av_set_string3(c,"bf","3",1,NULL);
                av_set_string3(c,"refs","1",1,NULL);
                av_set_string3(c,"directpred","1",1,NULL);
                av_set_string3(c,"trellis","0",1,NULL);
                av_set_string3(c,"flags2","+bpyramid-mixed_refs+wpred-dct8x8+fastpskip",1,NULL);
                av_set_string3(c,"wpredp","0",1,NULL);
                av_set_string3(c,"rc_lookahead","10",1,NULL);
            }
            break;
            case 9:
            {
                av_set_string3(c,"coder","1",1,NULL);
                av_set_string3(c,"flags","+loop",1,NULL);
                av_set_string3(c,"cmp","+chroma",1,NULL);
                av_set_string3(c,"partitions","-parti8x8-parti4x4-partp8x8-partb8x8",1,NULL);
                av_set_string3(c,"me_method","dia",1,NULL);
                av_set_string3(c,"subq","1",1,NULL);
                av_set_string3(c,"me_range","16",1,NULL);
                av_set_string3(c,"g","250",1,NULL);
                av_set_string3(c,"keyint_min","25",1,NULL);
                av_set_string3(c,"sc_threshold","40",1,NULL);
                av_set_string3(c,"i_qfactor","0.71",1,NULL);
                av_set_string3(c,"b_strategy","1",1,NULL);
                av_set_string3(c,"qcomp","0.6",1,NULL);
                av_set_string3(c,"qmin","10",1,NULL);
                av_set_string3(c,"qmax","51",1,NULL);
                av_set_string3(c,"qdiff","4",1,NULL);
                av_set_string3(c,"bf","3",1,NULL);
                av_set_string3(c,"refs","1",1,NULL);
                av_set_string3(c,"directpred","1",1,NULL);
                av_set_string3(c,"trellis","0",1,NULL);
                av_set_string3(c,"flags2","+bpyramid-mixed_refs+wpred-dct8x8+fastpskip-mbtree",1,NULL);
                av_set_string3(c,"wpredp","0",1,NULL);
                av_set_string3(c,"rc_lookahead","0",1,NULL);
            }
            break;
            case 10:
            {
                av_set_string3(c,"coder","0",1,NULL);
                av_set_string3(c,"flags","-loop",1,NULL);
                av_set_string3(c,"cmp","+chroma",1,NULL);
                av_set_string3(c,"partitions","-parti8x8-parti4x4-partp8x8-partb8x8",1,NULL);
                av_set_string3(c,"me_method","dia",1,NULL);
                av_set_string3(c,"subq","0",1,NULL);
                av_set_string3(c,"me_range","16",1,NULL);
                av_set_string3(c,"g","250",1,NULL);
                av_set_string3(c,"keyint_min","25",1,NULL);
                av_set_string3(c,"sc_threshold","0",1,NULL);
                av_set_string3(c,"i_qfactor","0.71",1,NULL);
                av_set_string3(c,"b_strategy","0",1,NULL);
                av_set_string3(c,"qcomp","0.6",1,NULL);
                av_set_string3(c,"qmin","10",1,NULL);
                av_set_string3(c,"qmax","51",1,NULL);
                av_set_string3(c,"qdiff","4",1,NULL);
                av_set_string3(c,"bf","0",1,NULL);
                av_set_string3(c,"refs","1",1,NULL);
                av_set_string3(c,"directpred","1",1,NULL);
                av_set_string3(c,"trellis","0",1,NULL);
                av_set_string3(c,"flags2","-bpyramid-mixed_refs-wpred-dct8x8+fastpskip-mbtree",1,NULL);
                av_set_string3(c,"wpredp","0",1,NULL);
                av_set_string3(c,"aq_mode","0",1,NULL);
                av_set_string3(c,"rc_lookahead","0",1,NULL);
            }
            break;
            }
        }
        else
        {
            switch(preset)
            {
            case 1:
            {
                av_set_string3(c,"coder","1",1,NULL);
                av_set_string3(c,"flags","+loop",1,NULL);
                av_set_string3(c,"cmp","+chroma",1,NULL);
                av_set_string3(c,"partitions","+parti8x8+parti4x4+partp8x8+partp4x4+partb8x8",1,NULL);
                av_set_string3(c,"me_method","tesa",1,NULL);
                av_set_string3(c,"subq","10",1,NULL);
                av_set_string3(c,"me_range","24",1,NULL);
                av_set_string3(c,"g","250",1,NULL);
                av_set_string3(c,"keyint_min","25",1,NULL);
                av_set_string3(c,"sc_threshold","40",1,NULL);
                av_set_string3(c,"i_qfactor","0.71",1,NULL);
                av_set_string3(c,"b_strategy","2",1,NULL);
                av_set_string3(c,"qcomp","0.6",1,NULL);
                av_set_string3(c,"qmin","10",1,NULL);
                av_set_string3(c,"qmax","51",1,NULL);
                av_set_string3(c,"qdiff","4",1,NULL);
                av_set_string3(c,"bf","16",1,NULL);
                av_set_string3(c,"refs","16",1,NULL);
                av_set_string3(c,"directpred","3",1,NULL);
                av_set_string3(c,"trellis","2",1,NULL);
                av_set_string3(c,"flags2","+bpyramid+mixed_refs+wpred+dct8x8-fastpskip",1,NULL);
                av_set_string3(c,"wpredp","2",1,NULL);
                av_set_string3(c,"rc_lookahead","60",1,NULL);
            }
            break;
            case 2:
            {
                av_set_string3(c,"coder","1",1,NULL);
                av_set_string3(c,"flags","+loop",1,NULL);
                av_set_string3(c,"cmp","+chroma",1,NULL);
                av_set_string3(c,"partitions","+parti8x8+parti4x4+partp8x8+partp4x4+partb8x8",1,NULL);
                av_set_string3(c,"me_method","umh",1,NULL);
                av_set_string3(c,"subq","10",1,NULL);
                av_set_string3(c,"me_range","24",1,NULL);
                av_set_string3(c,"g","250",1,NULL);
                av_set_string3(c,"keyint_min","25",1,NULL);
                av_set_string3(c,"sc_threshold","40",1,NULL);
                av_set_string3(c,"i_qfactor","0.71",1,NULL);
                av_set_string3(c,"b_strategy","2",1,NULL);
                av_set_string3(c,"qcomp","0.6",1,NULL);
                av_set_string3(c,"qmin","10",1,NULL);
                av_set_string3(c,"qmax","51",1,NULL);
                av_set_string3(c,"qdiff","4",1,NULL);
                av_set_string3(c,"bf","8",1,NULL);
                av_set_string3(c,"refs","16",1,NULL);
                av_set_string3(c,"directpred","3",1,NULL);
                av_set_string3(c,"trellis","2",1,NULL);
                av_set_string3(c,"flags2","+bpyramid+mixed_refs+wpred+dct8x8+fastpskip",1,NULL);
                av_set_string3(c,"wpredp","2",1,NULL);
                av_set_string3(c,"rc_lookahead","60",1,NULL);
            }
            break;
            case 3:
            {
                av_set_string3(c,"coder","1",1,NULL);
                av_set_string3(c,"flags","+loop",1,NULL);
                av_set_string3(c,"cmp","+chroma",1,NULL);
                av_set_string3(c,"partitions","+parti8x8+parti4x4+partp8x8+partp4x4+partb8x8",1,NULL);
                av_set_string3(c,"me_method","umh",1,NULL);
                av_set_string3(c,"subq","9",1,NULL);
                av_set_string3(c,"me_range","16",1,NULL);
                av_set_string3(c,"g","250",1,NULL);
                av_set_string3(c,"keyint_min","25",1,NULL);
                av_set_string3(c,"sc_threshold","40",1,NULL);
                av_set_string3(c,"i_qfactor","0.71",1,NULL);
                av_set_string3(c,"b_strategy","2",1,NULL);
                av_set_string3(c,"qcomp","0.6",1,NULL);
                av_set_string3(c,"qmin","10",1,NULL);
                av_set_string3(c,"qmax","51",1,NULL);
                av_set_string3(c,"qdiff","4",1,NULL);
                av_set_string3(c,"bf","3",1,NULL);
                av_set_string3(c,"refs","8",1,NULL);
                av_set_string3(c,"directpred","3",1,NULL);
                av_set_string3(c,"trellis","2",1,NULL);
                av_set_string3(c,"flags2","+bpyramid+mixed_refs+wpred+dct8x8+fastpskip",1,NULL);
                av_set_string3(c,"wpredp","2",1,NULL);
                av_set_string3(c,"rc_lookahead","60",1,NULL);
            }
            break;
            case 4:
            {
                av_set_string3(c,"coder","1",1,NULL);
                av_set_string3(c,"flags","+loop",1,NULL);
                av_set_string3(c,"cmp","+chroma",1,NULL);
                av_set_string3(c,"partitions","+parti8x8+parti4x4+partp8x8+partb8x8",1,NULL);
                av_set_string3(c,"me_method","umh",1,NULL);
                av_set_string3(c,"subq","8",1,NULL);
                av_set_string3(c,"me_range","16",1,NULL);
                av_set_string3(c,"g","250",1,NULL);
                av_set_string3(c,"keyint_min","25",1,NULL);
                av_set_string3(c,"sc_threshold","40",1,NULL);
                av_set_string3(c,"i_qfactor","0.71",1,NULL);
                av_set_string3(c,"b_strategy","2",1,NULL);
                av_set_string3(c,"qcomp","0.6",1,NULL);
                av_set_string3(c,"qmin","10",1,NULL);
                av_set_string3(c,"qmax","51",1,NULL);
                av_set_string3(c,"qdiff","4",1,NULL);
                av_set_string3(c,"bf","3",1,NULL);
                av_set_string3(c,"refs","5",1,NULL);
                av_set_string3(c,"directpred","3",1,NULL);
                av_set_string3(c,"trellis","1",1,NULL);
                av_set_string3(c,"flags2","+bpyramid+mixed_refs+wpred+dct8x8+fastpskip",1,NULL);
                av_set_string3(c,"wpredp","2",1,NULL);
                av_set_string3(c,"rc_lookahead","50",1,NULL);
            }
            break;
            case 5:
            {
                av_set_string3(c,"coder","1",1,NULL);
                av_set_string3(c,"flags","+loop",1,NULL);
                av_set_string3(c,"cmp","+chroma",1,NULL);
                av_set_string3(c,"partitions","+parti8x8+parti4x4+partp8x8+partb8x8",1,NULL);
                av_set_string3(c,"me_method","hex",1,NULL);
                av_set_string3(c,"subq","7",1,NULL);
                av_set_string3(c,"me_range","16",1,NULL);
                av_set_string3(c,"g","250",1,NULL);
                av_set_string3(c,"keyint_min","25",1,NULL);
                av_set_string3(c,"sc_threshold","40",1,NULL);
                av_set_string3(c,"i_qfactor","0.71",1,NULL);
                av_set_string3(c,"b_strategy","1",1,NULL);
                av_set_string3(c,"qcomp","0.6",1,NULL);
                av_set_string3(c,"qmin","10",1,NULL);
                av_set_string3(c,"qmax","51",1,NULL);
                av_set_string3(c,"qdiff","4",1,NULL);
                av_set_string3(c,"bf","3",1,NULL);
                av_set_string3(c,"refs","3",1,NULL);
                av_set_string3(c,"directpred","1",1,NULL);
                av_set_string3(c,"trellis","1",1,NULL);
                av_set_string3(c,"flags2","+bpyramid+mixed_refs+wpred+dct8x8+fastpskip",1,NULL);
                av_set_string3(c,"wpredp","2",1,NULL);
            }
            break;
            case 6:
            {
                av_set_string3(c,"coder","1",1,NULL);
                av_set_string3(c,"flags","+loop",1,NULL);
                av_set_string3(c,"cmp","+chroma",1,NULL);
                av_set_string3(c,"partitions","+parti8x8+parti4x4+partp8x8+partb8x8",1,NULL);
                av_set_string3(c,"me_method","hex",1,NULL);
                av_set_string3(c,"subq","6",1,NULL);
                av_set_string3(c,"me_range","16",1,NULL);
                av_set_string3(c,"g","250",1,NULL);
                av_set_string3(c,"keyint_min","25",1,NULL);
                av_set_string3(c,"sc_threshold","40",1,NULL);
                av_set_string3(c,"i_qfactor","0.71",1,NULL);
                av_set_string3(c,"b_strategy","1",1,NULL);
                av_set_string3(c,"qcomp","0.6",1,NULL);
                av_set_string3(c,"qmin","10",1,NULL);
                av_set_string3(c,"qmax","51",1,NULL);
                av_set_string3(c,"qdiff","4",1,NULL);
                av_set_string3(c,"bf","3",1,NULL);
                av_set_string3(c,"refs","2",1,NULL);
                av_set_string3(c,"directpred","1",1,NULL);
                av_set_string3(c,"trellis","1",1,NULL);
                av_set_string3(c,"flags2","+bpyramid+mixed_refs+wpred+dct8x8+fastpskip",1,NULL);
                av_set_string3(c,"wpredp","2",1,NULL);
                av_set_string3(c,"rc_lookahead","30",1,NULL);
            }
            break;
            case 7:
            {
                av_set_string3(c,"coder","1",1,NULL);
                av_set_string3(c,"flags","+loop",1,NULL);
                av_set_string3(c,"cmp","+chroma",1,NULL);
                av_set_string3(c,"partitions","+parti8x8+parti4x4+partp8x8+partb8x8",1,NULL);
                av_set_string3(c,"me_method","hex",1,NULL);
                av_set_string3(c,"subq","4",1,NULL);
                av_set_string3(c,"me_range","16",1,NULL);
                av_set_string3(c,"g","250",1,NULL);
                av_set_string3(c,"keyint_min","25",1,NULL);
                av_set_string3(c,"sc_threshold","40",1,NULL);
                av_set_string3(c,"i_qfactor","0.71",1,NULL);
                av_set_string3(c,"b_strategy","1",1,NULL);
                av_set_string3(c,"qcomp","0.6",1,NULL);
                av_set_string3(c,"qmin","10",1,NULL);
                av_set_string3(c,"qmax","51",1,NULL);
                av_set_string3(c,"qdiff","4",1,NULL);
                av_set_string3(c,"bf","3",1,NULL);
                av_set_string3(c,"refs","2",1,NULL);
                av_set_string3(c,"directpred","1",1,NULL);
                av_set_string3(c,"trellis","1",1,NULL);
                av_set_string3(c,"flags2","+bpyramid-mixed_refs+wpred+dct8x8+fastpskip",1,NULL);
                av_set_string3(c,"wpredp","1",1,NULL);
                av_set_string3(c,"rc_lookahead","20",1,NULL);
            }
            break;
            case 8:
            {
                av_set_string3(c,"coder","1",1,NULL);
                av_set_string3(c,"flags","+loop",1,NULL);
                av_set_string3(c,"cmp","+chroma",1,NULL);
                av_set_string3(c,"partitions","+parti8x8+parti4x4+partp8x8+partb8x8",1,NULL);
                av_set_string3(c,"me_method","hex",1,NULL);
                av_set_string3(c,"subq","2",1,NULL);
                av_set_string3(c,"me_range","16",1,NULL);
                av_set_string3(c,"g","250",1,NULL);
                av_set_string3(c,"keyint_min","25",1,NULL);
                av_set_string3(c,"sc_threshold","40",1,NULL);
                av_set_string3(c,"i_qfactor","0.71",1,NULL);
                av_set_string3(c,"b_strategy","1",1,NULL);
                av_set_string3(c,"qcomp","0.6",1,NULL);
                av_set_string3(c,"qmin","10",1,NULL);
                av_set_string3(c,"qmax","51",1,NULL);
                av_set_string3(c,"qdiff","4",1,NULL);
                av_set_string3(c,"bf","3",1,NULL);
                av_set_string3(c,"refs","1",1,NULL);
                av_set_string3(c,"directpred","1",1,NULL);
                av_set_string3(c,"trellis","0",1,NULL);
                av_set_string3(c,"flags2","+bpyramid-mixed_refs+wpred+dct8x8+fastpskip",1,NULL);
                av_set_string3(c,"wpredp","0",1,NULL);
                av_set_string3(c,"rc_lookahead","10",1,NULL);
            }
            break;
            case 9:
            {
                av_set_string3(c,"coder","1",1,NULL);
                av_set_string3(c,"flags","+loop",1,NULL);
                av_set_string3(c,"cmp","+chroma",1,NULL);
                av_set_string3(c,"partitions","+parti8x8+parti4x4-partp8x8-partb8x8",1,NULL);
                av_set_string3(c,"me_method","dia",1,NULL);
                av_set_string3(c,"subq","1",1,NULL);
                av_set_string3(c,"me_range","16",1,NULL);
                av_set_string3(c,"g","250",1,NULL);
                av_set_string3(c,"keyint_min","25",1,NULL);
                av_set_string3(c,"sc_threshold","40",1,NULL);
                av_set_string3(c,"i_qfactor","0.71",1,NULL);
                av_set_string3(c,"b_strategy","1",1,NULL);
                av_set_string3(c,"qcomp","0.6",1,NULL);
                av_set_string3(c,"qmin","10",1,NULL);
                av_set_string3(c,"qmax","51",1,NULL);
                av_set_string3(c,"qdiff","4",1,NULL);
                av_set_string3(c,"bf","3",1,NULL);
                av_set_string3(c,"refs","1",1,NULL);
                av_set_string3(c,"directpred","1",1,NULL);
                av_set_string3(c,"trellis","0",1,NULL);
                av_set_string3(c,"flags2","+bpyramid-mixed_refs+wpred+dct8x8+fastpskip-mbtree",1,NULL);
                av_set_string3(c,"wpredp","0",1,NULL);
                av_set_string3(c,"rc_lookahead","0",1,NULL);
            }
            break;
            case 10:
            {
                av_set_string3(c,"coder","0",1,NULL);
                av_set_string3(c,"flags","-loop",1,NULL);
                av_set_string3(c,"cmp","+chroma",1,NULL);
                av_set_string3(c,"partitions","-parti8x8-parti4x4-partp8x8-partb8x8",1,NULL);
                av_set_string3(c,"me_method","dia",1,NULL);
                av_set_string3(c,"subq","0",1,NULL);
                av_set_string3(c,"me_range","16",1,NULL);
                av_set_string3(c,"g","250",1,NULL);
                av_set_string3(c,"keyint_min","25",1,NULL);
                av_set_string3(c,"sc_threshold","0",1,NULL);
                av_set_string3(c,"i_qfactor","0.71",1,NULL);
                av_set_string3(c,"b_strategy","0",1,NULL);
                av_set_string3(c,"qcomp","0.6",1,NULL);
                av_set_string3(c,"qmin","10",1,NULL);
                av_set_string3(c,"qmax","51",1,NULL);
                av_set_string3(c,"qdiff","4",1,NULL);
                av_set_string3(c,"bf","0",1,NULL);
                av_set_string3(c,"refs","1",1,NULL);
                av_set_string3(c,"directpred","1",1,NULL);
                av_set_string3(c,"trellis","0",1,NULL);
                av_set_string3(c,"flags2","-bpyramid-mixed_refs-wpred-dct8x8+fastpskip-mbtree",1,NULL);
                av_set_string3(c,"wpredp","0",1,NULL);
                av_set_string3(c,"aq_mode","0",1,NULL);
                av_set_string3(c,"rc_lookahead","0",1,NULL);
            }
            break;

            }
        }
    }

}


static AVStream *add_video_stream(AVFormatContext *oc,const Movie::Info & info)
{

    AVCodecContext *c;
    AVStream *st;

    st = av_new_stream(oc, 0);


    AVCodec *codec = avcodec_find_encoder_by_name(info.videos[0].codec_short.toCString());

    c = st->codec;

    avcodec_get_context_defaults(c);

    c->codec_id = codec->id;
    c->codec_type = AVMEDIA_TYPE_VIDEO;

    if(info.videos[0].codec_short == "libxvid")
    {
        c->codec_tag = AV_RL32("xvid");
        //c->codec_id = CODEC_ID_XVID;
    }else if(info.videos[0].codec_short == "libx264")
    {
        is_codec_x264 = true;
    }

    /* put sample parameters */
   c->bit_rate = info.videos[0].bit_rate * 1000;
   //c->qmin = 0; c->qmax = info.videos[0].bit_rate;

    //c->flags |= CODEC_FLAG_QSCALE;
    //c->global_quality = st->quality = FF_QP2LAMBDA * (float)info.videos[0].bit_rate;

    /* resolution must be a multiple of two */
    c->width = info.videos[0].width;
    c->height = info.videos[0].height;

    AVRational sample_aspect_ratio;
    sample_aspect_ratio.den = 1;
    sample_aspect_ratio.num = 1;
    st->sample_aspect_ratio =
        c->sample_aspect_ratio = st->sample_aspect_ratio = sample_aspect_ratio;
    /* time base: this is the fundamental unit of time (in seconds) in terms
       of which frame timestamps are represented. for fixed-fps content,
       timebase should be 1/framerate and timestamp increments should be
       identically 1. */
    AVRational fps = av_d2q(info.videos[0].fps,65535);

    if (codec->supported_framerates)
        fps = codec->supported_framerates[av_find_nearest_q_idx(fps, codec->supported_framerates)];
    swapVariables(fps.num,fps.den);
    c->time_base = st->time_base = st->r_frame_rate = fps;

    c->gop_size = info.videos[0].gop; /* emit one intra frame every twelve frames at most */
    //c->max_b_frames = 2;
    //c->flags |= CODEC_FLAG_QSCALE;
    c->pix_fmt = PIX_FMT_YUV420P;

    setCompressionPreset(c,info.videos[0].compressionPreset,(all_pass>1)?current_pass:2);

    // some formats want stream headers to be separate
    if(oc->oformat->flags & AVFMT_GLOBALHEADER)
        c->flags |= CODEC_FLAG_GLOBAL_HEADER;

    //c->flags |= CODEC_FLAG2_LOCAL_HEADER;

    if(all_pass>1)
    {
        if(current_pass == 1)
            c->flags |= CODEC_FLAG_PASS1;
        else
        {
            c->flags |= CODEC_FLAG_PASS2;

            c->stats_in = const_cast<char *>(pass_info.toCString());
        }
    }
    avcodec_open(c, codec);

    return st;
}

static AVStream *add_audio_stream(AVFormatContext *oc, enum CodecID codec_id,const Movie::Info & info)
{
    AVCodecContext *c;
    AVStream *st;

    st = av_new_stream(oc, 1);

    c = st->codec;
    c->codec_id = codec_id;
    c->codec_type = AVMEDIA_TYPE_AUDIO;



    /* put sample parameters */
    c->sample_fmt = AV_SAMPLE_FMT_S16;
    c->bit_rate = 64000;
    c->sample_rate = 44100;
    c->channels = 2;

    // some formats want stream headers to be separate
    if(oc->oformat->flags & AVFMT_GLOBALHEADER)
        c->flags |= CODEC_FLAG_GLOBAL_HEADER;

    return st;
}

static void open_audio(AVFormatContext *oc, AVStream *st)
{
    AVCodecContext *c;
    AVCodec *codec;

    c = st->codec;

    /* find the audio encoder */
    codec = avcodec_find_encoder(c->codec_id);

    /* open it */
    avcodec_open(c, codec) < 0;

    /* init signal generator */
    t = 0;
    tincr = 2 * M_PI * 110.0 / c->sample_rate;
    /* increment frequency by 110 Hz per second */
    tincr2 = 2 * M_PI * 110.0 / c->sample_rate / c->sample_rate;

    audio_outbuf_size = 1000000;
    audio_outbuf = (uint8_t *)av_malloc(audio_outbuf_size);

    /* ugly hack for PCM codecs (will be removed ASAP with new PCM
       support to compute the input frame size in samples */
    if (c->frame_size <= 1)
    {
        audio_input_frame_size = audio_outbuf_size / c->channels;
        switch(st->codec->codec_id)
        {
        case CODEC_ID_PCM_S16LE:
        case CODEC_ID_PCM_S16BE:
        case CODEC_ID_PCM_U16LE:
        case CODEC_ID_PCM_U16BE:
            audio_input_frame_size >>= 1;
            break;
        default:
            break;
        }
    }
    else
    {
        audio_input_frame_size = c->frame_size;
    }
    samples = (int16_t *)av_malloc(audio_input_frame_size * 2 * c->channels);
}

static AVFrame *alloc_picture(enum PixelFormat pix_fmt, int width, int height)
{
    AVFrame *picture;
    uint8_t *picture_buf;
    int size;

    picture = avcodec_alloc_frame();
    if (!picture)
        return NULL;
    size = avpicture_get_size(pix_fmt, width, height);
    picture_buf = (uint8_t *)av_malloc(size);
    if (!picture_buf)
    {
        av_free(picture);
        return NULL;
    }
    avpicture_fill((AVPicture *)picture, picture_buf,
                   pix_fmt, width, height);
    return picture;
}

static void open_video(AVFormatContext *oc, AVStream *st)
{
    AVCodecContext *c;

    c = st->codec;


    video_outbuf = NULL;
    if (!(oc->oformat->flags & AVFMT_RAWPICTURE))
    {
        /* allocate output buffer */
        /* XXX: API change will be done */
        /* buffers passed into lav* can be allocated any way you prefer,
           as long as they're aligned enough for the architecture, and
           they're freed appropriately (such as using av_free for buffers
           allocated with av_malloc) */
        video_outbuf_size = 20000000;
        video_outbuf = (uint8_t *)av_malloc(video_outbuf_size);
    }

    /* allocate the encoded raw picture */
    picture = alloc_picture(c->pix_fmt, c->width, c->height);


    /* if the output format is not YUV420P, then a temporary YUV420P
       picture is needed too. It is then converted to the required
       output format */
}

static void close_video(AVFormatContext *oc, AVStream *st)
{
    avcodec_close(st->codec);
    if(img_convert_ctx != NULL )
    {
        sws_freeContext(img_convert_ctx);
        img_convert_ctx = NULL;
    }
    av_free(picture->data[0]);
    av_free(picture);
    av_free(video_outbuf);
}

static void close_audio(AVFormatContext *oc, AVStream *st)
{
    avcodec_close(st->codec);

    av_free(samples);
    av_free(audio_outbuf);
}

/* prepare a dummy image */
static void fill_frame(AVFrame *pict, int frame_index, const Movie::Info& info, Timeline * timeline,PixelFormat pix_fmt)
{

    Movie::VideoInfo video_info = info.videos[0];
    if(!timeline->current_interval)
    {
        //Make black image

        int height = video_info.height;
        int width = video_info.width;

        /* Y */memset(pict->data[0],0,(height-1) * pict->linesize[0] + width-1);
        /* U */memset(pict->data[1],128,(height/2-1) * pict->linesize[1] + width/2-1);
        /* V */memset(pict->data[2],128,(height/2-1) * pict->linesize[2] + width/2-1);

        end_writing = !timeline->SkipFrame();
        return;
    }
    Movie * movie = timeline->current_interval->movie;

    if(
        img_convert_ctx == NULL
        || srcW != movie->width
        || srcH != movie->height
        || dstW != video_info.width
        || dstH != video_info.height
        || srcFormat != movie->GetMovieInfo()->videos[0].pix_fmt
        || dstFormat != pix_fmt
    )
    {
        srcW = movie->width;
        srcH = movie->height;
        dstW = video_info.width;
        dstH = video_info.height;
        srcFormat = movie->GetMovieInfo()->videos[0].pix_fmt;
        dstFormat = pix_fmt;

        if(img_convert_ctx != NULL )
        {
            sws_freeContext(img_convert_ctx);
            img_convert_ctx = NULL;
        }
        img_convert_ctx = sws_getContext(srcW, srcH,
                                         srcFormat,
                                         dstW, dstH,
                                         dstFormat,
                                         sws_flags, NULL, NULL, NULL);
    }


    sws_scale(img_convert_ctx, movie->pFrame->data, movie->pFrame->linesize,
              0, movie->height, pict->data, pict->linesize);

    end_writing = !timeline->SkipFrame();

}


static bool write_video_frame(AVFormatContext *oc, AVStream *st, const Movie::Info& info, Timeline * timeline)
{
    int out_size;
    AVCodecContext *c;
    c = st->codec;


    {
        fill_frame(picture, frame_count, info, timeline, c->pix_fmt);
    }

    //picture->quality = info.videos[0].bit_rate;

    if (oc->oformat->flags & AVFMT_RAWPICTURE)
    {
        /* raw video case. The API will change slightly in the near
           futur for that */
        AVPacket pkt;
        av_init_packet(&pkt);

        pkt.flags |= AV_PKT_FLAG_KEY;
        pkt.stream_index= st->index;
        pkt.data= (uint8_t *)picture;
        pkt.size= sizeof(AVPicture);

        av_interleaved_write_frame(oc, &pkt);
    }
    else
    {
        /* encode the image */

        if(is_codec_x264)
            picture->pts = pts++;

        for(;;)
        {
            /* flushing buffers */
            AVFrame * send_picture = (end_writing)?NULL:picture;

            out_size = avcodec_encode_video(c, video_outbuf, video_outbuf_size, send_picture);
            /* if zero size, it means the image was buffered */

            if (out_size <= 0)
                break;
            AVPacket pkt;
            av_init_packet(&pkt);

            if (c->coded_frame->pts != AV_NOPTS_VALUE)
                pkt.pts= av_rescale_q(c->coded_frame->pts, c->time_base, st->time_base);
            if(c->coded_frame->key_frame)
                pkt.flags |= AV_PKT_FLAG_KEY;
            pkt.stream_index= st->index;
            pkt.data = video_outbuf;
            pkt.size = out_size;

            /* write the compressed frame in the media file */
            av_interleaved_write_frame(oc, &pkt);
            if(all_pass>1 && c->stats_out)
                pass_info<<c->stats_out;
            if (!end_writing)
                break;

        }

    }

    frame_count++;
    return true;
}

/* prepare a 16 bit dummy audio frame of 'frame_size' samples and
   'nb_channels' channels */
static void get_audio_frame(int16_t *samples, int frame_size, int nb_channels, const Movie::Info& info, Timeline * timeline)
{
    int j, i, v;
    int16_t *q;

    q = samples;
    for(j=0; j<frame_size; j++)
    {
        v = (int)(sin(t) * 10000);
        for(i = 0; i < nb_channels; i++)
            *q++ = v;
        t += tincr;
        tincr += tincr2;
    }
}

static bool write_audio_frame(AVFormatContext *oc, AVStream *st, const Movie::Info& info, Timeline * timeline)
{
    AVCodecContext *c;
    AVPacket pkt;
    av_init_packet(&pkt);

    c = st->codec;
    get_audio_frame(samples, audio_input_frame_size, c->channels, info, timeline);

    pkt.size= avcodec_encode_audio(c, audio_outbuf, audio_outbuf_size, samples);

    if (c->coded_frame && c->coded_frame->pts != AV_NOPTS_VALUE)
        pkt.pts= av_rescale_q(c->coded_frame->pts, c->time_base, st->time_base);
    pkt.flags |= AV_PKT_FLAG_KEY;
    pkt.stream_index= st->index;
    pkt.data= audio_outbuf;

    /* write the compressed frame in the media file */
    if (av_interleaved_write_frame(oc, &pkt) != 0)
    {
        return false;
    }
    return true;
}

bool Timeline::Render(const Movie::Info & info)
{
    bool video_enabled = info.videos.size()>0;
    pass_info=String::empty;
    is_codec_x264 = false;

    all_pass = (video_enabled)?info.videos[0].pass:1;

    for(current_pass=1; current_pass<=all_pass; ++current_pass)
    {
        bool audio_enabled = info.audios.size()>0 && all_pass==current_pass;

        pts = 0;

        end_writing = false;
        AVOutputFormat *fmt;
        AVFormatContext *oc;
        AVStream *audio_st, *video_st;
        double audio_pts, video_pts;
        int i;

        const char * c_string_filename = info.filename.toCString();
        fmt = av_guess_format(info.format_short.toCString(), NULL, NULL);

        oc = avformat_alloc_context();
        oc->oformat = fmt;

        snprintf(oc->filename, sizeof(oc->filename), "%s", c_string_filename);
        video_st = NULL;
        audio_st = NULL;
        if (video_enabled)
        {
            GotoSecondAndRead(0.0,false);
            video_st = add_video_stream(oc, info);
        }
        if (fmt->audio_codec != CODEC_ID_NONE && audio_enabled)
        {
            audio_st = add_audio_stream(oc, fmt->audio_codec,info);
        }
        av_set_parameters(oc, NULL);

        if (video_st)
        {
            open_video(oc, video_st);
        }
        if (audio_st)
        {
            open_audio(oc, audio_st);
        }

        url_fopen(&oc->pb, c_string_filename, URL_WRONLY);

        av_write_header(oc);



        for(;;)
        {
            /* compute current audio and video time */
            if (audio_st)
                audio_pts = (double)audio_st->pts.val * audio_st->time_base.num / audio_st->time_base.den;
            else
                audio_pts = 0.0;

            if (video_st)
                video_pts = (double)video_st->pts.val * video_st->time_base.num / video_st->time_base.den;
            else
                video_pts = 0.0;

            /*if ((!audio_st || audio_pts >= STREAM_DURATION) &&
                (!video_st || video_pts >= STREAM_DURATION))
                break;*/
            if(end_writing)
                break;

            /* write interleaved audio and video frames */
            if (!video_st || (video_st && audio_st && audio_pts < video_pts))
            {

                write_audio_frame(oc, audio_st,info,this);
            }
            else
            {
                write_video_frame(oc, video_st,info,this);
            }
        }
        av_write_trailer(oc);

        if (video_st)
        {
            close_video(oc, video_st);
        }
        if (audio_st)
        {
            close_audio(oc, audio_st);
        }

        for(i = 0; i < oc->nb_streams; i++)
        {
            av_freep(&oc->streams[i]->codec);
            av_freep(&oc->streams[i]);
        }

        if (!(fmt->flags & AVFMT_NOFILE))
        {

            url_fclose(oc->pb);
        }


        av_free(oc);
    }
    return true;
}

