#include "Getcolour.h"

char Getcolour(Mat image, cv::Ptr<cv::ml::KNearest> knn)
{
	resize(image, image, Size(40, 40));
	Mat img_show = image.clone();
	cvtColor(image, image, COLOR_BGR2HLS);
	vector<Mat> channels;
	//    ͨ������

	image.convertTo(image, CV_32F);
	split(image, channels);

	Mat H, L, S;
	H = channels.at(0);
	L = channels.at(1);
	S = channels.at(2);
	//�󷽲��Էֱ��ɫ
	cv::Scalar  mean,dev;
	cv::meanStdDev(H, mean, dev);
	float Dev = dev.val[0];

	Mat OneMat;
	//��������������uchar->float32
	//OneMat.convertTo(OneMat, CV_32F);
	//����Ȩ��,Ȩ����ͬʱӦ�� Hw = 255 / 180, Lw = 1, Sw = 1;
	float Hw = 255.0 / 180 , Lw = 1.15, Sw = 0.3;
	//float Hv = sum(H)[0] / (H.cols * H.rows);
	//float Lv = sum(L)[0] / (L.cols * L.rows);
	//float Sv = sum(S)[0] / (S.cols * S.rows);
	//OneMat = (cv::Mat_<double>(1, 3) << Hv * Hw, Lv * Lw, Sv * Sw);
	//OneMat = Mat(1, 1200, CV_8U, Scalar(0));
	OneMat = H.reshape(1, 0) * Hw;
	OneMat.push_back(L.reshape(1, 0) * Lw);
	OneMat.push_back(S.reshape(1, 0) * Sw);
	//Mat D = Mat(1, H.rows, CV_8U, Scalar(dev.val[0]));
	//OneMat.push_back(D);
	OneMat = OneMat.reshape(0, 1);
	
	//��һ��
	OneMat = OneMat/255.0;
	char ret;
	//Ԥ��ͼƬ
	ret = (char)knn->predict(OneMat);

	if (ret != 'r' && Dev > 10) {
		Mat H1 = H.reshape(0, 1);
		for (int k = 0; k < H1.cols; k++) {
			int t = H1.at<float>(0, k);
			t = 180 - t;
			if (t < 90) H1.at<float>(0, k) = t;
		}

		OneMat = H1 * Hw;
		OneMat.push_back(L.reshape(0, 1) * Lw);
		OneMat.push_back(S.reshape(0, 1) * Sw);
		//Mat D = Mat(1, H.rows, CV_8U, Scalar(dev.val[0]));
		//OneMat.push_back(D);
		OneMat = OneMat.reshape(0, 1);

		//��һ��
		OneMat = OneMat / 255.0;
		
	}
	ret = (char)knn->predict(OneMat);
	//if (Dev > 10 && ret != 'w') ret = 'r';

	//cout << ret << endl;
	//string colour;
	//if (ret == 'b') colour = "��ɫ";
	//else if (ret == 'g') colour = "��ɫ";
	//else if (ret == 'o') colour = "��ɫ";
	//else if (ret == 'r') colour = "��ɫ";
	//else if (ret == 'w') colour = "��ɫ";
	//else if (ret == 'y') colour = "��ɫ";
	//bgorwy
	return ret;
}