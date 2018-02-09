#include "color_object_recognition.h"

using namespace cv;

void find_red_object(cv::Mat origin_image, color_object_t* output)
{
    Mat hsv_image;

    //in case of red, HSV range is 0~10 and 170~180
    //because of this inconvenient range, inverse rgb map
    //and find almost middle of range 80~100, 
    Mat image_tmp;

    image_tmp = ~origin_image;
    int hsv_red_h_inverse = HSV_RED_H + 90;
    //convert to HSV colormap
    cvtColor(image_tmp, hsv_image, COLOR_BGR2HSV);

    inRange(hsv_image
            , Scalar(hsv_red_h_inverse-15, HSV_RED_S-10, HSV_RED_V-10)
            , Scalar(hsv_red_h_inverse+15, 255, 255)
            , output->thresholded_image);

    //morphological opening (removes small objects from the foreground)
    erode(output->thresholded_image, output->thresholded_image, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
    dilate(output->thresholded_image, output->thresholded_image, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) ); 

    //morphological closing (removes small holes from the foreground)
    dilate(output->thresholded_image, output->thresholded_image, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) ); 
    erode(output->thresholded_image, output->thresholded_image, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );

    //Calculate the moments of the thresholded image
    Moments oMoments = moments(output->thresholded_image);
    double dM01 = oMoments.m01;
    double dM10 = oMoments.m10;
    double dArea = oMoments.m00;

    // if the area <= 10000, I consider that the there are no object in the image and it's because of the noise, the area is not zero 
    if (dArea > 10000)
    {
        //calculate the position of the ball
        output->pos_x = dM10 / dArea;
        output->pos_y = dM01 / dArea;
        output->is_recognized = 1;
    }
    else
    {
        output->pos_x = 0;
        output->pos_y = 0;
        output->is_recognized = 0;
    }
}

void find_green_object(cv::Mat origin_image, color_object_t* output)
{
    Mat hsv_image;

    Mat image_tmp;

    image_tmp = origin_image;
    //convert to HSV colormap
    cvtColor(image_tmp, hsv_image, COLOR_BGR2HSV);

    inRange(hsv_image
            , Scalar(HSV_GREEN_H-15, HSV_GREEN_S-10, HSV_GREEN_V-10)
            , Scalar(HSV_GREEN_H+15, 255, 255)
            , output->thresholded_image);

    //morphological opening (removes small objects from the foreground)
    erode(output->thresholded_image, output->thresholded_image, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
    dilate(output->thresholded_image, output->thresholded_image, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) ); 

    //morphological closing (removes small holes from the foreground)
    dilate(output->thresholded_image, output->thresholded_image, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) ); 
    erode(output->thresholded_image, output->thresholded_image, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );

    //Calculate the moments of the thresholded image
    Moments oMoments = moments(output->thresholded_image);
    double dM01 = oMoments.m01;
    double dM10 = oMoments.m10;
    double dArea = oMoments.m00;

    // if the area <= 10000, I consider that the there are no object in the image and it's because of the noise, the area is not zero 
    if (dArea > 10000)
    {
        //calculate the position of the ball
        output->pos_x = dM10 / dArea;
        output->pos_y = dM01 / dArea;
        output->is_recognized = 1;
    }
    else
    {
        output->pos_x = 0;
        output->pos_y = 0;
        output->is_recognized = 0;
    }
}

void find_blue_object(cv::Mat origin_image, color_object_t* output)
{
    Mat hsv_image;

    Mat image_tmp;

    image_tmp = origin_image;
    //convert to HSV colormap
    cvtColor(image_tmp, hsv_image, COLOR_BGR2HSV);

    inRange(hsv_image
            , Scalar(HSV_BLUE_H-15, HSV_BLUE_S-10, HSV_BLUE_V-10)
            , Scalar(HSV_BLUE_H+15, 255, 255)
            , output->thresholded_image);

    //morphological opening (removes small objects from the foreground)
    erode(output->thresholded_image, output->thresholded_image, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
    dilate(output->thresholded_image, output->thresholded_image, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) ); 

    //morphological closing (removes small holes from the foreground)
    dilate(output->thresholded_image, output->thresholded_image, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) ); 
    erode(output->thresholded_image, output->thresholded_image, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );

    //Calculate the moments of the thresholded image
    Moments oMoments = moments(output->thresholded_image);
    double dM01 = oMoments.m01;
    double dM10 = oMoments.m10;
    double dArea = oMoments.m00;

    // if the area <= 10000, I consider that the there are no object in the image and it's because of the noise, the area is not zero 
    if (dArea > 10000)
    {
        //calculate the position of the ball
        output->pos_x = dM10 / dArea;
        output->pos_y = dM01 / dArea;
        output->is_recognized = 1;
    }
    else
    {
        output->pos_x = 0;
        output->pos_y = 0;
        output->is_recognized = 0;
    }
}
