#include<iostream>
#include<opencv2/opencv.hpp>
#include<vector>
#include<thread>
#include<mutex>
#include"Hough.h"

using namespace std;
using namespace cv;



int main() {
	
	// Rhombus

	Mat im = imread("Rhombus.jpg", 1);
	imshow("im", im);
	waitKey(1);
	
	Mat im_g;
	cvtColor(im, im_g, COLOR_BGR2GRAY);
	hough3 Rhombus(im, im_g, 150);

	Rhombus.showHough();
	Rhombus.showLine(20);


	im = imread("Three Lines.jpg", 1);
	imshow("im", im);
	waitKey(1);
	
	cvtColor(im, im_g, COLOR_BGR2GRAY);
	hough3 Lines(im, im_g, 150);

	Lines.showHough();
	Lines.showLine(1, 12000, 0);
	Lines.showLine(1, 10000, 0);
	Lines.showLine(1, 8000, 0);
	return 0;
}

