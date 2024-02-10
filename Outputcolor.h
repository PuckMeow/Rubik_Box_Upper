#ifndef OUTPUTCOLOR_H  
#define OUTPUTCOLOR_H   

#include "Getcolour.h"

using namespace std;
using namespace cv;

vector<char> Outputcolor(vector<Point2f> mct, Mat roi, Mat roi_o, cv::Ptr<cv::ml::KNearest> knn);


#endif