// TrainAndTest.cpp

#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/ml/ml.hpp>

#include<iostream>
#include<sstream>
#include<fstream>

const int MIN_CONTOUR_AREA = 100;

const int RESIZED_IMAGE_WIDTH = 20;
const int RESIZED_IMAGE_HEIGHT = 30;
using namespace std;

class ContourWithData {
public:
    std::vector<cv::Point> ptContour;
    cv::Rect boundingRect;
    float fltArea;


    bool checkIfContourIsValid() {
        if (fltArea < MIN_CONTOUR_AREA) return false;
        return true;
    }


    static bool sortByBoundingRectinPosition(const ContourWithData& cwdLeft, const ContourWithData& cwdRight) {
        return(cwdLeft.boundingRect.x < cwdRight.boundingRect.x);
    }

};


int main() {
    std::vector<ContourWithData> allContoursWithData;           // declare empty vectors,
    std::vector<ContourWithData> validContoursWithData;


    cv::Mat matClassificationInts;

    cv::FileStorage fsClassifications("classifications.xml", cv::FileStorage::READ);

    if (fsClassifications.isOpened() == false) {
        std::cout << "error, unable to open training classifications file, exiting program\n\n";
        return(0);
    }

    fsClassifications["classifications"] >> matClassificationInts;
    fsClassifications.release();

    cv::Mat matTrainingImagesAsFlattenedFloats;         // we will read images

    cv::FileStorage fsTrainingImages("images.xml", cv::FileStorage::READ);          // open the training images file

    if (fsTrainingImages.isOpened() == false) {
        std::cout << "error, unable to open training images file, exiting program\n\n";
        return(0);
    }

    fsTrainingImages["images"] >> matTrainingImagesAsFlattenedFloats;
    fsTrainingImages.release();



    cv::Ptr<cv::ml::KNearest>  kNearest(cv::ml::KNearest::create());

    kNearest->train(matTrainingImagesAsFlattenedFloats, cv::ml::ROW_SAMPLE, matClassificationInts);


    cv::Mat matTestingNumbers = cv::imread("test3.jpg"); //input image/////////////////////////////////////////////////////////////////////

    if (matTestingNumbers.empty()) {
        std::cout << "error: image not read from file\n\n";
        return(0);
    }

    cv::Mat matGrayscale;
    cv::Mat matBlurred;
    cv::Mat matThresh;
    cv::Mat matThreshCopy;
    cv::cvtColor(matTestingNumbers, matGrayscale, CV_BGR2GRAY);


    cv::GaussianBlur(matGrayscale,
        matBlurred,
        cv::Size(5, 5),            // smoothing window width and height in pixels
        0);                        // sigma value


    cv::adaptiveThreshold(matBlurred,
        matThresh,
        255,
        cv::ADAPTIVE_THRESH_GAUSSIAN_C,
        cv::THRESH_BINARY_INV,
        11,
        2);

    matThreshCopy = matThresh.clone();

    std::vector<std::vector<cv::Point> > ptContours;
    std::vector<cv::Vec4i> v4iHierarchy;

    cv::findContours(matThreshCopy,
        ptContours,
        v4iHierarchy,
        cv::RETR_EXTERNAL,                      // retrieve the outermost contours only
        cv::CHAIN_APPROX_SIMPLE);               // compress horizontal, vertical, and diagonal segments and leave only their end points

    for (int i = 0; i < ptContours.size(); i++) {
        ContourWithData contourWithData;                                                    // instantiate a contour with data object
        contourWithData.ptContour = ptContours[i];                                          // assign contour to contour with data
        contourWithData.boundingRect = cv::boundingRect(contourWithData.ptContour);         // get the bounding rect
        contourWithData.fltArea = cv::contourArea(contourWithData.ptContour);               // calculate the contour area
        allContoursWithData.push_back(contourWithData);                                     // add contour with data object to list of all contours with data
    }

    for (int i = 0; i < allContoursWithData.size(); i++) {                      // for all contours
        if (allContoursWithData[i].checkIfContourIsValid()) {                   // check if valid
            validContoursWithData.push_back(allContoursWithData[i]);            // if so, append to valid contour list
        }
    }

    std::sort(validContoursWithData.begin(), validContoursWithData.end(), ContourWithData::sortByBoundingRectinPosition);

    std::string strFinalString;         // declare final string

    for (int i = 0; i < validContoursWithData.size(); i++) {            // for each contour


        cv::rectangle(matTestingNumbers,
            validContoursWithData[i].boundingRect,
            cv::Scalar(0, 255, 0),
            2);

        cv::Mat matROI = matThresh(validContoursWithData[i].boundingRect);          // get ROI image of bounding rect

        cv::Mat matROIResized;
        cv::resize(matROI, matROIResized, cv::Size(RESIZED_IMAGE_WIDTH, RESIZED_IMAGE_HEIGHT));     // resize image, this will be more consistent for recognition and storage

        cv::Mat matROIFloat;
        matROIResized.convertTo(matROIFloat, CV_32FC1);             // convert Mat to float, necessary for call to find_nearest

        cv::Mat matROIFlattenedFloat = matROIFloat.reshape(1, 1);

        cv::Mat matCurrentChar(0, 0, CV_32F);

        kNearest->findNearest(matROIFlattenedFloat, 3, matCurrentChar);

        float fltCurrentChar = (float)matCurrentChar.at<float>(0, 0);

        strFinalString = strFinalString + char(int(fltCurrentChar));
    }

    std::cout << "\n\n" << "Text read = " << strFinalString << "\n\n";

    ofstream FinalText;                         //output a file with the final string
    FinalText.open ("Text.txt");
    FinalText << strFinalString;
    FinalText.close();

    cv::imshow("TestingNumbers", matTestingNumbers);

    cv::waitKey(0);

    return(0);
}

