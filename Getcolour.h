#ifndef GETCOLOUR_H  
#define GETCOLOUR_H   

#include<iostream>
#include<opencv.hpp>
#include<string>


using namespace std;
using namespace cv;

char Getcolour(Mat image, cv::Ptr<cv::ml::KNearest> knn);

#endif
