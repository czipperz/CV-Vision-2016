#ifndef HEADER_GUARD_LINE_INFO_H
#define HEADER_GUARD_LINE_INFO_H

#include "calculations.hh"

#include <opencv2/opencv.hpp>

class LineInfo {
public:
    cv::Vec4i line;
    float length;
    float slope;
    cv::Point midPt;

    LineInfo() {}
    LineInfo(const cv::Vec4i& line)
        : line(line)
        , length(calcLength(line))
        , slope(calcSlope(line))
        , midPt(calcMidPt(line)) {}
};

static inline LineInfo createLineInfo(const cv::Vec4i& line) {
    return line;
}

#endif
