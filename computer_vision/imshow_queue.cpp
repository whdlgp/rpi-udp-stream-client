#include "imshow_queue.h"

#include "unistd.h"
#include <pthread.h>
#include <queue>

#include "../common_util/common_util.h"

static int imshow_kill;
static pthread_t imshow_id;

typedef struct imshow_info_t
{
    std::string name;
    cv::Mat image;
} imshow_info_t;

std::queue<imshow_info_t> imshow_queue;

static void* imshow_thread(void* arg)
{
    while(!imshow_kill)
    {
        if(!imshow_queue.empty())
        {
            imshow_info_t imshow_info = imshow_queue.front();
            imshow_queue.pop();

            imshow(imshow_info.name, imshow_info.image);
            cv::waitKey(1);
        }
        usleep(1000);
    }
    pthread_exit((void *) 0);
}

void imshow_init()
{
    pthread_create(&imshow_id
                   , NULL
                   , imshow_thread
                   , (void*)NULL);
    DEBUG_MSG("imshow thread created\n");
}

void imshow_close()
{
    imshow_kill = 1;
    int status;
    pthread_join(imshow_id, (void**)&status);
    imshow_kill = 0;
}

void imshow_request(std::string name, cv::Mat image)
{
    imshow_info_t imshow_info;
    imshow_info.name = name;
    imshow_info.image = image;

    imshow_queue.push(imshow_info);
}
