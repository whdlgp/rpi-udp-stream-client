#ifndef GET_OPTICAL_FLOW_H
#define GET_OPTICAL_FLOW_H

#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <iostream>
#include <sstream>
#include <ctype.h>
#include <numeric>
#include <algorithm>

typedef struct opt_flow_t
{
    cv::Point2f output_point;
    cv::Mat masked_img;
    bool bad_condition;
} opt_flow_t;

void get_optical_flow(cv::Mat input_frame, opt_flow_t* out);

#endif
