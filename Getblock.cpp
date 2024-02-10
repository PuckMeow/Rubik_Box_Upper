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

	//������srcͼ�����RGB����
	std::vector<Mat> splitedBGR;
	splitedBGR.reserve(3);

	split(mSrc, splitedBGR);

	//�ֱ����R/G/Bͼ������ֵ��ֵ
	double meanR = 0, meanG = 0, meanB = 0;
	meanB = mean(splitedBGR[0])[0];
	meanG = mean(splitedBGR[1])[0];
	meanR = mean(splitedBGR[2])[0];

	//����R/G/Bͼ�������
	double gainR = 0, gainG = 0, gainB = 0;
	gainR = (meanR + meanG + meanB) / (3 * meanR);
	gainG = (meanR + meanG + meanB) / (3 * meanG);
	gainB = (meanR + meanG + meanB) / (3 * meanB);

	//���������R/G/Bͼ��
	splitedBGR[0] = splitedBGR[0] * gainB;
	splitedBGR[1] = splitedBGR[1] * gainG;
	splitedBGR[2] = splitedBGR[2] * gainR;

	//��������ͨ��ͼ��ϳ�һ����ͨ��ͼ��
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
	cvtColor(roi, image_gray, COLOR_BGR2GRAY);  //�ҶȻ�

	//equalizeHist(image_gray, image_gray);   	 	//��HSV������ͨ������ֱ��ͼ����
	//equalizeHist(image_gray, image_gray);  //ֱ��ͼ���⻯
	//GaussianBlur(image_gray, image_gray, Size(3, 3), 0, 0);
	//equalizeHist(image_gray, image_gray);  //ֱ��ͼ���⻯

	//////�Ҷ�ͼ����
	//Mat img_Thr, image_dst;
	////threshold(image_gray, img_Thr, 0, 255, THRESH_BINARY | THRESH_OTSU);    // ����Զ�Ѱ��ȫ����ֵ��

	//����Ӧ��ֵ�����ֵ��  ADAPTIVE_THRESH_MEAN_C   CV_ADAPTIVE_THRESH_GAUSSIAN_C
	Mat image_dst;
	adaptiveThreshold(image_gray, image_dst, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 25, 0);


	//Mat edge; Mat kernel; Mat edged; Mat edgee;//��Ե����
	//Canny(image_gray, edge, 10, 20, 3);

	Mat kernel; Mat edgee; Mat edge;
	kernel = getStructuringElement(MORPH_RECT, Size(3, 3));    //��һ��kernel�ں�
	//dilate(edge, edged, kernel);                               //����
	//erode(edged, edgee, kernel);                               //��ʴ
	//morphologyEx(edge, edgee, MORPH_CLOSE, kernel);
	//morphologyEx(edge, edgee, MORPH_CLOSE, kernel);

	//morphologyEx(image_dst, image_dst, MORPH_OPEN, kernel);//������,�Ͽ�ճ������
	morphologyEx(image_dst, image_dst, MORPH_CLOSE, kernel);//�ղ���,���϶Ͽ�����

	Mat  imageErode;
	erode(image_dst, imageErode, kernel);
	edge = image_dst - imageErode;
	morphologyEx(edge, edge, MORPH_CLOSE, kernel);//�ղ���,���϶Ͽ�����

	 //�������������
	vector<vector<Point>>contours;//����
	vector<Vec4i>hierachy;//��������ṹ����
	findContours(edge, contours, hierachy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	////��������

	vector<Moments> mu(contours.size());    //����������  
	for (int i = 0; i < contours.size(); i++)
	{
		mu[i] = moments(contours[i], true);
	}
	vector<Point2f>  mc(contours.size());    //������������
	for (int i = 0; i < contours.size(); i++)
	{
		mc[i] = Point2f(mu[i].m10 / mu[i].m00, mu[i].m01 / mu[i].m00);
	}

	vector<Rect> boundRect(contours.size()); //������Ӿ��μ���

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
			boundRect[i] = boundingRect((Mat)contours[i]); //����ÿ����������Ӿ���
			x0 = boundRect[i].x;  //��õ�i����Ӿ��ε����Ͻǵ�x����
			y0 = boundRect[i].y; //��õ�i����Ӿ��ε����Ͻǵ�y����
			w0 = boundRect[i].width; //��õ�i����Ӿ��εĿ��
			h0 = boundRect[i].height; //��õ�i����Ӿ��εĸ߶�
			rectangle(roi_o, Point(x0, y0), Point(x0 + w0, y0 + h0), Scalar(0, 255, 0), 2, 8); //���Ƶ�i����Ӿ���
		}
	}


	if (num < 9) ;//����

	//����
	sort(mct.begin(), mct.end(), compy);
	sort(mct.begin(), mct.begin() + 3, compx);
	sort(mct.begin() + 3, mct.begin() + 6, compx);
	sort(mct.end() - 3, mct.end(), compx);
	return mct;

}