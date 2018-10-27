#ifndef SHAPE_OBJECT_RECOGNITION_H
#define SHAPE_OBJECT_RECOGNITION_H

#include <iostream>
#include <vector>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

enum detected_shape_t
{
    SHAPE_TRI = 0,
    SHAPE_RECT,
    SHAPE_CIRCLE,
    SHAPE_UNKNOWN,
};

typedef struct shape_object_t
{
    int detected_num;
    std::vector<int> type;
    std::vector<cv::Point> position;
    cv::Mat thresholded_image;
} shape_object_t;

void shape_init();
void draw_label(cv::Mat& out, int type, cv::Point position);
void check_shape(cv::Mat origin_image, shape_object_t* output);

#endif