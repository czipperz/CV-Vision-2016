#include "variables.hh"

std::mutex mutex;
std::string socket_send_xml_buffer;

const char* const host_name = "roborio-955-frc.local";

const int ESC = 27;
const int portno = 5805;

bool UI = false;

//Images
cv::Mat src, hsv_src, hsv_filtered, masked_bgr, rect_ROI, rect_ROI_canny;

//Trackbars
const int slider_max_sat_val = 256;
const int slider_max_hue = 179;

//Data

//Reflective tape (High Light)
//Lower
int hue_slider_lower = 49;
int sat_slider_lower = 0;
int val_slider_lower = 223;
//Upper
int hue_slider_upper = 97;
int sat_slider_upper = 164;
int val_slider_upper = 256;
//Canny
int canny_slider = 35;
//Hough
int hough_slider = 256;
//Morph
int morph_slider = 2;
int morph_slider_max = 8;
//Shape-detect
int shape_slider_max = 100;
int shape_slider_lower = 20;
int shape_slider_upper = 45;

/*
//Reflective tape (Low Light)
//Lower
int hue_slider_lower = 81;
int sat_slider_lower = 29;
int val_slider_lower = 235;
//Upper
int hue_slider_upper = 94;
int sat_slider_upper = 256;
int val_slider_upper = 256;
//Canny
int canny_slider = 69;
//Hough
int hough_slider = 256;
//Morph
int morph_slider = 2;
int morph_slider_max = 8;
*/

/*
//Blue Tape
//Lower
int hue_slider_lower = 102;
int sat_slider_lower = 90;
int val_slider_lower = 91;
//Upper
int hue_slider_upper = 115;
int sat_slider_upper = 220;
int val_slider_upper = 256;
*/
