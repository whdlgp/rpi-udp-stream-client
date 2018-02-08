#ifndef IMSHOW_QUEUE_H
#define IMSHOW_QUEUE_H

#include "opencv2/opencv.hpp"
#include <string>

void imshow_init();
void imshow_close();
void imshow_request(std::string name, cv::Mat image);

#endif
