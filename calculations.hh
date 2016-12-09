#ifndef HEADER_GUARD_CALCULATIONS_H
#define HEADER_GUARD_CALCULATIONS_H

#include <opencv2/opencv.hpp>

cv::Point computeCentroid(const cv::Mat& mask);

float calcSlope(const cv::Vec4i& line);

cv::Point calcMidPt(const cv::Vec4i& line);

float calcLength(const cv::Vec4i& line);

#endif
