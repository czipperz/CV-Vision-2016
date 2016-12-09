#ifndef HEADER_GUARD_VARIABLES_H
#define HEADER_GUARD_VARIABLES_H

#include <opencv2/opencv.hpp>
#include <mutex>
#include <string>

extern std::mutex mutex;
extern std::string socket_send_xml_buffer;

extern const char* const host_name;

extern const int ESC;
extern const int portno;

extern bool UI;

//Images
extern cv::Mat src, hsv_src, hsv_filtered, masked_bgr, rect_ROI, rect_ROI_canny;
//Trackbars
extern const int slider_max_sat_val;
extern const int slider_max_hue;

//Data

//Reflective tape (High Light)
//Lower
extern int hue_slider_lower;
extern int sat_slider_lower;
extern int val_slider_lower;
//Upper
extern int hue_slider_upper;
extern int sat_slider_upper;
extern int val_slider_upper;
//Canny
extern int canny_slider;
//Hough
extern int hough_slider;
//Morph
extern int morph_slider;
extern int morph_slider_max;
//Shape-detect
extern int shape_slider_max;
extern int shape_slider_lower;
extern int shape_slider_upper;

#endif
