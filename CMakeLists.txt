cmake_minimum_required(VERSION 3.31)
project(colorless_color LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Bundle source files
set(SOURCES main.cpp stb_impl.cpp stb_wrapper.cpp stb_wrapper.h img_transforms.cpp img_transforms.h transform_runner.cpp transform_runner.h colorless_color_main_frame.cpp colorless_color_main_frame.h)

if(WIN32)
    list(APPEND SOURCES app.rc)
endif()

add_executable(colorless_color WIN32 MACOSX_BUNDLE ${SOURCES})

if(WIN32)
    set(wxWidgets_USE_STATIC ON)
    find_package(wxWidgets CONFIG REQUIRED)
    target_link_libraries(colorless_color PRIVATE wx::base wx::core)
else()
    find_package(wxWidgets REQUIRED COMPONENTS core base)
    include(${wxWidgets_USE_FILE})
    target_link_libraries(colorless_color PRIVATE ${wxWidgets_LIBRARIES})
endif()
