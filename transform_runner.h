#ifndef COLORLESS_COLOR_TRANSFORM_RUNNER_H
#define COLORLESS_COLOR_TRANSFORM_RUNNER_H

#include "transform_runner.h"
#include <wx/wx.h>
#include <wx/filename.h>

#include "stb_wrapper.h"

#include <comcat.h>
#include <thread>
#include <cstdlib>

#include "img_transforms.h"

#ifdef _WIN32
#include <direct.h>
#else
#include <unistd.h>
#endif

#define MAX_FILENAME_SIZE 5000

#define R_BRIGHTNESS_COEF 0.2126
#define G_BRIGHTNESS_COEF 0.7152
#define B_BRIGHTNESS_COEF 0.0722

#define NUM_COLORS 8

class colorless_colors_file_select_frame;

void parse_filename(const char* filename, char* clean);

int get_new_dimension(int orig, int window, int other_orig, int other_window);

void run_transform(char* filename, colorless_colors_file_select_frame* frame, const bool open_images_flag);

#endif //COLORLESS_COLOR_TRANSFORM_RUNNER_H
