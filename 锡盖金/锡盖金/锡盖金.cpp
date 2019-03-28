// 锡盖金.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include<iostream>
#include<opencv2/opencv.hpp>
//#include"preprocess.h"
cv::Mat image;
cv::Mat img;
cv::Mat tmp;
void on_mouse(int event, int x, int y, int flags, void *ustc)//event鼠标事件代号，x,y鼠标坐标，flags拖拽和键盘操作的代号
{
	static cv::Point pre_pt(-1, -1);//初始坐标
	static cv::Point cur_pt(-1, -1);//实时坐标
	char temp[16];
	if (event == CV_EVENT_LBUTTONDOWN)//左键按下，读取初始坐标，并在图像上该点处划圆
	{
		image.copyTo(img);//将原始图片复制到img中
		sprintf(temp, "(%d,%d)", x, y);
		pre_pt = cv::Point(x, y);
		putText(img, temp, pre_pt, cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0, 255), 1, 8);//在窗口上显示坐标
		cv::circle(img, pre_pt, 2, cv::Scalar(255, 0, 0, 0), CV_FILLED, CV_AA, 0);//划圆
		cv::imshow("img", img);
	}
	else if (event == CV_EVENT_MOUSEMOVE && !(flags & CV_EVENT_FLAG_LBUTTON))//左键没有按下的情况下鼠标移动的处理函数
	{
		img.copyTo(tmp);//将img复制到临时图像tmp上，用于显示实时坐标
		sprintf(temp, "(%d,%d)", x, y);
		cur_pt = cv::Point(x, y);
		putText(tmp, temp, cur_pt, cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0, 255));//只是实时显示鼠标移动的坐标
		cv::imshow("img", tmp);
	}
	else if (event == CV_EVENT_MOUSEMOVE && (flags & CV_EVENT_FLAG_LBUTTON))//左键按下时，鼠标移动，则在图像上划矩形
	{
		img.copyTo(tmp);
		sprintf(temp, "(%d,%d)", x, y);
		cur_pt = cv::Point(x, y);
		putText(tmp, temp, cur_pt, cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0, 255));
		rectangle(tmp, pre_pt, cur_pt, cv::Scalar(0, 255, 0, 0), 1, 8, 0);//在临时图像上实时显示鼠标拖动时形成的矩形
		imshow("img", tmp);
	}
	else if (event == CV_EVENT_LBUTTONUP)//左键松开，将在图像上划矩形
	{
		image.copyTo(img);
		sprintf(temp, "(%d,%d)", x, y);
		cur_pt = cv::Point(x, y);
		putText(img, temp, cur_pt, cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0, 255));
		circle(img, pre_pt, 2, cv::Scalar(255, 0, 0, 0), CV_FILLED, CV_AA, 0);
		rectangle(img, pre_pt, cur_pt, cv::Scalar(0, 255, 0, 0), 1, 8, 0);//根据初始点和结束点，将矩形画到img上
		cv::imshow("img", img);
		img.copyTo(tmp);
		//截取矩形包围的图像，并保存到dst中
		int width = abs(pre_pt.x - cur_pt.x);
		int height = abs(pre_pt.y - cur_pt.y);
		if (width == 0 || height == 0)
		{
			printf("width == 0 || height == 0");
			return;
		}
		cv::Mat dst = image(cv::Rect(std::min(cur_pt.x, pre_pt.x), std::min(cur_pt.y, pre_pt.y), width, height));
		cv::namedWindow("dst");
		cv::imshow("dst", dst);
		cv::imwrite("C:\\Users\\dyf01\\Desktop\\锡盖金\\锡盖金\\model.bmp", dst);
		cv::waitKey(0);
	}
}

void meanColor(cv::Mat img, cv::Mat dst, int cols, int rows,cv::Point &matchLoc)
{
	cv::Point matchLoc1;
	double minL = 255;
	double minR = 255;
	double minU = 255;
	double minD = 255;
	double min = 255;
	cv::Point matchL;
	cv::Point matchR;
	cv::Point matchU;
	cv::Point matchD;
	cv::Mat mean;
	cv::Mat stdDev;
	cv::meanStdDev(dst, mean, stdDev);
	matchD = matchLoc;
	matchU = matchLoc;
	matchL = matchLoc;
	matchR = matchLoc;
	std::cout << "matchLoc" << matchLoc.x << "," << matchLoc.y << std::endl;
	double mean_dst = mean.ptr<double>(0)[0];
	int  flag = 0;
	while (1) {
	//	matchD = matchLoc;
		cv::Mat src = cv::Mat(img, cv::Rect(matchD, cv::Size(cols, rows)));
		cv::cvtColor(src, src, cv::COLOR_BGR2GRAY);
		cv::threshold(src, src, 30, 255, cv::THRESH_BINARY);
		cv::meanStdDev(src, mean, stdDev);
		double mean_src = mean.ptr<double>(0)[0];
		if (flag == 0) {
			if (std::abs(mean_src - mean_dst) < minD) {
				minD = std::abs(mean_src - mean_dst);
				if(matchD.y + 5<=2048)
					matchD.y = matchD.y + 5;
			}
			else
			{
				matchD.y = matchD.y - 5;
				flag = 1;
			}
		}
		else  {
				if (std::abs(mean_src - mean_dst) < minD) {
					minD = std::abs(mean_src - mean_dst);
					if (matchD.y + 1<=2048)
						matchD.y = matchD.y + 1;
				}
				else
				{
					matchD.y = matchD.y -1;
					if (minD < min)
					{
						min = minD;
						std::cout << min << std::endl;

						matchLoc1 = matchD;
						std::cout <<"matchD:"<<matchLoc1.x << "," << matchLoc1.y << std::endl;	
						
					}
					break;
				}
		}
	}
	flag = 0;
	while (1) {
		//matchU = matchLoc;
		cv::Mat src = cv::Mat(img, cv::Rect(matchU, cv::Size(cols, rows)));
		cv::cvtColor(src, src, cv::COLOR_BGR2GRAY);
		cv::threshold(src, src,30, 255, cv::THRESH_BINARY);
		cv::meanStdDev(src, mean, stdDev);
		double mean_src = mean.ptr<double>(0)[0];
		if (flag == 0) {
			if (std::abs(mean_src - mean_dst) < minU) {
				minU = std::abs(mean_src - mean_dst);
				if (matchU.y - 5>=0)
					matchU.y = matchU.y - 5;
			}
			else
			{
				matchU.y = matchU.y + 5;
				flag = 1;
			}
		}
		else {
			if (std::abs(mean_src - mean_dst) < minU) {
				minU = std::abs(mean_src - mean_dst);
				if (matchU.y - 1 >= 0)
					matchU.y = matchU.y - 1;
			}
			else
			{
				matchU.y = matchU.y + 1;
				if (minU < min)
				{
					min = minU;
					std::cout << min << std::endl;

					matchLoc1 = matchU;
					std::cout <<"matchU:"<< matchLoc1.x << "," << matchLoc1.y << std::endl;
					
				}
				break;
			}
		}
	}
	flag = 0;
	while (1) {
		//matchR = matchLoc;
		cv::Mat src = cv::Mat(img, cv::Rect(matchR, cv::Size(cols, rows)));
		cv::cvtColor(src, src, cv::COLOR_BGR2GRAY);
		cv::threshold(src, src, 30, 255, cv::THRESH_BINARY);
		cv::meanStdDev(src, mean, stdDev);
		double mean_src = mean.ptr<double>(0)[0];
		if (flag == 0) {
			if (std::abs(mean_src - mean_dst) < minR) {
				minR = std::abs(mean_src - mean_dst);
				if(matchR.x+5<=2488)
					matchR.x = matchR.x + 5;
			}
			else
			{
				matchR.x = matchR.x - 5;
				flag = 1;
			}
		}
		else {
			if (std::abs(mean_src - mean_dst) < minR) {
				minR = std::abs(mean_src - mean_dst);
				if (matchR.x + 1 <= 2488)
					matchR.x = matchR.x + 1;
			}
			else
			{
				matchR.x = matchR.x - 1;
				if (minR < min)
				{
					min = minR;
					std::cout << min << std::endl;

					matchLoc1 = matchR;
					std::cout << "matchR:"<<matchLoc1.x << "," << matchLoc1.y << std::endl;
					
				}
				break;
			}
		}
	}
	flag = 0;
	while (1) {
		//matchL = matchLoc;
		cv::Mat src = cv::Mat(img, cv::Rect(matchL, cv::Size(cols, rows)));
		cv::cvtColor(src, src, cv::COLOR_BGR2GRAY);
		cv::threshold(src, src,30, 255, cv::THRESH_BINARY);
		cv::meanStdDev(src, mean, stdDev);
		double mean_src = mean.ptr<double>(0)[0];
		if (flag == 0) {
			if (std::abs(mean_src - mean_dst) < minL) {
				minL = std::abs(mean_src - mean_dst);
				if (matchL.x - 5 >= 0)
					matchL.x = matchL.x - 5;
			}
			else
			{
				matchL.x = matchL.x + 5;
				flag = 1;
			}
		}
		else {
			if (std::abs(mean_src - mean_dst) < minL) {
				minL = std::abs(mean_src - mean_dst);
				if (matchL.x - 1 >= 0)
					matchL.x = matchL.x - 1;
			}
			else
			{
				matchL.x = matchL.x + 1;
				if (minL < min)
				{
					min = minL;
					std::cout << min << std::endl;

					matchLoc1 = matchL;
					std::cout <<"matchL:"<< matchLoc1.x << "," << matchLoc1.y << std::endl;
					
				}
				break;
			}
		}
	}
	matchLoc = matchLoc1;
	std::cout << matchLoc1.x << "," << matchLoc1.y << std::endl;
}

void picture(cv::Mat img, cv::Mat dst, cv::Point &matchLoc,cv::Mat  templateImage) {
	for (int i = matchLoc.y; i < matchLoc.y+templateImage.rows; i++) {
		for (int j = matchLoc.x; j < matchLoc.x+templateImage.cols; j++) {
			dst.at<cv::Vec3b>(i, j)[0] = img.at<cv::Vec3b>(i, j)[0];
			dst.at<cv::Vec3b>(i, j)[1] = img.at<cv::Vec3b>(i, j)[1];
			dst.at<cv::Vec3b>(i, j)[2] = img.at<cv::Vec3b>(i, j)[2];
		}
	}
}

void SegmentRGB(cv::Mat src, cv::Mat &dst, cv::Point &matchLoc, cv::Mat &templateImage) {
	double distance = 0.0;
	double d = (double)57 / 168;
	int col = matchLoc.x;
	int row = matchLoc.y;
	int width = templateImage.cols;
	int height = templateImage.rows;
	for (int i = matchLoc.x; i < matchLoc.x + width; i++) {
		for (int j = matchLoc.y; j <  matchLoc.y + height; j++) {
			cv::Vec3b xx = src.at<cv::Vec3b>(j, i);
			//distance = Chanel3Distance(xx, color_center);
			double t = (double)xx[0] / xx[1];
			int c = dst.at<uchar>(j, i);
			if (c == 255) {
				//std::cout << t << std::endl;
				if (t > d - 0.12&&t < d + 0.12) {
					dst.at<uchar>(j, i) = 0;
					//dst.at<cv::Vec3b>(j, i)[0] = 0;
					//dst.at<cv::Vec3b>(j, i)[1] = 0;
					//dst.at<cv::Vec3b>(j, i)[2] = 0;
				}
				else {
					dst.at<uchar>(j, i) = 255;
					//dst.at<cv::Vec3b>(j, i)[0] = 255;
					//dst.at<cv::Vec3b>(j, i)[1] = 255;
					//dst.at<cv::Vec3b>(j, i)[2] = 255;
				}
			}
		}
	}
}

int main()
{
	image = imread("F:\\test\\test\\7.bmp", cv::IMREAD_COLOR);

	//使用以下代码得到模板，和rgb平均颜色的模板（在两块金镀上分别取一块颜色区域，求平均rgb）
	/*image.copyTo(img);
	image.copyTo(tmp);
	cv::namedWindow("img", 0);
	cv::resizeWindow("img", img.rows / 2, img.cols / 2);
	cv::setMouseCallback("img", on_mouse, 0);
	cv::waitKey(0);*/
	
	cv::Mat templateImage = imread("F:\\test\\Model\\model.bmp", cv::IMREAD_COLOR);
	int result_cols = image.cols - templateImage.cols + 1;
	int result_rows = image.rows - templateImage.rows + 1;

	//模板匹配，匹配到的区域为matchLoc.x matchLoc.y  match.x+templateImage.cols match.y+ templateImage.rows包围的区域
	cv::Mat result = cv::Mat(result_cols, result_rows, CV_32FC1);
	cv::matchTemplate(image, templateImage, result, CV_TM_SQDIFF);
	double minVal, maxVal;
	cv::Point minLoc, maxLoc,matchLoc;
	cv::minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat());
	matchLoc = minLoc;
	//cv::Mat image1 = cv::Mat::zeros(image.rows,image.cols,image.type());
	cv::Mat image1 = image.clone();
	cv::Mat mask = cv::Mat::zeros(image.rows, image.cols, CV_8U);
	cv::rectangle(image1, cv::Rect(matchLoc, cv::Size(templateImage.cols, templateImage.rows)), cv::Scalar(255, 0, 0),3, 8, 0);
	//cv::Mat dst = cv::Mat(image, cv::Rect(matchLoc, cv::Size(templateImage.cols, templateImage.rows)));
	//cv::rectangle(image1, cv::Rect(matchLoc, cv::Size(templateImage.cols, templateImage.rows)), cv::Scalar(0, 0, 255), 5, 8, 0);
	meanColor(image, templateImage, templateImage.cols, templateImage.rows,matchLoc);
	cv::rectangle(image1, cv::Rect(matchLoc, cv::Size(templateImage.cols, templateImage.rows)), cv::Scalar(0, 255, 0), 3, 8, 0);
	//picture(image, image1, matchLoc, templateImage);
	//cv::Mat image2 = image1.clone();
	//cv::cvtColor(image1, image1, cv::COLOR_BGR2GRAY);
	//cv::threshold(image1, image1, 30, 255, cv::THRESH_BINARY);
	////cv::namedWindow("dst1", 0);
	////cv::resizeWindow("dst1", image1.cols / 4, image1.rows / 4);
	////cv::imshow("dst1", image1);
	//std::vector<std::vector<cv::Point>>contours;
	//std::vector<cv::Vec4i>hierarchy;
	//findContours(image1, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
	//for (int i = 0; i<contours.size(); i++)
	//{
	//	double g_dConArea = contourArea(contours[i], true);
	//	double b = std::abs(g_dConArea);
	//	if (b > 50000) {
	//		drawContours(mask, contours, i, cv::Scalar(255),CV_FILLED);
	//		double g_dConArea = contourArea(contours[i], true);
	//		double b = std::abs(g_dConArea);
	//	}
	//}
	//cv::rectangle(image, cv::Rect(matchLoc, cv::Size(templateImage.cols, templateImage.rows)), cv::Scalar(0, 255,0), 5, 8, 0);
	//cv::rectangle(image, cv::Rect(matchLoc1, cv::Size(templateImage.cols, templateImage.rows)), cv::Scalar(0, 0, 255), 5, 8, 0);
	//dst = cv::Mat(image, cv::Rect(matchLoc, cv::Size(templateImage.cols, templateImage.rows)));
	//matchLoc.y = matchLoc.y + 5;
	//meanColor(image, templateImage, matchLoc, templateImage.cols, templateImage.rows);
	//cv::rectangle(image, cv::Rect(matchLoc, cv::Size(templateImage.cols, templateImage.rows)), cv::Scalar(0, 255,0), 5, 8, 0);
//	cv::threshold(image1, image1, 250, 255, cv::THRESH_BINARY);
///*	cv::namedWindow("mask", 0);
//	cv::resizeWindow("mask", mask.cols / 4, mask.rows / 4);
//	cv::imshow("mask", mask);
//
//	cv::namedWindow("image2", 0);
//	cv::resizeWindow("image2", image2.cols / 4, image2.rows / 4);
//	cv::imshow("image2", image2);*/
//	SegmentRGB(image2,mask, matchLoc, templateImage);
//
//	
//	cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
//	cv::erode(mask, mask, element);
//	cv::dilate(mask, mask, element);
//	element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(7, 7));
//	cv::dilate(mask, mask, element);
//	cv::erode(mask, mask, element);
//	//element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
//	//cv::erode(mask, mask, element);
//	cv::namedWindow("mask1", 0);
//	cv::resizeWindow("mask1", mask.cols / 4, mask.rows / 4);
//	cv::imshow("mask1", mask);
//	//cv::namedWindow("dst1", 0);
//	//cv::resizeWindow("dst1", image1.cols / 4, image1.rows / 4);
//	//cv::imshow("dst1", image1);
//	//dst = cv::Mat(image, cv::Rect(matchLoc, cv::Size(templateImage.cols, templateImage.rows)));
//	//cv::cvtColor(dst, dst, cv::COLOR_BGR2GRAY);
//	//cv::threshold(dst, dst, 30, 255, cv::THRESH_BINARY);
//	//cv::imshow("ghsdg", dst);
//	//cv::bitwise_and(dst, templateImage, dst);
//	//cv::imshow("dst1", dst);
//	//cv::imwrite("C:\\Users\\dyf01\\Desktop\\and.bmp", dst);
//	//cv::Vec3i v;
//	//meancolor(v);//得到平均的rgb  不用每次都求，求一次即可*//*
//	//cv::Vec3i v(108, 219, 243);
//	//cv::Vec3i v(88, 199, 223);
//	//cv::Mat dst1 = cv::Mat(image.size(), CV_32FC1);
//	
//	//根据rgb颜色模型得到的图像
//	/*SegmentRGB(image1, v,30, matchLoc, templateImage);
//	cv::imwrite("C:\\Users\\dyf01\\Desktop\\锡盖金\\锡盖金\\result\\1rgb.bmp", image1);
//
//	//腐蚀 膨胀 操作
//	cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(30, 30));
//	cv::dilate(image1, image1, element);
//	element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(25, 25));
//	cv::erode(image1, image1, element);
//	cv::imwrite("C:\\Users\\dyf01\\Desktop\\锡盖金\\锡盖金\\result\\2dilate.bmp", image1);
//	*/
//	//cv::cvtColor(image1, image1, cv::COLOR_BGR2GRAY);
//	//cv::GaussianBlur(image1, image1, cv::Size(3, 3), 0);
//	//cv::Canny(image1, image1, 100, 250);
//	std::vector<std::vector<cv::Point>> contours1;
//	std::vector<cv::Vec4i> hierarchy1;
//	findContours(mask, contours1, hierarchy1, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE,cv:: Point());
//	double score=0;
//	for (int i = 0; i < contours1.size(); i++)
//	{
//		//if ((matchLoc.x+10<contours[i][0].x&&contours[i][0].x<matchLoc.x + templateImage.cols-10)
//		//	&& (matchLoc.y+10<contours[i][0].y&&contours[i][0].y<matchLoc.y + templateImage.rows-30)) {
//			//drawContours(image1, contours, i, cv::Scalar(255), 3, 8, hierarchy);
//		double g_dConArea = contourArea(contours1[i], true);
//		double b = std::abs(g_dConArea);
//		if (b >1500) {
//			//score = 1 - b / 100000;
//		/*	score += 0.35*(1-(b-100000)/100000);
//			cv::RotatedRect rect = minAreaRect(contours[i]);
//			double a = std::abs(rect.angle);
//			if (a > 45)
//				a = 90 - a;
//			if (a<2)
//				score += 0.05*(1-std::abs(a)/2);
//			double c = (double) rect.size.width / rect.size.height;
//			if (c > 1)
//				c = 2 - c;
//			if (c > 0.7&&c < 1)
//				score += 0.05*(1 - std::abs(c - 0.83) / 0.83);
//			double d = double(std::abs(g_dConArea) / (rect.size.width*rect.size.height));
//			if (d > 0.6&&d < 0.7)
//				score += 0.05*(1 - std::abs(c - 0.6433) / 0.6433);
//			std::cout << a << " " << b << " " << c << " " << d << std::endl;
//			if (score>0.2)*/
//			drawContours(image, contours1, i, cv::Scalar(255), 3, 8, hierarchy1);
//			//}
//	   //}
//		}
//	}
	cv::namedWindow("image", 0);
	cv::resizeWindow("image", image1.cols / 4, image1.rows / 4);
	cv::imshow("image", image1);
	
	/*std::cout <<"该工件的检测分数为：" <<score << std::endl<<std::endl;
	if (score > 0.8)
		std::cout << "该工件是检测结果：" << std::endl << "OK" << std::endl;
	else
		std::cout << "该工件是检测结果：" << std::endl << "NG" << std::endl;*/
	//cv::imwrite("C:\\Users\\dyf01\\Desktop\\锡盖金\\锡盖金\\result\\3contours.bmp", image1);
	//cv::imwrite("C:\\Users\\dyf01\\Desktop\\锡盖金\\锡盖金\\result\\4contours_原图.bmp", image);
	//system("pause");
	cv::waitKey(0);
	return 0;
}
