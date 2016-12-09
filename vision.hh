#ifndef HEADER_GUARD_VISION_H
#define HEADER_GUARD_VISION_H

#include "to_string.hh"
#include "variables.hh"
#include "socket.hh"
#include "calculations.hh"
#include "line_info.hh"
#include "trackbar.hh"

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

LineInfo getInfo(const cv::Vec4i& line) {
    LineInfo result;
    result.line = line;
    result.length = calcLength(line);
    result.slope = calcSlope(line);
    result.midPt = calcMidPt(line);
    return result;
}

void netcode_thread() {
    std::cout << "Netcode booting..." << std::endl;

    //http://www.cs.rpi.edu/~moorthy/Courses/os98/Pgms/client.c

    struct sockaddr_in serv_addr;
    struct hostent* server = gethostbyname(host_name);

    while (server == NULL) {
        std::cout << "No such host" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        server = gethostbyname("roborio-955-frc.local");
    }
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    memcpy(server->h_addr, &serv_addr.sin_addr.s_addr,
           server->h_length);
    //Convert small endian/big endian
    serv_addr.sin_port = htons(portno);

    while (true) {
        Socket socket;
        char buf[2048];

        if (socket.connect(serv_addr) < 0) {
            std::cout << "Connect error" << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            continue;
        }

        std::cout << "Connected to RIO\n" << std::endl;

        //Loop to handle all requests
        while (true) {
            //Read a request
            //memset(buf,0,strlen(buf));

            ssize_t nread = socket.read(buf, sizeof(buf));

            if (nread > 0) {
                //printf("%d\n", nread);
                buf[nread] = 0;
                std::cout << "Got Request: " << buf << std::endl;
            } else {
                break;
            }

            //Send a response
            size_t buflen;
            {
                std::lock_guard<std::mutex> lock(mutex);
                buflen = socket_send_xml_buffer.size();
                memcpy(buf, socket_send_xml_buffer.c_str(), buflen);
            }
            socket.send(buf, buflen);
            buf[buflen] = 0;
            std::cout << "Sent: " << buf << std::endl;
        }

        std::cout << "Disconnected\n" << std::endl;
    }

    std::cout << "Client loop failed\n" << std::endl;
}

int main(int numArgs, char**) {
    UI = (numArgs == 2); //Set in UI mode if there is an argument

    std::cout << "Vision Starting..." << std::endl;
    pugi::xml_document settings;
    try {
        if (pugi::xml_parse_result result = settings.load_file(
                "/home/ubuntu/slidersettings.xml")) {
        } else {
            std::cerr << "ERROR loading XML document "
                      << result.description() << std::endl;
            return EXIT_FAILURE;
        }

        pugi::xml_node settingRoot = settings.child("sliders");
        hue_slider_lower = settingRoot.attribute("HueLw").as_int();
        hue_slider_upper = settingRoot.attribute("HueUp").as_int();
        sat_slider_lower = settingRoot.attribute("SatLw").as_int();
        sat_slider_upper = settingRoot.attribute("SatUp").as_int();
        val_slider_lower = settingRoot.attribute("ValLw").as_int();
        val_slider_upper = settingRoot.attribute("ValUp").as_int();
        canny_slider = settingRoot.attribute("Canny").as_int();
        hough_slider = settingRoot.attribute("Hough").as_int();
        morph_slider = settingRoot.attribute("Morph").as_int();
        shape_slider_lower = settingRoot.attribute("ShapeLw").as_int();
        shape_slider_upper = settingRoot.attribute("ShapeUp").as_int();
    } catch (int e) {
        std::cout << "Slider XML parse failed" << std::endl;
    }

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

    std::thread other(netcode_thread);
    other.detach();

    std::cout << "Start vision loop" << std::endl;

    while(true) {
        if (cv::waitKey(1) == ESC) break;

        frameCount++;
        cap >> src; // get a new frame from camera
        if (src.empty()) break;

        //imshow("Source", src);

        //Convert to HSV colorspace
        cvtColor(src, hsv_src, cv::COLOR_BGR2HSV);
        //HSV Threshold
        cv::inRange(hsv_src,
                    cv::Scalar(hue_slider_lower, sat_slider_lower,
                       val_slider_lower),
                    cv::Scalar(hue_slider_upper, sat_slider_upper,
                       val_slider_upper),
                hsv_filtered);
        //Erase old data from last render (background to the HSV threshhold)
        masked_bgr.setTo(cv::Scalar(0,0,0,0));

        // Make sure that objects have a certain area
        cv::Mat element =
            getStructuringElement(cv::MORPH_RECT,
                                  cv::Size(2 * morph_slider + 1,
                                       2 * morph_slider + 1),
                                  cv::Point(morph_slider, morph_slider));
        morphologyEx(hsv_filtered, hsv_filtered, cv::MORPH_OPEN, element);
        //Mask the final image by the HSV selection
        src.copyTo(masked_bgr, hsv_filtered);

        cv::Mat contour_hsv_filtered_copy = hsv_filtered.clone();
        findContours(contour_hsv_filtered_copy, contours,
                     CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE,
                     cv::Point(0, 0));

        std::vector<std::vector<cv::Point> > hull(contours.size());

        pugi::xml_document send_to_RIO;

        pugi::xml_node visionNode = send_to_RIO.append_child("Vision");
        visionNode.append_attribute("frameNumber").set_value(frameCount);

        u_int shapeCount = 0;

        for (std::size_t idx = 0; idx < contours.size(); idx++) {
            cv::Rect boundRect = boundingRect(contours[idx]); // boundingbox
            cv::Point boundCenter =
                cv::Point(boundRect.width / 2, boundRect.height / 2) +
                boundRect.tl();

            convexHull(cv::Mat(contours[idx]), hull[0], false);
            //drawContours(masked_bgr, boundRect, idx, Scalar(0, 0, 255));
            //drawContours(masked_bgr, contours, idx, Scalar(0, 255, 255), 1, 8, noArray(), 0, Point());

            //Find the U shape
            const double hullArea = contourArea(hull[0]);
            const double contArea = contourArea(contours[idx]);
            //double perimeter = arcLength(contours[idx], false);
            const double areaRatio = hullArea / contArea;
            //std::cout << patch::to_string(areaRatio) << std::endl;

            if (shape_slider_lower / 10.0f < areaRatio &&
                areaRatio < shape_slider_upper / 10.0f /*4.5*/) {
                shapeCount++;
                rectangle(masked_bgr, boundRect,
                          cv::Scalar(0, 255, 255), 2);
                rect_ROI = hsv_filtered(boundRect);
                if (canny_slider == 0) { canny_slider = 1; }
                Canny(rect_ROI, rect_ROI_canny, canny_slider, 0, 3);

                //imshow("Canny", rect_ROI_canny);

                std::vector<cv::Vec4i> lines;
                HoughLinesP(rect_ROI_canny, lines, 1, CV_PI / 180,
                            canny_slider, canny_slider, 10);

                std::vector<LineInfo> lineInfoCache(lines.size());
                LineInfo line_exclude = getInfo(cv::Vec4i(0,0,0,0));
                for (std::size_t i = 0; i < lines.size(); i++) {
                    LineInfo current = getInfo(lines[i]);
                    // Exclude slopes > 45 degrees
                    if (current.slope < 1.0 && current.slope > -1.0) {
                        lineInfoCache[i] = current;
                    } else {
                        lineInfoCache[i] = line_exclude;
                    }
                }

                std::sort(lineInfoCache.begin(), lineInfoCache.end(),
                          [](const LineInfo& a, const LineInfo& b) {
                              return a.length < b.length;
                          });

                double transMag = 0.0;
                for (size_t i = 0; i < lines.size(); i++) {
                    const cv::Vec4i& l = lines[i];
                    if (calcLength(l) == lineInfoCache[0].length) {
                        line(masked_bgr,
                             cv::Point(l[0], l[1]) + boundRect.tl(),
                             cv::Point(l[2], l[3]) + boundRect.tl(),
                             cv::Scalar(255, 0, 255), 2, CV_AA);
                        double degrees =
                            (atan2(static_cast<double>(l[3]) -
                                   static_cast<double>(l[1]),
                                   static_cast<double>(l[2]) -
                                   static_cast<double>(l[0])) *
                             180.0) /
                            CV_PI;
                        transMag = degrees * (100.0 / 45.0);
                        putText(masked_bgr,
                                ("Translate: " +
                                 patch::to_string((int)transMag))
                                    .c_str(),
                                boundCenter + cv::Point(-65, 105),
                                cv::FONT_HERSHEY_COMPLEX_SMALL, 1,
                                cv::Scalar(0, 0, 255));
                    }
                }

                circle(masked_bgr, boundCenter, 5, cv::Scalar(0, 255, 0),
                       1, 8, 0);
                // double rotateMag = (((double)boundCenter.x /
                // (double)pixelWidth) - 0.5) * 200.0;
                const int screenCenterX = (pixelWidth / 2);
                const int distanceToCenterX =
                    screenCenterX -
                    (boundCenter.x + (boundRect.width / 2));
                int distanceToCenterXMagnitude =
                    (static_cast<float>(distanceToCenterX) /
                     static_cast<float>(screenCenterX)) *
                    160.0f;
                const int yPos = boundCenter.y;
                const double distanceToGoal = (3e-6 * pow(yPos, 3)) +
                                        (-1e-3 * pow(yPos, 2)) +
                                        (0.315 * yPos) - 0.557;
                if (distanceToCenterXMagnitude > 100.0) {
                    distanceToCenterXMagnitude = 100.0;
                }
                if (distanceToCenterXMagnitude < -100.0) {
                    distanceToCenterXMagnitude = -100.0;
                }
                putText(masked_bgr,
                        ("Rotate: " +
                         patch::to_string(distanceToCenterXMagnitude))
                            .c_str(),
                        boundCenter + cv::Point(-65, 85),
                        cv::FONT_HERSHEY_COMPLEX_SMALL, 1,
                        cv::Scalar(0, 0, 255));
                putText(masked_bgr, ("Distance: " +
                                     patch::to_string(distanceToGoal))
                                        .c_str(),
                        boundCenter + cv::Point(-65, 125),
                        cv::FONT_HERSHEY_COMPLEX_SMALL, 1,
                        cv::Scalar(0, 0, 255));
                // putText(masked_bgr, ("X: " +
                // patch::to_string(boundCenter.x) + " Y: " +
                // patch::to_string(boundCenter.y)).c_str(),
                // boundCenter + Point(-65, 85),
                // FONT_HERSHEY_COMPLEX_SMALL, 1, Scalar(0, 0, 255));

                pugi::xml_node subNode;

                //TODO: Sort the goals by X position on the screen

                //if (shapeCount == 0) {
                    subNode = visionNode.append_child("goal");

                    // visionNode.append_child("FrameInfo").append_attribute("FrameCount").set_value(frameCount);
                    subNode.append_attribute("translation")
                        .set_value((int)transMag);
                    subNode.append_attribute("rotation")
                        .set_value(distanceToCenterXMagnitude);
                    subNode.append_attribute("distance")
                        .set_value(distanceToGoal);
                    // subNode.append_attribute("area").set_value(boundRect.width
                    // * boundRect.height);
                    subNode.append_attribute("area").set_value(
                        contArea);
                //}

                // xml_node distance_node =
                // subNode.append_child("Distance");
                // distance_node.set_value(patch::to_string(boundRect.height).c_str());
            } else {
                //std::cout << patch::to_string(areaRatio) << std::endl;
            }
        }

        if (UI) imshow("Output", masked_bgr);

        if (cv::waitKey(10) == ESC) break;

        std::stringstream ss;
        send_to_RIO.save(ss);
        std::string toRep(ss.str());
        std::replace(toRep.begin(), toRep.end(), '\n', ' ');
        toRep.push_back('\n');

        //TODO send XML
        {
            std::lock_guard<std::mutex> lock(mutex);
            socket_send_xml_buffer = std::move(toRep);
        }
    }

    std::cout << "Vision crashed" << std::endl;

    return 0;
}

#endif
