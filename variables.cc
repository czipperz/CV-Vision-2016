#include "variables.hh"

mutexed<std::string> socketSendXmlBuffer;

const char* const hostName = "roborio-955-frc.local";

const int ESC = 27;
const int portno = 5805;

//Images
cv::Mat src;
cv::Mat hsvSrc;
cv::Mat hsvFiltered;
cv::Mat maskedBgr;
cv::Mat rectRoi;
cv::Mat rectRoiCanny;

//Trackbars
const int sliderMaxSatVal = 256;
const int sliderMaxHue = 179;

//Data

//Reflective tape (High Light)
//Lower
int hueSliderLower = 49;
int satSliderLower = 0;
int valSliderLower = 223;
//Upper
int hueSliderUpper = 97;
int satSliderUpper = 164;
int ValSliderUpper = 256;
//Canny
int cannySlider = 35;
//Hough
int houghSlider = 256;
//Morph
int morphSlider = 2;
int morphSliderMax = 8;
//Shape-detect
int shapeSliderMax = 100;
int shapeSliderLower = 20;
int shapeSliderUpper = 45;

/*
//Reflective tape (Low Light)
//Lower
int hueSliderLower = 81;
int satSliderLower = 29;
int valSliderLower = 235;
//Upper
int hueSliderUpper = 94;
int satSliderUpper = 256;
int valSliderUpper = 256;
//Canny
int cannySlider = 69;
//Hough
int houghSlider = 256;
//Morph
int morphSlider = 2;
int morphSliderMax = 8;
*/

/*
//Blue Tape
//Lower
int hueSliderLower = 102;
int satSliderLower = 90;
int valSliderLower = 91;
//Upper
int hueSliderUpper = 115;
int satSliderUpper = 220;
int valSliderUpper = 256;
*/
