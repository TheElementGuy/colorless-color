cmake_minimum_required(VERSION 3.31)
project(colorless_color LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(wxWidgets_USE_STATIC ON)

find_package(wxWidgets CONFIG REQUIRED)

add_executable(colorless_color WIN32 main.cpp stb_impl.cpp stb_wrapper.cpp stb_wrapper.h img_transforms.cpp img_transforms.h app.rc
        transform_runner.cpp
        transform_runner.h
        colorless_color_main_frame.cpp
        colorless_color_main_frame.h)

target_link_libraries(colorless_color PRIVATE wx::base wx::core)