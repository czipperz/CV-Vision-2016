#include "parse_settings.hh"
#include "variables.hh"
#include <iostream>
#include <cstdlib>

void parse_settings(pugi::xml_document& settings) {
    try {
        if (const pugi::xml_parse_result result = settings.load_file(
                "/home/ubuntu/slidersettings.xml")) {
        } else {
            std::cerr << "ERROR loading XML document "
                      << result.description() << std::endl;
            std::exit(EXIT_FAILURE);
        }

        pugi::xml_node settingRoot = settings.child("sliders");
        hueSliderLower = settingRoot.attribute("HueLw").as_int();
        hueSliderUpper = settingRoot.attribute("HueUp").as_int();
        satSliderLower = settingRoot.attribute("SatLw").as_int();
        satSliderUpper = settingRoot.attribute("SatUp").as_int();
        valSliderLower = settingRoot.attribute("ValLw").as_int();
        valSliderUpper = settingRoot.attribute("ValUp").as_int();
        cannySlider = settingRoot.attribute("Canny").as_int();
        houghSlider = settingRoot.attribute("Hough").as_int();
        morphSlider = settingRoot.attribute("Morph").as_int();
        shapeSliderLower = settingRoot.attribute("ShapeLw").as_int();
        shapeSliderUpper = settingRoot.attribute("ShapeUp").as_int();
    } catch (int e) {
        std::cout << "Slider XML parse failed" << std::endl;
    }
}
