#include "calculations.hh"
#include "clamp.hh"
#include "line_info.hh"
#include "netcode_thread.hh"
#include "parse_settings.hh"
#include "socket.hh"
#include "to_string.hh"
#include "trackbar.hh"
#include "variables.hh"

#include <arpa/inet.h>
#include <math.h>
#include <math.h>
#include <netdb.h>
#include <netinet/in.h>
// #include <networktables/NetworkTable.h>
// #include <ntcore.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <mutex>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <pugixml.hpp>
#include <thread>

int main(int numArgs, char**) {
    bool UI = numArgs == 2; //Set in UI mode if there is an argument

    std::cout << "Vision Starting..." << std::endl;
    pugi::xml_document settings;
    parse_settings(settings);

    //TODO: Add arg to disable gui (-nogui)

    cv::VideoCapture cap(-1); // open the default camera
    if (!cap.isOpened()) {  // check if we succeeded
        std::cout << "Camera not found" << std::endl;
        return -1;
    }

    const int pixelWidth = 1920 / 2;
    const int pixelHeight = 1080 / 2;

    cap.set(CV_CAP_PROP_FRAME_WIDTH, pixelWidth);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT, pixelHeight);

    //double temp = cap.get(CV_CAP_PROP_EXPOSURE);
    //cap.set(CV_CAP_PROP_EXPOSURE, -40.0);
    //std::cout << temp << std::endl;

    if (UI) {
        cv::namedWindow("Output", CV_WINDOW_AUTOSIZE);
        createTrackbars();
    }

    std::vector<std::vector<cv::Point> > contours;
    u_int frameCount = 0;

    std::cout << "Starting netcode thread..." << std::endl;

    std::thread other(netcodeThread);
    other.detach();

    std::cout << "Start vision loop" << std::endl;

    while (cv::waitKey(1) != ESC) {
        ++frameCount;

        cap >> src; // get a new frame from camera
        if (src.empty()) break;

        //imshow("Source", src);

        //Convert to HSV colorspace
        cv::cvtColor(src, hsvSrc, cv::COLOR_BGR2HSV);
        //HSV Threshold
        cv::inRange(hsvSrc,
                    cv::Scalar(hueSliderLower, satSliderLower,
                       valSliderLower),
                    cv::Scalar(hueSliderUpper, satSliderUpper,
                       valSliderUpper),
                hsvFiltered);
        //Erase old data from last render (background to the HSV threshhold)
        maskedBgr.setTo(cv::Scalar(0, 0, 0, 0));

        // Make sure that objects have a certain area
        const cv::Mat element =
            getStructuringElement(cv::MORPH_RECT,
                                  cv::Size(2 * morphSlider + 1,
                                           2 * morphSlider + 1),
                                  cv::Point(morphSlider,
                                            morphSlider));
        cv::morphologyEx(hsvFiltered, hsvFiltered, cv::MORPH_OPEN,
                         element);
        //Mask the final image by the HSV selection
        src.copyTo(maskedBgr, hsvFiltered);

        cv::findContours(hsvFiltered.clone(), contours,
                         CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE,
                         cv::Point(0, 0));

        std::vector<cv::Point> hull;

        pugi::xml_document sendToRio;

        pugi::xml_node visionNode = sendToRio.append_child("Vision");
        visionNode.append_attribute("frameNumber")
            .set_value(frameCount);

        u_int shapeCount = 0;

        for (std::size_t idx = 0; idx < contours.size(); idx++) {
            cv::Rect boundRect = boundingRect(contours[idx]);
            cv::Point boundCenter =
                cv::Point(boundRect.width / 2, boundRect.height / 2) +
                boundRect.tl();

            cv::convexHull(cv::Mat(contours[idx]), hull, false);
            //drawContours(masked_bgr, boundRect, idx, Scalar(0, 0, 255));
            //drawContours(masked_bgr, contours, idx,
            //             Scalar(0, 255, 255), 1, 8, noArray(), 0,
            //             Point());

            //Find the U shape
            const double hullArea = contourArea(hull);
            const double contArea = contourArea(contours[idx]);
            //double perimeter = arcLength(contours[idx], false);
            const double areaRatio = hullArea / contArea;
            //std::cout << patch::to_string(areaRatio) << std::endl;

            if (shapeSliderLower / 10.0f < areaRatio &&
                areaRatio < shapeSliderUpper / 10.0f /*4.5*/) {
                shapeCount++;
                cv::rectangle(maskedBgr, boundRect,
                              cv::Scalar(0, 255, 255), 2);
                rectRoi = hsvFiltered(boundRect);
                if (cannySlider == 0) { cannySlider = 1; }
                cv::Canny(rectRoi, rectRoiCanny, cannySlider, 0, 3);

                //imshow("Canny", rect_ROI_canny);

                std::vector<cv::Vec4i> lines;
                cv::HoughLinesP(rectRoiCanny, lines, 1, CV_PI / 180,
                            cannySlider, cannySlider, 10);

                std::vector<LineInfo> lineInfoCache(lines.size());
                LineInfo line_exclude = cv::Vec4i(0, 0, 0, 0);

                std::transform(lines.begin(), lines.end(),
                               lineInfoCache.begin(), createLineInfo);
                std::replace_if(lineInfoCache.begin(),
                                lineInfoCache.end(),
                                [](const LineInfo& current) {
                                    // Exclude slopes > 45 degrees
                                    return current.slope >= 1.0 ||
                                           current.slope <= -1.0;
                                },
                                line_exclude);

                std::sort(lineInfoCache.begin(), lineInfoCache.end(),
                          [](const LineInfo& a, const LineInfo& b) {
                              return a.length < b.length;
                          });

                double transMag = 0.0;
                std::for_each(
                    lines.begin(), lines.end(),
                    [&](const cv::Vec4i& l) {
                        if (calcLength(l) ==
                            lineInfoCache[0].length) {
                            cv::line(maskedBgr,
                                     cv::Point(l[0], l[1]) +
                                         boundRect.tl(),
                                     cv::Point(l[2], l[3]) +
                                         boundRect.tl(),
                                     cv::Scalar(255, 0, 255), 2,
                                     CV_AA);
                            const double degrees =
                                std::atan2(l[3] - l[1], l[2] - l[0]) *
                                180.0 / CV_PI;
                            transMag = degrees * (100.0 / 45.0);
                            putText(maskedBgr,
                                    ("Translate: " +
                                     patch::to_string(
                                         static_cast<int>(transMag)))
                                        .c_str(),
                                    boundCenter + cv::Point(-65, 105),
                                    cv::FONT_HERSHEY_COMPLEX_SMALL, 1,
                                    cv::Scalar(0, 0, 255));
                        }
                    });

                cv::circle(maskedBgr, boundCenter, 5,
                           cv::Scalar(0, 255, 0), 1, 8, 0);

                // double rotateMag =
                //     (((double)boundCenter.x / (double)pixelWidth) -
                //      0.5) *
                //     200.0;

                const int screenCenterX = pixelWidth / 2;
                const int distanceToCenterX = screenCenterX -
                                              boundCenter.x +
                                              boundRect.width / 2;
                const int yPos = boundCenter.y;
                const double distanceToGoal =
                    0.000003 * pow(yPos, 3) + -0.001 * pow(yPos, 2) +
                    0.315 * yPos - 0.557;
                const int distanceToCenterXMagnitude =
                    clamp(160.0f * distanceToCenterX / screenCenterX,
                          -100.f, 100.f);
                cv::putText(maskedBgr,
                            ("Rotate: " +
                             patch::to_string(
                                 distanceToCenterXMagnitude))
                                .c_str(),
                            boundCenter + cv::Point(-65, 85),
                            cv::FONT_HERSHEY_COMPLEX_SMALL, 1,
                            cv::Scalar(0, 0, 255));
                cv::putText(maskedBgr,
                            ("Distance: " +
                             patch::to_string(distanceToGoal))
                                .c_str(),
                            boundCenter + cv::Point(-65, 125),
                            cv::FONT_HERSHEY_COMPLEX_SMALL, 1,
                            cv::Scalar(0, 0, 255));

                // putText(masked_bgr,
                //         ("X: " + patch::to_string(boundCenter.x) +
                //          " Y: " + patch::to_string(boundCenter.y))
                //             .c_str(),
                //         boundCenter + Point(-65, 85),
                //         FONT_HERSHEY_COMPLEX_SMALL, 1,
                //         Scalar(0, 0, 255));

                pugi::xml_node subNode;

                //TODO: Sort the goals by X position on the screen

                //if (shapeCount == 0) {
                    subNode = visionNode.append_child("goal");

                    // visionNode.append_child("FrameInfo")
                    //     .append_attribute("FrameCount")
                    //     .set_value(frameCount);
                    subNode.append_attribute("translation")
                        .set_value((int)transMag);
                    subNode.append_attribute("rotation")
                        .set_value(distanceToCenterXMagnitude);
                    subNode.append_attribute("distance")
                        .set_value(distanceToGoal);
                    // subNode.append_attribute("area").set_value(
                    //     boundRect.width * boundRect.height);
                    subNode.append_attribute("area").set_value(
                        contArea);
                //}

                // xml_node distance_node =
                //     subNode.append_child("Distance");
                // distance_node.set_value(
                //     patch::to_string(boundRect.height).c_str());
            } else {
                //std::cout << patch::to_string(areaRatio) << std::endl;
            }
        }

        if (UI) { cv::imshow("Output", maskedBgr); }

        if (cv::waitKey(10) == ESC) { break; }

        std::stringstream ss;
        sendToRio.save(ss);
        std::string toRep(ss.str());
        std::replace(toRep.begin(), toRep.end(), '\n', ' ');
        toRep.push_back('\n');

        //TODO send XML
        {
            std::lock_guard<std::mutex> lock(mutex);
            socketSendXmlBuffer = std::move(toRep);
        }
    }

    std::cout << "Vision crashed" << std::endl;

    return 0;
}
