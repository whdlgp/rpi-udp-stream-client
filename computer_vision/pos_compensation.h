#ifndef POS_COMPENSATION_H
#define POS_COMPENSATION_H

#include <iostream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

typedef struct drone_position_finding
{
    cv::Mat pointed_image;
    int pos_pix_x;
    int pos_pix_y;
} drone_position_finding;

cv::Point2i find_position_in_image(cv::Mat origin_image, double roll, double pitch, double altitude);

#endif
