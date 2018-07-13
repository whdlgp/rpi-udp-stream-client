#include "pos_compensation.h"
#include <math.h>

using namespace cv;

// origin_image : input 
// 
cv::Point2i find_position_in_image(cv::Mat origin_image, double roll, double pitch, double altitude)
{
	static int x, y;
	int height = origin_image.rows;
	int width = origin_image.cols;

	roll = 0.00175 * roll;
	pitch = 0.00175 * pitch;

	x = 159 + (int) (265.23 * tan(roll));//(x+1) % width;
	y = 119 - (int) (264.53 * tan(pitch));//(y+1) % height;

	//satuation
	if(x>319) x=319;
	else if(x<0) x=0;
	if(y>239) y=239;
	else if(y<0) y=0;

	return Point2i(x, y);
}
