#ifndef HEADER_GUARD_LINE_INFO_H
#define HEADER_GUARD_LINE_INFO_H

#include <opencv2/opencv.hpp>

class LineInfo {
public:
    cv::Vec4i line;
    float length;
    float slope;
    cv::Point midPt;
};

#endif
