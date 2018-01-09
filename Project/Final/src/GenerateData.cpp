// GenData.cpp

#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/ml/ml.hpp>

#include<iostream>
#include<vector>


const int MIN_CONTOUR_AREA = 100;

const int RESIZED_IMAGE_WIDTH = 20;
const int RESIZED_IMAGE_HEIGHT = 30;


int main() {

    cv::Mat imgTrainingNumbers;         // input image
    cv::Mat imgGrayscale;
    cv::Mat imgBlurred;
    cv::Mat imgThresh;
    cv::Mat imgThreshCopy;

    std::vector<std::vector<cv::Point> > ptContours;
    std::vector<cv::Vec4i> v4iHierarchy;

    cv::Mat matClassificationInts;      // these are our training classifications, note we will have to perform some conversions before writing to file later

    cv::Mat matTrainingImagesAsFlattenedFloats;

    char a[]={ '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z'};
    std::vector<char> intValidChars (a, a + sizeof(a)/sizeof(char));

    imgTrainingNumbers = cv::imread("training_chars.jpg");

    if (imgTrainingNumbers.empty()) {
        std::cout << "error: image not read from file\n\n";
        return(0);
    }

    cv::cvtColor(imgTrainingNumbers, imgGrayscale, CV_BGR2GRAY);

    cv::GaussianBlur(imgGrayscale,
        imgBlurred,
        cv::Size(5, 5),
        0);


    cv::adaptiveThreshold(imgBlurred, imgThresh, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY_INV, 11, 2);

    cv::imshow("ThresholdImage", imgThresh);

    imgThreshCopy = imgThresh.clone();

    cv::findContours(imgThreshCopy,             // input image
        ptContours,                             // output contours
        v4iHierarchy,                           // output hierarchy
        cv::RETR_EXTERNAL,                      // outermost contours only
        cv::CHAIN_APPROX_SIMPLE);               // compress horizontal, vertical, and diagonal segments and leave only their end points

    for (int i = 0; i < ptContours.size(); i++) {
        if (cv::contourArea(ptContours[i]) > MIN_CONTOUR_AREA) {
            cv::Rect boundingRect = cv::boundingRect(ptContours[i]);

            cv::rectangle(imgTrainingNumbers, boundingRect, cv::Scalar(0, 255, 0), 2);

            cv::Mat matROI = imgThresh(boundingRect);           // get ROI image of bounding rect

            cv::Mat matROIResized;
            cv::resize(matROI, matROIResized, cv::Size(RESIZED_IMAGE_WIDTH, RESIZED_IMAGE_HEIGHT));     // resize image,

            cv::imshow("matROI", matROI);
            cv::imshow("matROIResized", matROIResized);
            cv::imshow("imgTrainingNumbers", imgTrainingNumbers);

            int intChar = cv::waitKey(0);

            if (intChar == 27) {        //esc
                return(0);
            }
            else if (std::find(intValidChars.begin(), intValidChars.end(), intChar) != intValidChars.end()) {     // else if the char is in the list of chars we are looking for . . .

                matClassificationInts.push_back(intChar);       // append classification char to integer list of chars

                cv::Mat matImageFloat;
                matROIResized.convertTo(matImageFloat, CV_32FC1);

                cv::Mat matImageFlattenedFloat = matImageFloat.reshape(1, 1);

                matTrainingImagesAsFlattenedFloats.push_back(matImageFlattenedFloat);       // add to Mat as though it was a vector, this is necessary due to the
                                                                                            // data types that KNearest.train accepts
            }
        }
    }

    std::cout << "training complete\n\n";


    cv::FileStorage fsClassifications("classifications.xml", cv::FileStorage::WRITE);

    if (fsClassifications.isOpened() == false) {
        std::cout << "error, unable to open training classifications file, exiting program\n\n";
        return(0);
    }

    fsClassifications << "classifications" << matClassificationInts;
    fsClassifications.release();



    cv::FileStorage fsTrainingImages("images.xml", cv::FileStorage::WRITE);

    if (fsTrainingImages.isOpened() == false) {
        std::cout << "error, unable to open training images file, exiting program\n\n";
        return(0);
    }

    fsTrainingImages << "images" << matTrainingImagesAsFlattenedFloats;
    fsTrainingImages.release();

    return(0);
}



