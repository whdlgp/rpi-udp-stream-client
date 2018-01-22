#include "../udp_setup/udp_setup.h"
#include "../common_util/common_util.h"

#include <signal.h>
#include <string.h>
#include <pthread.h>
#include <fcntl.h>

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
    udp_client_close();
    
    DEBUG_MSG("wait until thread join\n");
    pthread_join(receive_id, (void **)&receive_status);
    pthread_join(keep_alive_id, (void **)&keep_alive_status);
 
    exit(0);
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

    while(!is_quit())
    {
        if(udp_recevie_stream(frame_buf, &frame_len))
            DEBUG_MSG("frame received size : %d\n", frame_len);

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
    
    udp_client_setup(argv[1]);
        
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
                udp_client_close();
                
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
