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
	image = imread("44.png");//��ȡͼ��
	//GaussianBlur(image, image, Size(21, 21), 0, 0);
	//GrayWorld(image, image);
	//����ƽ��
	PerfectReflect(image, image);

	int x = image.rows;  int y = image.cols;
	Mat roi_o = image(Range(0.65 * x / 4, 3.05 * x / 4), Range(y / 4, 2.85 * y / 4));
	Mat roi = roi_o.clone();

	Mat imgHSV;
	vector<Mat> hsvSplit;   						//�����������������HSV����ͨ������
	cvtColor(roi, imgHSV, COLOR_BGR2HSV); 	//Convert the captured frame from BGR to HSV
	split(imgHSV, hsvSplit);						//����ԭͼ���HSV��ͨ��
	equalizeHist(hsvSplit[2], hsvSplit[2]);   	 	//��HSV������ͨ������ֱ��ͼ����
	merge(hsvSplit, imgHSV);				   		//�ϲ�����ͨ��
	cvtColor(imgHSV, roi, COLOR_HSV2BGR);    	//��HSV�ռ�ת����RGB�ռ䣬Ϊ����������ɫʶ����׼��

	Mat roi1 = roi.clone();

	//GaussianBlur(roi1, roi1, Size(21, 21), 0, 0);
	
	//�����������򲢻�ȡ���ĵ�
	vector<Point2f> mct = Getblock(roi, roi_o);

	//����knnģ��
	cv::Ptr<cv::ml::KNearest> knn = cv::ml::StatModel::load<cv::ml::KNearest>("C://Users//31746//Desktop//�γ�//����ϵͳ����//KNN//knn.xml");

	//ʶ����ɫ����ͼ�ϻ��ƣ�������ɫ���飬���մ����ң����ϵ��µ�˳��
	Outputcolor(mct, roi1, roi_o, knn);

	imshow("1",roi_o);
	waitKey(0);
	return 0;
}

