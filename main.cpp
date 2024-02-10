#define _CRT_SECURE_NO_WARNINGS 1
#include <stdio.h>
#include "Getcolour.h"
#include "Getblock.h"
#include "Outputcolor.h"
#include <opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;


int main() {
	Mat image; Mat image_gray;
	image = imread("44.png");//读取图像
	//GaussianBlur(image, image, Size(21, 21), 0, 0);
	//GrayWorld(image, image);
	//做白平衡
	PerfectReflect(image, image);

	int x = image.rows;  int y = image.cols;
	Mat roi_o = image(Range(0.65 * x / 4, 3.05 * x / 4), Range(y / 4, 2.85 * y / 4));
	Mat roi = roi_o.clone();

	Mat imgHSV;
	vector<Mat> hsvSplit;   						//创建向量容器，存放HSV的三通道数据
	cvtColor(roi, imgHSV, COLOR_BGR2HSV); 	//Convert the captured frame from BGR to HSV
	split(imgHSV, hsvSplit);						//分类原图像的HSV三通道
	equalizeHist(hsvSplit[2], hsvSplit[2]);   	 	//对HSV的亮度通道进行直方图均衡
	merge(hsvSplit, imgHSV);				   		//合并三种通道
	cvtColor(imgHSV, roi, COLOR_HSV2BGR);    	//将HSV空间转回至RGB空间，为接下来的颜色识别做准备

	Mat roi1 = roi.clone();

	//GaussianBlur(roi1, roi1, Size(21, 21), 0, 0);
	
	//绘制轮廓方框并获取中心点
	vector<Point2f> mct = Getblock(roi, roi_o);

	//加载knn模型
	cv::Ptr<cv::ml::KNearest> knn = cv::ml::StatModel::load<cv::ml::KNearest>("C://Users//31746//Desktop//课程//机电系统试验//KNN//knn.xml");

	//识别颜色并在图上绘制，给出颜色数组，按照从左到右，从上到下的顺序
	Outputcolor(mct, roi1, roi_o, knn);

	imshow("1",roi_o);
	waitKey(0);
	return 0;
}

