#include "Getcolour.h"

char Getcolour(Mat image, cv::Ptr<cv::ml::KNearest> knn)
{
	resize(image, image, Size(40, 40));
	Mat img_show = image.clone();
	cvtColor(image, image, COLOR_BGR2HLS);
	vector<Mat> channels;
	//    通道分离

	image.convertTo(image, CV_32F);
	split(image, channels);

	Mat H, L, S;
	H = channels.at(0);
	L = channels.at(1);
	S = channels.at(2);
	//求方差以分辨红色
	cv::Scalar  mean,dev;
	cv::meanStdDev(H, mean, dev);
	float Dev = dev.val[0];

	Mat OneMat;
	//更换数据类型有uchar->float32
	//OneMat.convertTo(OneMat, CV_32F);
	//分配权重,权重相同时应有 Hw = 255 / 180, Lw = 1, Sw = 1;
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
	
	//归一化
	OneMat = OneMat/255.0;
	char ret;
	//预测图片
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

		//归一化
		OneMat = OneMat / 255.0;
		
	}
	ret = (char)knn->predict(OneMat);
	//if (Dev > 10 && ret != 'w') ret = 'r';

	//cout << ret << endl;
	//string colour;
	//if (ret == 'b') colour = "蓝色";
	//else if (ret == 'g') colour = "绿色";
	//else if (ret == 'o') colour = "橙色";
	//else if (ret == 'r') colour = "红色";
	//else if (ret == 'w') colour = "白色";
	//else if (ret == 'y') colour = "黄色";
	//bgorwy
	return ret;
}