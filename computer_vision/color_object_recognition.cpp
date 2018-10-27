#include "color_object_recognition.h"

using namespace cv;

typedef struct hsv_t
{
    int low_h;
    int high_h;

    int low_s;
    int high_s;

    int low_v;
    int high_v;
} hsv_t;

static hsv_t red = {30, 145, 100, 255, 20, 255}
            , blue = {90, 145, 100, 255, 20, 255}
            , green = {30, 90, 100, 255, 20, 255};

void color_object_init()
{
    //red
    cvNamedWindow("red", 1 );
    cvCreateTrackbar("LowH", "red", &red.low_h, 179); //Hue (0 - 179)
    cvCreateTrackbar("HighH", "red", &red.high_h, 179);

    cvCreateTrackbar("LowS","red", &red.low_s, 255); //Saturation (0 - 255)
    cvCreateTrackbar("HighS", "red", &red.high_s, 255);

    cvCreateTrackbar("LowV", "red", &red.low_v, 255); //Value (0 - 255)
    cvCreateTrackbar("HighV", "red", &red.high_v, 255);

    //blue
    cvNamedWindow("blue", 1 );
    cvCreateTrackbar("LowH", "blue", &blue.low_h, 179); //Hue (0 - 179)
    cvCreateTrackbar("HighH", "blue", &blue.high_h, 179);

    cvCreateTrackbar("LowS","blue", &blue.low_s, 255); //Saturation (0 - 255)
    cvCreateTrackbar("HighS", "blue", &blue.high_s, 255);

    cvCreateTrackbar("LowV", "blue", &blue.low_v, 255); //Value (0 - 255)
    cvCreateTrackbar("HighV", "blue", &blue.high_v, 255);

    //green
    cvNamedWindow("green", 1 );
    cvCreateTrackbar("LowH", "green", &green.low_h, 179); //Hue (0 - 179)
    cvCreateTrackbar("HighH", "green", &green.high_h, 179);

    cvCreateTrackbar("LowS","green", &green.low_s, 255); //Saturation (0 - 255)
    cvCreateTrackbar("HighS", "green", &green.high_s, 255);

    cvCreateTrackbar("LowV", "green", &green.low_v, 255); //Value (0 - 255)
    cvCreateTrackbar("HighV", "green", &green.high_v, 255);
}

void find_red_object(cv::Mat origin_image, color_object_t* output)
{
    Mat hsv_image;

    /*
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
    */
    //red 이진화
    cvtColor(origin_image, hsv_image, COLOR_BGR2HSV);

    Mat gray1, gray2;
    inRange(hsv_image, Scalar(0, red.low_s, red.low_v), Scalar(red.low_h, red.high_s, red.high_v), gray1);
    inRange(hsv_image, Scalar(red.high_h, red.low_s, red.low_v), Scalar(179, red.high_s, red.high_v), gray2);
    output->thresholded_image = gray1 | gray2;

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

    inRange(hsv_image, Scalar(green.low_h, green.low_s, green.low_v)
            , Scalar(green.high_h, green.high_s, green.high_v)
            , output->thresholded_image);

    //morphological opening (removes small objects from the foreground)
    erode(output->thresholded_image, output->thresholded_image, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
    dilate(output->thresholded_image, output->thresholded_image, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) ); 

    //morphological closing (removes small holes from the foreground)
    dilate(output->thresholded_image, output->thresholded_image, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) ); 
    erode(output->thresholded_image, output->thresholded_image, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );\

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

    inRange(hsv_image, Scalar(blue.low_h, blue.low_s, blue.low_v)
            , Scalar(blue.high_h, blue.high_s, blue.high_v)
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
