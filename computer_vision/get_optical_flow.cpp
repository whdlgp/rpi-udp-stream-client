#include "get_optical_flow.h"

using namespace cv;
using namespace std;

#define MAX_POINT_NUM 500
#define POINT_INTERVAL 20

static int is_init = 1;
static vector<Point2f> track_point;
static vector<Point2f> found_point;
static vector<float> disp_x;
static vector<float> disp_y;

static Mat gray;
static Mat pre_gray;
static Mat mask;

void get_optical_flow(cv::Mat input_frame, opt_flow_t* out)
{
    input_frame.copyTo(out->masked_img);
    cvtColor(input_frame, gray, CV_BGR2GRAY);

    if(is_init)
    {
        for(int y = POINT_INTERVAL-1; y < gray.rows; y += POINT_INTERVAL)
        {
            for(int x = POINT_INTERVAL-1; x < gray.cols; x += POINT_INTERVAL)
            {
                track_point.push_back(Point(x, y));
            }
        }             
        is_init = 0;
    }
    else
    {
        vector<uchar> status;
        vector<float> err;

        if(pre_gray.empty())
            gray.copyTo(pre_gray);

        //find optical flow point
        calcOpticalFlowPyrLK(pre_gray
                , gray
                , track_point
                , found_point
                , status
                , err
                , Size(40, 40)
                , 3
                , TermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS, 20, 0.03)
                , 0
                , 0.001);

        vector<Point2f> found = found_point;
        vector<Point2f> track = track_point;
        Point2f found_sum(0, 0);
        Point2f found_mean(0, 0);
        disp_x.clear();
        disp_y.clear();

        int valid_count = 0;
        for(int j = 0; j < status.size(); j++)
        {
            //find valid point and check it's circle point symmetry
            //if(status[j] && status[status.size()-1-j])
            if(status[j])
            {
                //calculate displacement
                disp_x.push_back(found[j].x - track[j].x);
                disp_y.push_back(found[j].y - track[j].y);

                line(out->masked_img
                        , track_point[j]
                        , Point(track[j].x + disp_x[valid_count]
                            , track[j].y + disp_y[valid_count])
                        , Scalar(255, 255, 0));

                valid_count++;
            }
        }
        if(!valid_count)
        {
            //bad condition
            found_mean.x = 0;
            found_mean.y = 0;

            out->bad_condition = true;
        }
        else
        {
            //trimmed meaning 
            //cut first and last 10 percent of elements
            int percent_10 = valid_count / 10;
            sort(disp_x.begin(), disp_x.end());
            sort(disp_y.begin(), disp_y.end());

            //trim
            disp_x.erase(disp_x.begin(), disp_x.begin() + percent_10);
            disp_x.erase(disp_x.end() - percent_10, disp_x.end());
            disp_y.erase(disp_y.begin(), disp_y.begin() + percent_10);
            disp_y.erase(disp_y.end() - percent_10, disp_y.end());

            //calculate mean
            found_mean.x = std::accumulate(disp_x.begin(), disp_x.end(), 0.0f)/disp_x.size();
            found_mean.y = std::accumulate(disp_y.begin(), disp_y.end(), 0.0f)/disp_y.size();

            out->bad_condition = false;
        }

        line(out->masked_img
                , Point(gray.cols/2, gray.rows/2)
                , Point(gray.cols/2 + found_mean.x*2
                    , gray.rows/2 + found_mean.y*2)
                , Scalar(0, 0, 255));
        
        out->output_point.x = found_mean.x;
        out->output_point.y = found_mean.y;

        swap(pre_gray, gray);
    }
}
