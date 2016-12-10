#ifndef HEADER_GUARD_VARIABLES_H
#define HEADER_GUARD_VARIABLES_H

#include <opencv2/opencv.hpp>
#include <mutex>
#include <string>

extern std::mutex mutex;
extern std::string socketSendXmlBuffer;

extern const char* const hostName;

extern const int ESC;
extern const int portno;

//Images
extern cv::Mat src, hsvSrc, hsvFiltered, maskedBgr, rectRoi, rectRoiCanny;
//Trackbars
extern const int sliderMaxSatVal;
extern const int sliderMaxHue;

//Data

//Reflective tape (High Light)
//Lower
extern int hueSliderLower;
extern int satSliderLower;
extern int valSliderLower;
//Upper
extern int hueSliderUpper;
extern int satSliderUpper;
extern int valSliderUpper;
//Canny
extern int cannySlider;
//Hough
extern int houghSlider;
//Morph
extern int morphSlider;
extern int morphSliderMax;
//Shape-detect
extern int shapeSliderMax;
extern int shapeSliderLower;
extern int shapeSliderUpper;

#endif
