#include "shape_object_recognition.h"

void draw_label(cv::Mat& out, int type, cv::Point position)
{
    int fontface = cv::FONT_HERSHEY_SIMPLEX;
	double scale = 0.4;
	int thickness = 1;

    std::cout << position << std::endl;

    std::string label;
    switch(type)
    {
        case SHAPE_TRI:
        label = "CIRCLE";
        break;

        case SHAPE_RECT:
        label = "RECT";
        break;

        case SHAPE_PENTA:
        label = "PENTA";
        break;

        case SHAPE_HEXA:
        label = "HEXA";
        break;

        case SHAPE_CIRCLE:
        label = "CIRCLE";
        break;
        
        case SHAPE_UNKNOWN:
        label = "UNKNOWN";
    }

    std::stringstream ss1;
	std::stringstream ss2;
	ss1 << position.x;
	ss2 << position.y;

	std::string Pos = ss1.str() + ',' + ss2.str();

    cv::putText(out, label+' '+Pos, position, fontface, scale, CV_RGB(0, 0, 0), thickness, 8);	
}

void check_shape(cv::Mat origin_image, shape_object_t* output)
{
    //find contour, detect shape and position
	std::vector<std::vector<cv::Point> > contours;
    std::vector<cv::Point> approx;
    cv::findContours(origin_image, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

    output->detected_num = contours.size();

    for (int i = 0; i < contours.size(); i++)
    {
        cv::approxPolyDP(cv::Mat(contours[i]), approx, cv::arcLength(cv::Mat(contours[i]), true)*0.03, true);

        if (std::fabs(cv::contourArea(contours[i])) < 100 || !cv::isContourConvex(approx))
        {
            output->detected_num--;
            continue;
        }

        //determin type(shape) of object
        if(approx.size() >= 3)
        {
            //find object's posistion with contour
            cv::Rect r = cv::boundingRect(approx);
            output->position.push_back(cv::Point(r.x + (r.width / 2), r.y + (r.height / 2)));

            switch(approx.size())
            {
                case 3: //triangle
                output->type.push_back(SHAPE_TRI);
                break;

                case 4: //rectangle
                output->type.push_back(SHAPE_RECT);
                break;

                case 5: //penta
                output->type.push_back(SHAPE_PENTA);
                break;

                case 6: //hexa
                output->type.push_back(SHAPE_HEXA);
                break;

                default: //circle
                double area = cv::contourArea(contours[i]);
                cv::Rect r = cv::boundingRect(contours[i]);
                int radius = r.width / 2;

                if (std::abs(1 - ((double)r.width / r.height)) <= 0.2 &&
                    std::abs(1 - (area / (CV_PI * (radius*radius)))) <= 0.2)
                    output->type.push_back(SHAPE_CIRCLE);
                else
                    output->type.push_back(SHAPE_UNKNOWN);
                break;
            }
        }
    }
}