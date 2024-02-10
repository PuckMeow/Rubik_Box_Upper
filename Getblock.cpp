#include "Getblock.h"
bool compx(Point2f a, Point2f b)
{
	return a.x < b.x;
}

bool compy(Point2f a, Point2f b)
{
	return a.y < b.y;
}


int GrayWorld(InputArray src, OutputArray dst)
{
	CV_Assert(src.channels() == 3, "GrayWorld() input image must be 3 channels!");

	Mat mSrc = src.getMat();
	if (mSrc.empty())
	{
		std::cout << "GrayWorld() input image is empty!" << std::endl;
		return -1;
	}

	dst.create(mSrc.size(), mSrc.type());
	Mat mDst = dst.getMat();

	if (mDst.empty())
	{
		std::cout << "GrayWorld() create dst image failed!" << std::endl;
		return -1;
	}

	//对输入src图像进行RGB分离
	std::vector<Mat> splitedBGR;
	splitedBGR.reserve(3);

	split(mSrc, splitedBGR);

	//分别计算R/G/B图像像素值均值
	double meanR = 0, meanG = 0, meanB = 0;
	meanB = mean(splitedBGR[0])[0];
	meanG = mean(splitedBGR[1])[0];
	meanR = mean(splitedBGR[2])[0];

	//计算R/G/B图像的增益
	double gainR = 0, gainG = 0, gainB = 0;
	gainR = (meanR + meanG + meanB) / (3 * meanR);
	gainG = (meanR + meanG + meanB) / (3 * meanG);
	gainB = (meanR + meanG + meanB) / (3 * meanB);

	//计算增益后R/G/B图像
	splitedBGR[0] = splitedBGR[0] * gainB;
	splitedBGR[1] = splitedBGR[1] * gainG;
	splitedBGR[2] = splitedBGR[2] * gainR;

	//将三个单通道图像合成一个三通道图像
	merge(splitedBGR, mDst);

	return 0;
}



int PerfectReflect(InputArray src, OutputArray dst)
{
	CV_Assert_2(src.channels() == 3, "PerfectReflect() src image must has 3 channels!");

	Mat mSrc = src.getMat();
	if (mSrc.empty())
	{
		std::cout << "PerfectReflect() src image can't be empty!" << std::endl;
		return -1;
	}

	dst.create(mSrc.size(), mSrc.type());
	Mat mDst = dst.getMat();

	int sumHist[766] = { 0 };//max(R+G+B) = 255*3 = 765, 0~765->766
	int maxVal = 0;

	for (int i = 0; i < mSrc.rows; i++)
	{
		for (int j = 0; j < mSrc.cols; j++)
		{
			Vec3b p = mSrc.at<Vec3b>(i, j);
			int sum = p[0] + p[1] + p[2];
			sumHist[sum]++;
			maxVal = maxVal > p[0] ? maxVal : p[0];
			maxVal = maxVal > p[1] ? maxVal : p[1];
			maxVal = maxVal > p[2] ? maxVal : p[2];
		}
	}

	int totalPixels = 0;
	for (int i = 765; i >= 0; i--)
	{
		totalPixels += sumHist[i];
	}

	CV_Assert_2(totalPixels == mSrc.rows * mSrc.cols, "sumHist pixels number isn't equal with image size!");

	float ratio = 0.1;
	int cumPixel = 0;
	int threshold = 0;
	for (int i = 765; i >= 0; i--)
	{
		cumPixel += sumHist[i];
		if (cumPixel >= ratio * mSrc.rows * mSrc.cols)
		{
			threshold = i;
			break;
		}
	}

	int avgB = 0, avgG = 0, avgR = 0;
	int countPixels = 0;
	for (int i = 0; i < mSrc.rows; i++)
	{
		for (int j = 0; j < mSrc.cols; j++)
		{
			Vec3b p = mSrc.at<Vec3b>(i, j);
			int sum = p[0] + p[1] + p[2];
			if (sum > threshold)
			{
				countPixels++;
				avgB += p[0];
				avgG += p[1];
				avgR += p[2];
			}
		}
	}

	avgB /= countPixels;
	avgG /= countPixels;
	avgR /= countPixels;

	for (int i = 0; i < mSrc.rows; i++)
	{
		for (int j = 0; j < mSrc.cols; j++)
		{
			Vec3b p = mSrc.at<Vec3b>(i, j);
			int B = p[0] * maxVal / avgB;
			B = B > 255 ? 255 : B;
			mDst.at<Vec3b>(i, j)[0] = (uchar)B;

			int G = p[1] * maxVal / avgG;
			G = G > 255 ? 255 : G;
			mDst.at<Vec3b>(i, j)[1] = (uchar)G;

			int R = p[2] * maxVal / avgR;
			R = R > 255 ? 255 : R;
			mDst.at<Vec3b>(i, j)[2] = (uchar)R;
		}
	}

	return 0;
}


vector<Point2f> Getblock(Mat roi,Mat roi_o)
{
	Mat image_gray;
	cvtColor(roi, image_gray, COLOR_BGR2GRAY);  //灰度化

	//equalizeHist(image_gray, image_gray);   	 	//对HSV的亮度通道进行直方图均衡
	//equalizeHist(image_gray, image_gray);  //直方图均衡化
	//GaussianBlur(image_gray, image_gray, Size(3, 3), 0, 0);
	//equalizeHist(image_gray, image_gray);  //直方图均衡化

	//////灰度图像大津法
	//Mat img_Thr, image_dst;
	////threshold(image_gray, img_Thr, 0, 255, THRESH_BINARY | THRESH_OTSU);    // 大津法自动寻求全局阈值。

	//自适应阈值处理二值化  ADAPTIVE_THRESH_MEAN_C   CV_ADAPTIVE_THRESH_GAUSSIAN_C
	Mat image_dst;
	adaptiveThreshold(image_gray, image_dst, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 25, 0);


	//Mat edge; Mat kernel; Mat edged; Mat edgee;//边缘计算
	//Canny(image_gray, edge, 10, 20, 3);

	Mat kernel; Mat edgee; Mat edge;
	kernel = getStructuringElement(MORPH_RECT, Size(3, 3));    //做一个kernel内核
	//dilate(edge, edged, kernel);                               //膨胀
	//erode(edged, edgee, kernel);                               //侵蚀
	//morphologyEx(edge, edgee, MORPH_CLOSE, kernel);
	//morphologyEx(edge, edgee, MORPH_CLOSE, kernel);

	//morphologyEx(image_dst, image_dst, MORPH_OPEN, kernel);//开操作,断开粘连部分
	morphologyEx(image_dst, image_dst, MORPH_CLOSE, kernel);//闭操作,补上断开部分

	Mat  imageErode;
	erode(image_dst, imageErode, kernel);
	edge = image_dst - imageErode;
	morphologyEx(edge, edge, MORPH_CLOSE, kernel);//闭操作,补上断开部分

	 //轮廓发现与绘制
	vector<vector<Point>>contours;//轮廓
	vector<Vec4i>hierachy;//存放轮廓结构变量
	findContours(edge, contours, hierachy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	////绘制轮廓

	vector<Moments> mu(contours.size());    //计算轮廓矩  
	for (int i = 0; i < contours.size(); i++)
	{
		mu[i] = moments(contours[i], true);
	}
	vector<Point2f>  mc(contours.size());    //计算轮廓中心
	for (int i = 0; i < contours.size(); i++)
	{
		mc[i] = Point2f(mu[i].m10 / mu[i].m00, mu[i].m01 / mu[i].m00);
	}

	vector<Rect> boundRect(contours.size()); //定义外接矩形集合

	vector<Point2f>  mct(9);

	int x0 = 0, y0 = 0, w0 = 0, h0 = 0; int num = 0;
	for (int i = 0, j = 0; i < contours.size(); i++)
	{
		int areasValue = contourArea(contours[i], false);
		if (areasValue <= 6000 && areasValue >= 4000)
		{
			//drawContours(roi, contours, i, Scalar(0, 0, 255), 2, 8);
			mct[j].x = mc[i].x; mct[j].y = mc[i].y;
			j = j + 1; num = j;
			boundRect[i] = boundingRect((Mat)contours[i]); //查找每个轮廓的外接矩形
			x0 = boundRect[i].x;  //获得第i个外接矩形的左上角的x坐标
			y0 = boundRect[i].y; //获得第i个外接矩形的左上角的y坐标
			w0 = boundRect[i].width; //获得第i个外接矩形的宽度
			h0 = boundRect[i].height; //获得第i个外接矩形的高度
			rectangle(roi_o, Point(x0, y0), Point(x0 + w0, y0 + h0), Scalar(0, 255, 0), 2, 8); //绘制第i个外接矩形
		}
	}


	if (num < 9) ;//报错

	//排序
	sort(mct.begin(), mct.end(), compy);
	sort(mct.begin(), mct.begin() + 3, compx);
	sort(mct.begin() + 3, mct.begin() + 6, compx);
	sort(mct.end() - 3, mct.end(), compx);
	return mct;

}