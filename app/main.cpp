#include "../udp_setup/udp_setup.h"
#include "../common_util/common_util.h"
#include "../ffmpeg_setup/ffmpeg_setup.h"

#include "opencv2/opencv.hpp"
using namespace cv;

pthread_t receive_id;
int receive_status;
pthread_t keep_alive_id;
int keep_alive_status;

#define SAVE_VIDEO

#ifdef SAVE_VIDEO
#define FILENAME "video.h264"
#endif

static void sig_int(int arg)
{
    DEBUG_MSG("request server close and exit\n");
    udp_send_command("QUIT_SERVER");

    set_quit();
    DEBUG_MSG("close and exit client\n");
    udp_client_command_close();
    udp_client_stream_close();
    
    DEBUG_MSG("wait until thread join\n");
    pthread_join(receive_id, (void **)&receive_status);
    pthread_join(keep_alive_id, (void **)&keep_alive_status);
 
    exit(0);
}

void avframe_mat_conversion(const AVFrame * frame, Mat& image)
{
    int width = frame->width;
    int height = frame->height;
 
    // Allocate the opencv mat and store its stride in a 1-element array
    if (image.rows != height 
            || image.cols != width 
            || image.type() != CV_8UC3)
    {
        image = Mat(height, width, CV_8UC3);
    }
    
    int cvLinesizes[1];
    cvLinesizes[0] = image.step1();
 
    // Convert the colour format and write directly to the opencv matrix
    SwsContext* conversion = sws_getContext(width
                                            , height
                                            , (AVPixelFormat) frame->format
                                            , width
                                            , height
                                            , PIX_FMT_BGR24
                                            , SWS_FAST_BILINEAR
                                            , NULL, NULL, NULL);
    
    sws_scale(conversion
              , frame->data
              , frame->linesize
              , 0
              , height
              , &image.data
              , cvLinesizes);

    sws_freeContext(conversion);
}

static void* receive_video_udp(void* arg)
{
    uint32_t frame_len;
    uint8_t frame_buf[FRAME_BUFSIZE];

#ifdef SAVE_VIDEO
    int fd = open (FILENAME, O_WRONLY | O_CREAT | O_TRUNC | O_APPEND, 0666);
    if (fd == -1)
    {
        DEBUG_ERR("error: open\n");
        exit(1);
    }
#endif

    ffmpeg_decode_init();

    Mat converted_image;

    while(!is_quit())
    {
        if(udp_recevie_stream(frame_buf, &frame_len))
            DEBUG_MSG("frame received size : %d\n", frame_len);
        
        AVFrame picture;
        if(ffmpeg_decode_start(frame_buf, frame_len, &picture))
        {
            DEBUG_MSG("YUV frame successfully decoded\n");
            //DEBUG_MSG("picture linesize 0 : %d\n", picture.linesize[0]);
            //DEBUG_MSG("picture linesize 1 : %d\n", picture.linesize[1]);
            //DEBUG_MSG("picture linesize 2 : %d\n", picture.linesize[2]);

            avframe_mat_conversion(&picture, converted_image);
            imshow("convert", converted_image);
            waitKey(1);
        }

#ifdef SAVE_VIDEO
        if (write(fd
                  , frame_buf
                  , frame_len) == -1)
        {
            DEBUG_ERR("error: write\n");
            exit(1);
        }
#endif
    }

#ifdef SAVE_VIDEO
    close(fd);
#endif

    ffmpeg_decode_close();
    
    pthread_exit((void *) 0);
}

static void* keep_alive(void* arg)
{
    while(!is_quit())
    {
        udp_send_command("SET_TIMEOUT");
        sleep(1);
    }

    pthread_exit((void *) 0);
}

int main(int argc, char* argv[])
{
    if(argc != 2) 
    {
       DEBUG_ERR("Usage: %s IP_address\n", argv[0]);
       exit(1);
    }

    //signal interrupt
    signal(SIGINT,  sig_int);
    signal(SIGTERM, sig_int);
    signal(SIGQUIT, sig_int);
    
    udp_set_server_ip(argv[1]);
    udp_client_command_setup();
    udp_client_stream_setup();
        
    char user_input[128];

    while(1)
    {
        if(fgets(user_input, 128, stdin) != NULL)
        {
            if(!strncmp("s", user_input, 1))
            {
                udp_send_command("VIDEO_REQUEST");

                pthread_create(&receive_id
                        , NULL
                        , receive_video_udp
                        , (void*)NULL);
                DEBUG_MSG("receive_video_udp thread created\n");

                pthread_create(&keep_alive_id
                        , NULL
                        , keep_alive
                        , (void*)NULL);
                DEBUG_MSG("keep_alive thread created\n");
            }
            else if(!strncmp("q", user_input, 1))
            {
                DEBUG_MSG("request server close and exit\n");
                udp_send_command("QUIT_SERVER");

                set_quit();
                DEBUG_MSG("close and exit client\n");
                udp_client_command_close();
                udp_client_stream_close();
                
                DEBUG_MSG("wait until thread join\n");
                pthread_join(receive_id, (void **)&receive_status);
                pthread_join(keep_alive_id, (void **)&keep_alive_status);
                
                break;
            }
        }
    }

    DEBUG_MSG("exit done\n");

    return 0;
}
