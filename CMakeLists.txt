cmake_minimum_required(VERSION 3.31)
project(colorless_color VERSION 2.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES main.cpp stb_impl.cpp stb_wrapper.cpp stb_wrapper.h img_transforms.cpp img_transforms.h transform_runner.cpp transform_runner.h colorless_color_main_frame.cpp colorless_color_main_frame.h)

set(APP_FULL_VERSION "${PROJECT_VERSION}.${APP_BUILD_NUMBER}")
set(APP_OUTPUT_NAME "${PROJECT_NAME}-${PROJECT_VERSION}")

if(WIN32)
    set(APP_OUTPUT_NAME "${PROJECT_NAME}-${PROJECT_VERSION}-windows")
elseif (APPLE)
    set(APP_OUTPUT_NAME "${PROJECT_NAME}-${PROJECT_VERSION}-apple")
elseif (UNIX)
    set(APP_OUTPUT_NAME "${PROJECT_NAME}-${PROJECT_VERSION}-linux")
else()
    set(APP_OUTPUT_NAME "${PROJECT_NAME}-${PROJECT_VERSION}")
endif()

set(APP_OUTPUT_FILENAME "${APP_OUTPUT_NAME}${CMAKE_EXECUTABLE_SUFFIX}")

if(WIN32)
    set(APP_BUILD_NUMBER 0)
    set(APP_GIT_HASH "unknown")

    find_package(Git QUIET)
    if(GIT_FOUND AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/.git")
        if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/.git/logs/HEAD")
            set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/.git/logs/HEAD")
        endif()

        execute_process(COMMAND ${GIT_EXECUTABLE} rev-list --count HEAD WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} OUTPUT_VARIABLE GIT_COMMIT_COUNT OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET
        )
        execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} OUTPUT_VARIABLE GIT_SHORT_HASH OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET
        )
        if(GIT_COMMIT_COUNT)
            set(APP_BUILD_NUMBER ${GIT_COMMIT_COUNT})
        endif()
        if(GIT_SHORT_HASH)
            set(APP_GIT_HASH ${GIT_SHORT_HASH})
        endif()
    endif()

    set(APP_FULL_VERSION "${PROJECT_VERSION}.${APP_BUILD_NUMBER}")

    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(APP_FILEFLAGS "0x1L")
    else()
        set(APP_FILEFLAGS "0x0L")
    endif()


    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/app.rc.in" "${CMAKE_CURRENT_BINARY_DIR}/app.rc" @ONLY)
    list(APPEND SOURCES "${CMAKE_CURRENT_BINARY_DIR}/app.rc")
endif()

add_executable(colorless_color WIN32 MACOSX_BUNDLE ${SOURCES})

set_target_properties(colorless_color PROPERTIES OUTPUT_NAME "${APP_OUTPUT_NAME}")

if(WIN32)
    set(wxWidgets_USE_STATIC ON)
    find_package(wxWidgets CONFIG REQUIRED)
    target_link_libraries(colorless_color PRIVATE wx::base wx::core)

    if(MSVC)
        target_link_options(colorless_color PRIVATE /MANIFEST:NO)
    endif()
else()
    find_package(wxWidgets REQUIRED COMPONENTS core base)
    include(${wxWidgets_USE_FILE})
    target_link_libraries(colorless_color PRIVATE ${wxWidgets_LIBRARIES})
endif()