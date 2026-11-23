#ifndef COLORLESS_COLOR_IMG_TRANSFORMS_H
#define COLORLESS_COLOR_IMG_TRANSFORMS_H

#define NUM_COLORS 8

#include "stb_wrapper.h"

struct named_color {
	rgba color;
	char name[80] = "";
	named_color(rgba set_color, const char* name_string);
};

struct named_img_transform {
	function<rgba(rgba)> func;
	char name[80] = "";
	named_img_transform(function<rgba(rgba)> def_func, char* transform_name);
	named_img_transform() = default;
};

rgba grayscale(rgba to_gs);

rgba reddify_green(rgba to_rf);

rgba reddify_blue(rgba to_rf);

rgba greenify_red(rgba to_gf);

rgba greenify_blue(rgba to_gf);

rgba blueify_red(rgba to_bf);

rgba blueify_green(rgba to_bf);

rgba through_filter_p(rgba filtered, rgba filter, double transmittance = 0.5);

rgba through_filter_l(rgba filtered, rgba filter, double transmittance = 0.5);

function<rgba(rgba)> create_transparent(rgba filter, double transmittance = 0.5);

function<rgba(rgba)> create_translucent(rgba filter, double transmittance = 0.5);

rgba color_based_grayscale(rgba in, rgba filter);

function<rgba(rgba)> of_lands(rgba filter1, rgba filter2);

function<rgba(rgba)> adjusted_lands(rgba filter1, rgba filter2);

named_img_transform nit_of_lands(named_color filter1, named_color filter2);

named_img_transform nit_of_adjusted_lands(named_color filter1, named_color filter2);

named_img_transform nit_of_translucent(named_color filter);

named_img_transform nit_of_transparent(named_color filter);

named_img_transform compose(named_img_transform first, named_img_transform second);

/*named_img_transform enhanced_lands_adjusted(named_color filter1, named_color filter2) {
	return compose(nit_of_adjusted_lands(filter1, filter2), nit_of_translucent(filter1));
}*/
//sucks

rgba histogram_compression(rgba color_in, int rmin, int rmax, int gmin, int gmax, int bmin, int bmax);

named_img_transform nit_of_compression(named_color filter);

inline const named_color RED = named_color(rgba(255, 0, 0), "red");
inline const named_color GREEN = named_color(rgba(0, 255, 0), "green");
inline const named_color BLUE = named_color(rgba(0, 0, 255), "blue");

inline const named_color YELLOW = named_color(rgba(255, 255, 0), "yellow");
inline const named_color CYAN = named_color(rgba(0, 255, 255), "cyan");
inline const named_color MAGENTA = named_color(rgba(255, 0, 255), "magenta");

inline const named_color ORANGE = named_color(rgba(255, 165, 0), "orange");
inline const named_color PURPLE = named_color(rgba(128, 0, 128), "purple");

inline const named_color ALL_COLORS[NUM_COLORS] = {RED, GREEN, BLUE, YELLOW, CYAN, MAGENTA, ORANGE, PURPLE};

#endif //COLORLESS_COLOR_IMG_TRANSFORMS_H