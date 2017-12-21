#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>
#include <opencv2/opencv.hpp>
#include <cstdio>
#include <opencv2/ml/ml.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <string.h>
#include <string>

using namespace std;
using namespace cv;

void main(){
	Mat image = imread("homework_1_original.bmp", IMREAD_COLOR);
	Mat new_image = image.clone();
	Mat cut_image;

	//int Row = image.rows;
	int Col = image.cols;
	int newRow = 0;
	int newCol = 0;
	int temp1 = 0;
	int b = 0;


	Mat_<Vec3b>::iterator Begin = image.begin<Vec3b>();
	Mat_<Vec3b>::iterator End = image.end<Vec3b>();
	Mat_<Vec3b>::iterator New = new_image.begin<Vec3b>();

	while(Begin != End){
		if((*Begin)[0] == 0 &&  (*Begin)[1] == 0 && (*Begin)[2] == 255){
			(*New)[0] = 200;
			(*New)[1] = 200;
			(*New)[2] = 0;
		}

		if((*Begin)[0] == 255 &&  (*Begin)[1] == 0 && (*Begin)[2] == 0){
			(*New)[0] = 128;
			(*New)[1] = 0;
			(*New)[2] = 128;
		}

		if((*Begin)[0] == 0 &&  (*Begin)[1] == 255 && (*Begin)[2] == 0){
			newRow = temp1 / Col;
			newCol = temp1 - (newRow * Col);
			b = newRow - 300;

			new_image.at<Vec3b>(b, newCol)[0] = image.at<Vec3b>(newRow, newCol)[0];
			new_image.at<Vec3b>(b, newCol)[0] = image.at<Vec3b>(newRow, newCol)[0];
			new_image.at<Vec3b>(b, newCol)[0] = image.at<Vec3b>(newRow, newCol)[0];
			new_image.at<Vec3b>(newRow, newCol)[0] = {255,255,255};
		}

		if((*Begin)[0] == 0 &&  (*Begin)[1] == 255 && (*Begin)[2] == 255){
			newRow = temp1 / Col;
			newCol = temp1 - (newRow * Col);
		}

		++temp1;
		++Begin;
		++New;
	}

	cut_image = image(Rect(682, 390, 111, 220));

	imshow("Original Image", image);
	imshow("New Image", new_image);
	imshow("Cut off Image", cut_image);

	imwrite("hw1_change_image.jpg", new_image);
	imwrite("hw1_roi_image.jpg", cut_image);



	waitKey(0);

}
