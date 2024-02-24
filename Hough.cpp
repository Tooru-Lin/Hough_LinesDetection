#include<iostream>
#include<opencv2/opencv.hpp>
#include<vector>
#include<thread>
#include<mutex>
#include"Hough.h"

using namespace std;
using namespace cv;

mutex mu;



hough::hough(const Mat& im_g, const int& thresh):im_g(im_g) {
	
	threshold(im_g, im_b, thresh, 255, THRESH_BINARY);
	//imshow("img", img);
	im_h = Mat::zeros(sqrt(pow(im_b.cols, 2) + pow(im_b.rows, 2)) + im_b.cols, 180, CV_16UC1);
	/*
	imshow("im_b", im_b);
	waitKey(1);
	*/
	transfer();
}


void hough::transfer() {
	
	int num_t = 12;
	int segment = im_b.rows / num_t;

	vector<thread> threads;

	for (int i = 0; i<num_t; i++){
		int row_start, row_end;
		
		row_start = segment * i;
		row_end = segment * (i+1);
		if (i == num_t) row_end = im_b.rows;
		threads.push_back(thread(&hough::transfer_t, this, i, row_start, row_end));
	}

	for (int i = 0; i<num_t; i++){
		threads[i].join();
	}
}

void hough::transfer_t(const int& i,const int& row_start, const int& row_end) {

	int count = 0;
	for (int y = row_start; y < row_end; y++) {

		if (y % 5 == 0) {
			cout << y+1 << "/" << im_b.rows << ", Thread : " << i << ", count : " << count << endl;
			count++;
		}
		
		for (int x = 0; x < im_b.cols; x++) {
			//cout << j+1 << "/" << im_b.cols << endl;
			if ((int)im_b.ptr<uchar>(y)[x]) {
				count_points(x, y);
			}
		}
	}
}


void hough::count_points(const float& i, const float& j) {


	for (float theta = -90; theta < 90; theta++) {
		float theta_rad = theta * 3.14 / 180;
		float tho = i * cosf(theta_rad) + j * sinf(theta_rad);
		bool start = 0;
		uint count = 0;
		for (int x = 0; x < im_b.cols; x++) {
			int y = (tho - x * cosf(theta_rad)) / sinf(theta_rad);
			if (y >= 0 && y < im_b.rows) {
				start = 1;
				if ((int)im_b.ptr<uchar>(y)[x]) count++;
			}
			else {
				if (start) break;
			}
		}
		
		
		int tho_idx = tho + im_b.cols;
		int theta_idx = theta + 90;
		
		lock_guard<mutex> lock(mu);
		im_h.ptr<ushort>(tho_idx)[theta_idx] += (ushort)count;
	}
}

void hough::showHough(const int& channel) {
	Mat tmp = im_h.clone();
	float gamma = 2;
	tmp.convertTo(tmp, CV_32F);
	pow(tmp, 1.0 / gamma, tmp);
	normalize(tmp, tmp, 0, 255, NORM_MINMAX, CV_8U);
	resize(tmp, tmp, Size(600, 600));
	imshow("Hough"+(channel+'0'), tmp);
	waitKey(1);
}

void hough::showLine(const int& num, const int& Length, const bool& wholeLine) {

	uint count = 0;
	
	vector <pair<int, pair<float, float>>> Line_para;
	for (int tho_idx = 0; tho_idx < im_h.rows; tho_idx++) {
		for (int theta_idx = 0; theta_idx < im_h.cols; theta_idx++) {
			int curVal = (int)im_h.ptr<ushort>(tho_idx)[theta_idx];
			if ((count < 3 || curVal > Line_para[0].first) && (curVal < Length) ) {
				if (count >= num) Line_para.erase(Line_para.begin());
				Line_para.push_back(make_pair(curVal, make_pair(tho_idx-im_b.cols, theta_idx-90)));
				sort(Line_para.begin(), Line_para.end());
				count++;
			}
		}
	}
	if (channel_num == 1) im_drawLine = im_g.clone();
	for (const auto& i : Line_para) {
		cout << i.first << ", (" << i.second.first << ", " << i.second.second << ")" << endl;
		float tho = i.second.first;
		float theta_rad = i.second.second * 3.14 / 180;
		
		bool start = 0;
		for (int x = 0; x < im_drawLine.cols; x++) {
			int y = (tho - x * cosf(theta_rad)) / sinf(theta_rad);
			if (y >= 0 && y < im_drawLine.rows && ((int)im_b.ptr<uchar>(y)[x] || wholeLine)) {
				start = 1;
				im_drawLine.ptr<uchar>(y)[x] = (channel == 2) ? (uchar)255 : (uchar)0;
			}
			else {
				if (start) break;
			}
		}
	}
}

void hough::setChannel(const int& c) {
	channel = c;
}




// Hough3


hough3::hough3(const Mat& im, const Mat& im_g, const int& thresh):hough(im_g, thresh) {
	split(im, bgr);
	channel_num = 3;
}

void hough3::showLine(const int& num, const int& Length, const bool& wholeLine) {
	vector<Mat> im_drawLine_3D;
	for (int c = 0; c<3; c++){
		setChannel(c);
		im_drawLine = bgr[c].clone();
		
		hough::showLine(num, Length, wholeLine);
		cout << "Size = " << im_drawLine.size() << endl;
		im_drawLine_3D.push_back(im_drawLine.clone());
	}
	
	Mat mergedImg;
	merge(im_drawLine_3D, mergedImg);
	imshow("mergedImg", mergedImg);
	waitKey();
	
}