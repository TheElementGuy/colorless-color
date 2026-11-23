#include "img_transforms.h"

#include <cstring>
#include "stb_wrapper.h"

#define IMG_TRANSFORM_MAX_FILENAME_SIZE 5000

named_color::named_color(rgba set_color, const char* name_string) {
	strcpy(name, name_string);
	color = set_color;
}

named_img_transform::named_img_transform(function<rgba(rgba)> def_func, char* transform_name) {
	func = def_func;
	strcpy(name, transform_name);
}

rgba grayscale(const rgba to_gs) {
	return rgba((int) to_gs.brightness(), to_gs.brightness(), to_gs.brightness());
}

rgba reddify_green(const rgba to_rf) {
	int base = to_rf.r;
	int channel = to_rf.g;
	int channel_scaled = channel;
	return rgba((base + channel_scaled) / 2, channel_scaled/2, channel_scaled/2);
}

rgba reddify_blue(const rgba to_rf) {
	int base = to_rf.r;
	int channel = to_rf.b;
	int channel_scaled = channel;
	return rgba((base + channel_scaled) / 2, channel_scaled/2, channel_scaled/2);
}

rgba greenify_red(const rgba to_gf) {
	int base = to_gf.g;
	int channel = to_gf.r;
	int channel_scaled = channel;
	return rgba(channel_scaled/2, (base + channel_scaled) / 2, channel_scaled/2);
}

rgba greenify_blue(const rgba to_gf) {
	int base = to_gf.g;
	int channel = to_gf.b;
	int channel_scaled = channel;
	return rgba(channel_scaled/2, (base + channel_scaled) / 2, channel_scaled/2);
}

rgba blueify_red(const rgba to_bf) {
	int base = to_bf.b;
	int channel = to_bf.r;
	int channel_scaled = channel;
	return rgba(channel_scaled/2, channel_scaled/2, (base + channel_scaled) / 2);
}

rgba blueify_green(const rgba to_bf) {
	int base = to_bf.b;
	int channel = to_bf.g;
	int channel_scaled = channel;
	return rgba(channel_scaled/2, channel_scaled/2, (base + channel_scaled) / 2);
}

rgba through_filter_p(const rgba filtered, const rgba filter, double transmittance) {
	double r = filtered.r / 255.0 * (transmittance + (1 - transmittance) * (filter.r / 255.0));
	double g = filtered.g / 255.0 * (transmittance + (1 - transmittance) * (filter.g / 255.0));
	double b = filtered.b / 255.0 * (transmittance + (1 - transmittance) * (filter.b / 255.0));
	return {(int) (r * 255), (int) (g * 255), (int) (b * 255)};
}

rgba through_filter_l(const rgba filtered, const rgba filter, double transmittance) {
	double r = filtered.r / 255.0 * transmittance + (1 - transmittance) * (filter.r / 255.0);
	double g = filtered.g / 255.0 * transmittance + (1 - transmittance) * (filter.g / 255.0);
	double b = filtered.b / 255.0 * transmittance + (1 - transmittance) * (filter.b / 255.0);
	return {(int) (r * 255), (int) (g * 255), (int) (b * 255)};
}

function<rgba(rgba)> create_transparent(rgba filter, double transmittance) {
	return [filter, transmittance](rgba in) -> rgba {return through_filter_p(in, filter, transmittance);};
}

function<rgba(rgba)> create_translucent(rgba filter, double transmittance) {
	return [filter, transmittance](rgba in) -> rgba {return through_filter_l(in, filter, transmittance);};
}

rgba color_based_grayscale(rgba in, rgba filter) {
	int intensity = (in.r * filter.r + in.g * filter.g + in.b * filter.b) * 255 / (filter.r * filter.r + filter.g * filter.g + filter.b * filter.b);
	return {intensity, intensity, intensity};
}

function<rgba(rgba)> of_lands(rgba filter1, rgba filter2) {
	return [filter1, filter2](rgba in) -> rgba {
		rgba filtered1 = through_filter_p(in, filter1, 0);
		//rgba filtered2 = through_filter_p(in, filter2, 0);
		rgba gray = color_based_grayscale(in, filter2);
		return {rgba_combine(filtered1, gray)};
	};
}

function<rgba(rgba)> adjusted_lands(rgba filter1, rgba filter2) {
	return [filter1, filter2](rgba in) -> rgba {
		rgba filter1in = through_filter_p(in, filter1, 0);
		double intensity = (double) (filter1in.r * filter1.r + filter1in.g * filter1.g + filter1in.b * filter1.b) / (filter1.r * filter1.r + filter1.g * filter1.g + filter1.b * filter1.b);
		rgba other = color_based_grayscale(through_filter_p(in, filter2, 0), filter2);
		rgba filter1gray = rgba(intensity * filter1.r, intensity * filter1.g, intensity * filter1.b);
		return {rgba_combine(filter1gray, other)};
	};
}

named_img_transform nit_of_lands(named_color filter1, named_color filter2) {
	char name_to_return[80] = "lands_";
	strcat(name_to_return, filter1.name);
	strcat(name_to_return, "ify-with");
	strcat(name_to_return, filter2.name);
	return named_img_transform(of_lands(filter1.color, filter2.color), name_to_return);
}

named_img_transform nit_of_adjusted_lands(named_color filter1, named_color filter2) {
	char name_to_return[80] = "lands_";
	strcat(name_to_return, filter1.name);
	strcat(name_to_return, "ify-adjusted-with");
	strcat(name_to_return, filter2.name);
	return named_img_transform(adjusted_lands(filter1.color, filter2.color), name_to_return);
}

named_img_transform nit_of_translucent(named_color filter) {
	char name_to_return[80] = "filter_l_";
	strcat(name_to_return, filter.name);
	return named_img_transform(create_translucent(filter.color), name_to_return);
}

named_img_transform nit_of_transparent(named_color filter) {
	char name_to_return[80] = "filter_p_";
	strcat(name_to_return, filter.name);
	return named_img_transform(create_transparent(filter.color), name_to_return);
}

named_img_transform compose(named_img_transform first, named_img_transform second) {
	char name[100];
	strcpy(name, first.name);
	strcat(name, "_then_");
	strcat(name, second.name);
	return named_img_transform([first, second](rgba in) -> rgba {
		return second.func(first.func(in));
	}, name);
}

/*named_img_transform enhanced_lands_adjusted(named_color filter1, named_color filter2) {
	return compose(nit_of_adjusted_lands(filter1, filter2), nit_of_translucent(filter1));
}*/
//sucks

rgba histogram_compression(rgba color_in, int rmin, int rmax, int gmin, int gmax, int bmin, int bmax) {
	return {rmin + ((rmax - rmin) * color_in.r) / 255, gmin + ((gmax - gmin) * color_in.g) / 255, bmin + ((bmax - bmin) * color_in.b) / 255};
}

named_img_transform nit_of_compression(named_color filter) {
	char transform_name[IMG_TRANSFORM_MAX_FILENAME_SIZE] = "histcompress_";
	strcat(transform_name, filter.name);
	return named_img_transform([filter](rgba in) -> rgba {
		rgba complement = filter.color.complement();
		int r1 = filter.color.r;
		int r2 = complement.r;
		int g1 = filter.color.g;
		int g2 = complement.g;
		int b1 = filter.color.b;
		int b2 = complement.b;
		int rmin, rmax, gmin, gmax, bmin, bmax;
		if (r1 > r2) {
			rmin = r2 / 2 + 128;
			rmax = r1 / 2 + 128;
		} else {
			rmin = r1 / 2;
			rmax = r2 / 2;
		}
		if (g1 > g2) {
			gmin = g2 / 2 + 128;
			gmax = g1 / 2 + 128;
		} else {
			gmin = g1 / 2;
			gmax = g2 / 2;
		}
		if (b1 > b2) {
			bmin = b2 / 2 + 128;
			bmax = b1 / 2 + 128;
		} else {
			bmin = b1 / 2;
			bmax = b2 / 2;
		}
		return histogram_compression(in, rmin, rmax, gmin, gmax, bmin, bmax);
	}, transform_name);
}