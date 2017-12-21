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

int main(int argc, char** argv){

	Mat srcimage = imread("hw2_original_image.jpg", IMREAD_COLOR);

	int width = srcimage.cols;
	int height = srcimage.rows;

	Point2f srcver[4];

	srcver[0] = Point(109,22);
	srcver[1] = Point(776,103);
	srcver[2] = Point(40,392);
	srcver[3] = Point(714, 493);

	Point2f transformver[4];

	transformver[0] = Point(0,0);
	transformver[1] = Point(width,0);
	transformver[2] = Point(0,height);
	transformver[3] = Point(width, height);

	Mat warpMatrix = getPerspectiveTransform(srcver, transformver);
    Mat warped;

    warpPerspective(srcimage, warped, warpMatrix, warped.size(), INTER_LINEAR, BORDER_CONSTANT);

    imshow("Perspective Transform", warped);
	waitKey(0);

	return 0;
}
