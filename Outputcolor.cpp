#include"Outputcolor.h"


String Outputcolor(vector<Point2f> mct, Mat roi, Mat roi_o, cv::Ptr<cv::ml::KNearest> knn) {
	String Output;
	for (int i = 0; i < mct.size(); i++) {
		//Rect rectL(-20 + mct[i].x, -20 + mct[i].y, 40, 40);
		//Mat imageblock = roi1(rectL);
		Mat imageblock = roi(Range(-20 + mct[i].y, 20 + mct[i].y), Range(-20 + mct[i].x, 20 + mct[i].x));
		//bgorwy
		char ret;
		ret = Getcolour(imageblock, knn);
		Scalar color;
		string colour;
		if (ret == 'b') colour = "蓝色";
		else if (ret == 'g') colour = "绿色";
		else if (ret == 'o') colour = "橙色";
		else if (ret == 'r') colour = "红色";
		else if (ret == 'w') colour = "白色";
		else if (ret == 'y') colour = "黄色";

		if (ret == 'b') color = Scalar(255, 0, 0);
		else if (ret == 'g') color = Scalar(0, 255, 0);
		else if (ret == 'o') color = Scalar(0, 69, 255);
		else if (ret == 'r') color = Scalar(0, 0, 255);
		else if (ret == 'w') color = Scalar(255, 255, 255);
		else if (ret == 'y') color = Scalar(0, 255, 255);

		Output.push_back(ret);

		circle(roi_o, mct[i], 15, color, -1, 6, 0);      //画中心圆
		cout << "图片颜色是——" << colour << '\n' << endl;
	}


	return Output;


}