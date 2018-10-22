#ifndef COLOR_OBJECT_RECOGNITION_H
#define COLOR_OBJECT_RECOGNITION_H

#include <iostream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

enum hsv_color_t
{
    HSV_RED_H = 0,
    HSV_RED_S = 96,
    HSV_RED_V = 93,

    HSV_GREEN_H = 68,
    HSV_GREEN_S = 55,
    HSV_GREEN_V = 65,

    HSV_BLUE_H = 110,
    HSV_BLUE_S = 60,
    HSV_BLUE_V = 80,
};

typedef struct color_object_t
{
    cv::Mat thresholded_image;
    int pos_x;
    int pos_y;
    int is_recognized;
} color_object_t;

void color_object_init();

void find_red_object(cv::Mat origin_image, color_object_t* output);
void find_green_object(cv::Mat origin_image, color_object_t* output);
void find_blue_object(cv::Mat origin_image, color_object_t* output);

#endif
