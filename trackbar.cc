#include "trackbar.hh"

#include "variables.hh"

#include <pugixml.hpp>

static void onTrackbar(int, void*) {
    pugi::xml_document settings;
    pugi::xml_node sliders = settings.append_child("sliders");
    sliders.append_attribute("HueLw").set_value(hueSliderLower);
    sliders.append_attribute("HueUp").set_value(hueSliderUpper);
    sliders.append_attribute("SatLw").set_value(satSliderLower);
    sliders.append_attribute("SatUp").set_value(satSliderUpper);
    sliders.append_attribute("ValLw").set_value(valSliderLower);
    sliders.append_attribute("ValUp").set_value(valSliderUpper);
    sliders.append_attribute("Canny").set_value(cannySlider);
    sliders.append_attribute("Hough").set_value(houghSlider);
    sliders.append_attribute("Morph").set_value(morphSlider);
    sliders.append_attribute("ShapeLw").set_value(shapeSliderLower);
    sliders.append_attribute("ShapeUp").set_value(shapeSliderUpper);
    settings.save_file("/home/ubuntu/slidersettings.xml");
}

void createTrackbars() {
    cv::Mat useless(1, 1920 / 3, 0);
    imshow("Sliders", useless);

    //Hue
    cv::createTrackbar("Hue Lw", "Sliders", &hueSliderLower,
                       sliderMaxHue, onTrackbar);
    cv::createTrackbar("Hue Up", "Sliders", &hueSliderUpper,
                       sliderMaxHue, onTrackbar);

    // Sat
    cv::createTrackbar("Sat Lw", "Sliders", &satSliderLower,
                       sliderMaxSatVal, onTrackbar);
    cv::createTrackbar("Sat Up", "Sliders", &satSliderUpper,
                       sliderMaxSatVal, onTrackbar);

    // Val
    cv::createTrackbar("Val Lw", "Sliders", &valSliderLower,
                       sliderMaxSatVal, onTrackbar);
    cv::createTrackbar("Val Up", "Sliders", &valSliderUpper,
                       sliderMaxSatVal, onTrackbar);

    // Canny
    cv::createTrackbar("Canny", "Sliders", &cannySlider,
                       sliderMaxSatVal, onTrackbar);

    // Hough
    cv::createTrackbar("Hough", "Sliders", &houghSlider,
                       sliderMaxSatVal, onTrackbar);

    // Morph
    cv::createTrackbar("Morph", "Sliders", &morphSlider,
                       morphSliderMax, onTrackbar);

    // Goal Shape-detect
    cv::createTrackbar("ShapeLw", "Sliders", &shapeSliderLower,
                       shapeSliderMax, onTrackbar);
    cv::createTrackbar("ShapeUp", "Sliders", &shapeSliderUpper,
                       shapeSliderMax, onTrackbar);
}
