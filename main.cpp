#include <iostream>
#include <functional>
#include <cstdlib>
#include <direct.h>
#include <string>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "stb_image.h"
#include "stb_image_write.h"
#include "SimpleFileDialog.h"

#define MAX_FILENAME_SIZE 200

#define R_BRIGHTNESS_COEF 0.2126
#define G_BRIGHTNESS_COEF 0.7152
#define B_BRIGHTNESS_COEF 0.0722

#define NUM_COLORS 6

using namespace std;

struct rgba {
	int r;
	int g;
	int b;
	int a;
	rgba(const int r, const int g, const int b, const int a = 255) {
		this->r = r;
		this->g = g;
		this->b = b;
		this->a = a;
	}
	rgba() {
		this->r = 0;
		this->g = 0;
		this->b = 0;
		this->a = 0;
	}
	bool operator==(const rgba obj) const {
		return (r == obj.r) && (g == obj.g) && (b == obj.b) && (a == obj.a);
	}
	[[nodiscard]] double brightness() const {
		return r * R_BRIGHTNESS_COEF + g * G_BRIGHTNESS_COEF + b * B_BRIGHTNESS_COEF;
	}
	bool operator<(const rgba& obj) const {
		return brightness() < obj.brightness();
	}
	bool operator>(const rgba& obj) const {
		return brightness() > obj.brightness();
	}
};

struct named_color {
	rgba color;
	char name[80] = "";
	named_color(rgba set_color, const char* name_string) {
		strcpy(name, name_string);
		color = set_color;
	}
};

named_color RED = named_color(rgba(255, 0, 0), "red");
named_color GREEN = named_color(rgba(0, 255, 0), "green");
named_color BLUE = named_color(rgba(0, 0, 255), "blue");

named_color YELLOW = named_color(rgba(255, 255, 0), "yellow");
named_color CYAN = named_color(rgba(0, 255, 255), "cyan");
named_color MAGENTA = named_color(rgba(255, 0, 255), "magenta");

int compare_rgba(const void* first, const void* second) {
	//ooooh, compiler mad!
	const rgba arg1 = *((rgba*) first);
	const rgba arg2 = *((rgba*) second);
	if (arg1 < arg2) {
		return -1;
	}
	if (arg1 > arg2) {
		return 1;
	}
	return 0;
}

void from_raw_dat(const unsigned char* data, int num_pix, rgba* write_into);

class img {
protected:
	rgba* data;
	int pixel_num;
	int width;
	int height;
public:
	img(const unsigned char* raw_dat, const int* width, const int* height) {
		cout << "Constructing image." << endl;
		data = new rgba[*width * *height];
		pixel_num = *width * *height;
		from_raw_dat(raw_dat, pixel_num, data);
		this->width = *width;
		this->height = *height;
	}
	virtual ~img() {
		cout << "Destructing image." << endl;
		delete[] data;
	}
	img(const img& other) {
		cout << "Copy constructing image." << endl;
		pixel_num = other.pixel_num;
		width = other.width;
		height = other.height;
		data = new rgba[pixel_num];
		for (int i = 0; i < pixel_num; i++) {
			data[i] = other.data[i];
		}
	}
	img& operator=(const img& other) {
		cout << "Copy constructing image." << endl;

		if (this == &other) {
			return *this;
		}

		pixel_num = other.pixel_num;
		width = other.width;
		height = other.height;

		delete[] data;

		data = new rgba[pixel_num];
		for (int i = 0; i < pixel_num; i++) {
			data[i] = other.data[i];
		}

		return *this;
	}
	int write(const char* filename) const;
	void transform(function<rgba(rgba)> funct);
	void combine(const img& other) const;
};

struct named_img_transform {
	function<rgba(rgba)> func;
	char name[80] = "";
	named_img_transform(function<rgba(rgba)> def_func, char* transform_name) {
		func = def_func;
		strcpy(name, transform_name);
	}
	named_img_transform() = default;
};

ostream& operator<<(ostream &stream, rgba obj);

ostream& operator<<(ostream &stream, const rgba obj) {
	stream << "Red: " << obj.r << " Green: " << obj.g << " Blue: " << obj.b << " Alpha: " << obj.a;
	return stream;
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

rgba through_filter_p(const rgba filtered, const rgba filter, double transmittance = 0.5) {
	double r = filtered.r / 255.0 * (transmittance + (1 - transmittance) * (filter.r / 255.0));
	double g = filtered.g / 255.0 * (transmittance + (1 - transmittance) * (filter.g / 255.0));
	double b = filtered.b / 255.0 * (transmittance + (1 - transmittance) * (filter.b / 255.0));
	return {(int) (r * 255), (int) (g * 255), (int) (b * 255)};
}

rgba through_filter_l(const rgba filtered, const rgba filter, double transmittance = 0.5) {
	double r = filtered.r / 255.0 * transmittance + (1 - transmittance) * (filter.r / 255.0);
	double g = filtered.g / 255.0 * transmittance + (1 - transmittance) * (filter.g / 255.0);
	double b = filtered.b / 255.0 * transmittance + (1 - transmittance) * (filter.b / 255.0);
	return {(int) (r * 255), (int) (g * 255), (int) (b * 255)};
}

function<rgba(rgba)> create_transparent(rgba filter, double transmittance = 0.5) {
	return [filter, transmittance](rgba in) -> rgba {return through_filter_p(in, filter, transmittance);};
}

function<rgba(rgba)> create_translucent(rgba filter, double transmittance = 0.5) {
	return [filter, transmittance](rgba in) -> rgba {return through_filter_l(in, filter, transmittance);};
}

void img::transform(function<rgba(rgba)> funct) {

	for (int i = 0; i < pixel_num; i++) {
		data[i] = funct(data[i]);
	}

}

rgba rgba_combine(rgba first, rgba second) {
	return rgba((first.r + second.r) / 2, (first.g + second.g) / 2, (first.b + second.b) / 2, (first.a + second.a) / 2);
}

void img::combine(const img& other) const {

	const rgba* other_data = other.data;

	for (int i = 0; i < pixel_num; i++) {
		data[i] = rgba_combine(data[i], other_data[i]);
	}

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
		double intensity = (double) (in.r * filter1.r + in.g * filter1.g + in.b * filter1.b) / (filter1.r * filter1.r + filter1.g * filter1.g + filter1.b * filter1.b);
		rgba other = color_based_grayscale(in, filter2);
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

void to_raw_dat(rgba* data, int num_pix, unsigned char* write_into) {
	for (int i = 0; i < num_pix; i++) {
		write_into[4 * i] = data[i].r;
		write_into[4 * i + 1] = data[i].g;
		write_into[4 * i + 2] = data[i].b;
		write_into[4 * i + 3] = data[i].a;
	}
}

void from_raw_dat(const unsigned char* data, int num_pix, rgba* write_into) {
	for (int i = 0; i < num_pix; i++) {
		write_into[i] = rgba(data[4 * i], data[4 * i + 1], data[4 * i + 2], data[4 * i + 3]);
	}
}

int img::write(const char* filename) const {
	unsigned char* raw_data = new unsigned char[pixel_num * 4];
	to_raw_dat(data, pixel_num, raw_data);

	int to_return = stbi_write_png(filename, width, height, 4, raw_data, 0);

	delete[] raw_data;
	cout << "Write exit code: " << to_return << endl;
	return to_return;
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

void parse_filename(char* filename, char* clean) {
	char next_filename[MAX_FILENAME_SIZE];
	char* final = strrchr(filename, '\\') + 1;
	if (!final) {
		return;
	}
	int i = 0;
	while (*final) {
		next_filename[i] = *final;
		i++;
		final++;
	}
	next_filename[i] = '\0';
	strcpy(clean, next_filename);
}

int main(int argc, char* argv[]) {

	char filename[MAX_FILENAME_SIZE];

	if (argc < 2) {
		cout << "Enter file name:" << endl;
		strcpy(filename, rapidpixel::SimpleFileDialog::dialog().c_str());
	} else {
		strcpy(filename, argv[1]);
	}

	cout << "Filename: " << filename << endl;

	bool open = false;

	for (int i = 0; i < argc; i++) {
		if (strcmp(argv[i], "-o") == 0) {
			open = true;
		}
	}

	char foldername[MAX_FILENAME_SIZE];
	strncpy(foldername, filename, strlen(filename) - 4);
	foldername[strlen(filename) - 4] = '\0';

	char parsed_filename[MAX_FILENAME_SIZE];
	parse_filename(filename, parsed_filename);

	_mkdir(foldername);

	int width, height, num_channels;
	//put image in cmake-build-debug folder
	unsigned char* data = stbi_load(filename, &width, &height, &num_channels, 4);

	if (data) {
		cout << "Congratulations! File loaded! :)" << endl;
		cout << "Width: " << width << " Height: " << height << " Channels: " << num_channels << endl;
		const img working = img(data, &width, &height);

		named_color all_colors[NUM_COLORS] = {RED, GREEN, BLUE, YELLOW, CYAN, MAGENTA};

		named_img_transform transforms[NUM_COLORS * 6] = {};

		int index = 0;
		for (int i = 0; i < NUM_COLORS; i++) {
			for (int j = 0; j < NUM_COLORS; j++) {
				if (i == j) {
					transforms[index] = nit_of_translucent(all_colors[i]);
				} else {
					transforms[index] = nit_of_adjusted_lands(all_colors[i], all_colors[j]);
				}
				index++;
			}
		}

		for (int i = 0; i < sizeof(transforms) / sizeof(named_img_transform); i++) {
			cout << "Running: " << transforms[i].name << endl;
			img to_convert(working);
			to_convert.transform(transforms[i].func);
			char new_filename[MAX_FILENAME_SIZE + 10];
			strcpy(new_filename, foldername);
			strcat(new_filename, "\\");
			strcat(new_filename, transforms[i].name);
			strcat(new_filename, "_");
			strcat(new_filename, parsed_filename);
			cout << "Writing..." << endl;
			if (to_convert.write(new_filename) != 1) {
				cerr << "Error writing image." << endl;
			}
#ifdef WIN32
			if (open) {
				char command[strlen(new_filename) + 10];
				strcpy(command, "start ");
				strcat(command, new_filename);
				system(command);
			}
			char folder_command[strlen(foldername) + 10];
			strcpy(folder_command, "start ");
			strcat(folder_command, foldername);
			system(folder_command);
#endif
		}
	} else {
		cerr << "Gosh diddly darn it, the file didn't load." << endl;
		return -2;
	}

	stbi_image_free(data);

	return 0;
}