#include "stdafx.h"
#include"preprocess.h"

double Chanel3Distance(cv::Vec3i x, cv::Vec3b g) {
	return sqrt((x[0] - g[0])*(x[0] - g[0]) +
		(x[1] - g[1])*(x[1] - g[1]) +
		(x[2] - g[2])*(x[2] - g[2]));


}
void SegmentRGB(cv::Mat &dst, cv::Vec3i &color_center, double threshold, cv::Point &matchLoc, cv::Mat &templateImage) {
	double distance = 0.0;
	int col = matchLoc.x;
	int row = matchLoc.y;
	int width = templateImage.cols;
	int height = templateImage.rows;
	for (int i = col; i<col + width; i++) {
		for (int j = row; j<row + height; j++) {
			cv::Vec3b xx = dst.at<cv::Vec3b>(j, i);
			//distance = Chanel3Distance(xx, color_center);
			double t = (double)xx[0] / xx[1];
			//std::cout << t << std::endl;
			if (t>0.48&&t<0.5) {
				dst.at<cv::Vec3b>(j, i)[0] = 255;
				dst.at<cv::Vec3b>(j, i)[1] = 255;
				dst.at<cv::Vec3b>(j, i)[2] = 255;
			}
			else {
				dst.at<cv::Vec3b>(j, i)[0] = 0;
				dst.at<cv::Vec3b>(j, i)[1] = 0;
				dst.at<cv::Vec3b>(j, i)[2] = 0;
			}
		}
	}
}

void meancolor(cv::Vec3i &color_center)
{
	cv::Mat src = cv::imread("model_color1.bmp", 1);
	int b1 = 0, g1 = 0, r1 = 0;
	int n1 = src.cols*src.rows;
	for (int i = 0; i<src.rows; i++) {
		for (int j = 0; j<src.cols; j++) {
			int b = src.at<cv::Vec3b>(i, j)[0];
			int g = src.at<cv::Vec3b>(i, j)[1];
			int r = src.at<cv::Vec3b>(i, j)[2];
			b1 = b1 + b;
			g1 = g1 + g;
			r1 = r1 + r;
		}
	}
	cv::Mat src2 = cv::imread("model_color2.bmp", 1);
	int b2 = 0, g2 = 0, r2 = 0;
	int n2 = src2.cols*src2.rows;
	for (int i = 0; i<src2.rows; i++) {
		for (int j = 0; j<src2.cols; j++) {
			int b = src2.at<cv::Vec3b>(i, j)[0];
			int g = src2.at<cv::Vec3b>(i, j)[1];
			int r = src2.at<cv::Vec3b>(i, j)[2];
			b2 = b2 + b;
			g2 = g2 + g;
			r2 = r2 + r;
		}
	}
	color_center[0] = (b1 + b2) / (n1 + n2);
	color_center[1] = (g1 + g2) / (n1 + n2);
	color_center[2] = (r1 + r2) / (n1 + n2);
}

