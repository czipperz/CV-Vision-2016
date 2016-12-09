#include "calculations.hh"

//http://stackoverflow.com/questions/9074202/opencv-2-centroid
cv::Point computeCentroid(const cv::Mat& mask) {
    cv::Moments m = moments(mask, true);
    return cv::Point(m.m10 / m.m00, m.m01 / m.m00);
}

float calcSlope(const cv::Vec4i& line) {
    if (line[2] - line[0] != 0) {
        return (static_cast<float>(line[3]) -
                static_cast<float>(line[1])) /
               (static_cast<float>(line[2]) -
                static_cast<float>(line[0]));
    } else {
        return 9999999; //Pseudo infinity
    }
}

cv::Point calcMidPt(const cv::Vec4i& line) {
    return cv::Point((line[0] + line[2]) / 2,
                     (line[1] + line[3]) / 2);
}

float calcLength(const cv::Vec4i& line) {
    int distancex = line[2] - line[0];
    distancex *= distancex;
    int distancey = line[3] - line[1];
    distancey *= distancey;
    return std::sqrt(static_cast<float>(distancex + distancey));
}
