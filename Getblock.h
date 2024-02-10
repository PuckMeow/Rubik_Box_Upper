#ifndef GETBLOCK_H  
#define GETBLOCK_H   

#include "Getcolour.h"

using namespace std;
using namespace cv;

vector<Point2f> Getblock(Mat roi, Mat roi_o);
int GrayWorld(InputArray src, OutputArray dst);
int PerfectReflect(InputArray src, OutputArray dst);

#endif
