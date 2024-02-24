#pragma once

#ifndef HOUGH_H
#define HOUGH_H
#endif

#include<opencv2/opencv.hpp>
#include<vector>
using namespace std;
using namespace cv;

class hough {

private:
	Mat im_g, im_b, im_h;
	int showTimes = 0;
	int maxVal = 0;
	int channel = 2;
	
public:
	Mat im_drawLine;
	int channel_num = 1;
	void setChannel(const int&);
	hough(const Mat&, const int& thesh = 180);
	void count_points(const float&, const float&);
	void showLine(const int&, const int& Length = INT_MAX, const bool& wholeLine = 1); //number of lines(para)
	void transfer();
	void transfer_t(const int&, const int&, const int&);
	void showHough(const int& channel = 0);
};

class hough3 :public hough {
public:
	vector<Mat> bgr;
	hough3(const Mat& mat3D, const Mat& mat_gray, const int& thesh = 180);
	void showLine(const int&, const int& Length = INT_MAX, const bool& wholeLine = 1); //channel number, number of lines(para)
	
};