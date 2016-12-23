cmake_minimum_required(VERSION 2.6)
project(CV_VISION_2016)

include_directories(${CV_VISION_2016})
link_directories(${CV_VISION_2016})

file(GLOB sources "*.cc")
file(GLOB headers "*.hh")

add_executable(cv_vision_2016 ${sources} ${headers})

target_compile_features(cv_vision_2016 PRIVATE cxx_range_for)

target_link_libraries(cv_vision_2016 opencv2)
