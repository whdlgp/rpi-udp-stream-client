#include "ffmpeg_setup.h"

#include "../common_util/common_util.h"

static bool isCodecActive = false;
static AVPacket avpkt; // wrapper for encoded data handshaking
static AVCodecContext *codecCtx = NULL; // codec instance
static AVFrame *picture = NULL; // ouput picture

int ffmpeg_decode_init()
{
    static bool inited = false;
    AVCodec *codec;

    if (!inited)
    {     	// only once for program
        inited = true;
        av_register_all();
        avcodec_register_all();
    }

    av_init_packet(&avpkt);

    /* find the mpeg1 video decoder */
    codec = avcodec_find_decoder(AV_CODEC_ID_H264);
    if (!codec)
    {
        fprintf(stderr, "codec not found\n");
        exit(1);
    }

    codecCtx = avcodec_alloc_context3(codec);

#if LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(55,28,1)
    picture = av_frame_alloc();
#else
    picture = avcodec_alloc_frame();

#endif
    if (codec->capabilities & CODEC_CAP_TRUNCATED)
        codecCtx->flags |= CODEC_FLAG_TRUNCATED; /* we do not send complete frames */

    /* open it */
    if (avcodec_open2(codecCtx, codec, NULL) < 0)
    {
        fprintf(stderr, "could not open codec\n");
        exit(1);
    }

    isCodecActive = true;

    return 0; // OK
}

int ffmpeg_decode_close()
{
    if (!isCodecActive)
    {
        fprintf(stderr, "Codec Close Request in inactive\n");
        return -1;
    }
    isCodecActive = false;
    avcodec_close(codecCtx);
    av_free(codecCtx);
    av_free(picture);

    return 0; // ok
}

int ffmpeg_decode_start(unsigned char* frame, int frame_len, AVFrame* dst)
{
    int res;
    int got_picture;

    avpkt.data = frame;
    avpkt.size = frame_len;

    res = avcodec_decode_video2(codecCtx, picture, &got_picture, &avpkt);
    if(res < 0)
    {
        DEBUG_ERR("Error while decoding frame\n");
    }
    
    if(got_picture)
    {
        memcpy(dst, picture, sizeof(AVFrame));
        return 1;
    }

    return 0;
}
