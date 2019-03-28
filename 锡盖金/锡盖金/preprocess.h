#pragma once
#include<iostream>
#include<opencv2/opencv.hpp>
double Chanel3Distance(cv::Vec3i x, cv::Vec3b g);
void SegmentRGB(cv::Mat &dst, cv::Vec3i &color_center, double threshold, cv::Point &matchLoc, cv::Mat &templateImage);
void meancolor(cv::Vec3i &color_center);