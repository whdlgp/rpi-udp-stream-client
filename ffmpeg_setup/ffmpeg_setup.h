extern "C" {
//FFmpeg include
#include "libavutil/imgutils.h"
#include "libavutil/opt.h"
#include "libavcodec/avcodec.h"
#include "libavutil/mathematics.h"
#include "libavutil/samplefmt.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
}

int ffmpeg_decode_init();
int ffmpeg_decode_close();
int ffmpeg_decode_start(unsigned char* frame, int frame_len, AVFrame* dst);
