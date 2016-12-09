#include "trackbar.hh"

#include "variables.hh"

#include <pugixml.hpp>

static void onTrackbar(int, void*) {
    pugi::xml_document settings;
    pugi::xml_node sliders = settings.append_child("sliders");
    sliders.append_attribute("HueLw").set_value(hue_slider_lower);
    sliders.append_attribute("HueUp").set_value(hue_slider_upper);
    sliders.append_attribute("SatLw").set_value(sat_slider_lower);
    sliders.append_attribute("SatUp").set_value(sat_slider_upper);
    sliders.append_attribute("ValLw").set_value(val_slider_lower);
    sliders.append_attribute("ValUp").set_value(val_slider_upper);
    sliders.append_attribute("Canny").set_value(canny_slider);
    sliders.append_attribute("Hough").set_value(hough_slider);
    sliders.append_attribute("Morph").set_value(morph_slider);
    sliders.append_attribute("ShapeLw").set_value(shape_slider_lower);
    sliders.append_attribute("ShapeUp").set_value(shape_slider_upper);
    settings.save_file("/home/ubuntu/slidersettings.xml");
}

void createTrackbars() {
    cv::Mat useless(1, 1920 / 3, 0);
    imshow("Sliders", useless);

    //Hue
    cv::createTrackbar("Hue Lw", "Sliders", &hue_slider_lower,
                       slider_max_hue, onTrackbar);
    cv::createTrackbar("Hue Up", "Sliders", &hue_slider_upper,
                       slider_max_hue, onTrackbar);

    // Sat
    cv::createTrackbar("Sat Lw", "Sliders", &sat_slider_lower,
                       slider_max_sat_val, onTrackbar);
    cv::createTrackbar("Sat Up", "Sliders", &sat_slider_upper,
                       slider_max_sat_val, onTrackbar);

    // Val
    cv::createTrackbar("Val Lw", "Sliders", &val_slider_lower,
                       slider_max_sat_val, onTrackbar);
    cv::createTrackbar("Val Up", "Sliders", &val_slider_upper,
                       slider_max_sat_val, onTrackbar);

    // Canny
    cv::createTrackbar("Canny", "Sliders", &canny_slider,
                       slider_max_sat_val, onTrackbar);

    // Hough
    cv::createTrackbar("Hough", "Sliders", &hough_slider,
                       slider_max_sat_val, onTrackbar);

    // Morph
    cv::createTrackbar("Morph", "Sliders", &morph_slider,
                       morph_slider_max, onTrackbar);

    // Goal Shape-detect
    cv::createTrackbar("ShapeLw", "Sliders", &shape_slider_lower,
                       shape_slider_max, onTrackbar);
    cv::createTrackbar("ShapeUp", "Sliders", &shape_slider_upper,
                       shape_slider_max, onTrackbar);
}
