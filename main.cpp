#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <functional>

#include "stb_image.h"
#include "stb_image_write.h"

#define R_BRIGHTNESS_COEF 0.2126
#define G_BRIGHTNESS_COEF 0.7152
#define B_BRIGHTNESS_COEF 0.0722

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
	int channel_scaled = channel * G_BRIGHTNESS_COEF;
	return rgba((base + channel_scaled) / 2, channel_scaled/2, channel_scaled/2);
}

rgba reddify_blue(const rgba to_rf) {
	int base = to_rf.r;
	int channel = to_rf.b;
	int channel_scaled = channel * B_BRIGHTNESS_COEF;
	return rgba((base + channel_scaled) / 2, channel_scaled/2, channel_scaled/2);
}

rgba greenify_red(const rgba to_gf) {
	int base = to_gf.g;
	int channel = to_gf.r;
	int channel_scaled = channel * R_BRIGHTNESS_COEF;
	return rgba(channel_scaled/2, (base + channel_scaled) / 2, channel_scaled/2);
}

rgba greenify_blue(const rgba to_gf) {
	int base = to_gf.g;
	int channel = to_gf.b;
	int channel_scaled = channel * B_BRIGHTNESS_COEF;
	return rgba(channel_scaled/2, (base + channel_scaled) / 2, channel_scaled/2);
}

rgba blueify_red(const rgba to_bf) {
	int base = to_bf.b;
	int channel = to_bf.r;
	int channel_scaled = channel * R_BRIGHTNESS_COEF;
	return rgba(channel_scaled/2, channel_scaled/2, (base + channel_scaled) / 2);
}

rgba blueify_green(const rgba to_bf) {
	int base = to_bf.b;
	int channel = to_bf.g;
	int channel_scaled = channel * G_BRIGHTNESS_COEF;
	return rgba(channel_scaled/2, channel_scaled/2, (base + channel_scaled) / 2);
}

rgba blueify(const rgba to_bf) {
	return rgba(0, 0, to_bf.b);
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

int main() {
	const char* filename = "DSC00209.jpg";

	int width, height, num_channels;
	//put image in cmake-build-debug folder
	unsigned char* data = stbi_load(filename, &width, &height, &num_channels, 4);

	if (data) {
		cout << "Congratulations! File loaded! :)" << endl;
		cout << "Width: " << width << " Height: " << height << " Channels: " << num_channels << endl;
		const img working = img(data, &width, &height);

		rgba (*funcs[])(rgba to_rf) = {reddify_green, reddify_blue, greenify_red, greenify_blue, blueify_red, blueify_green};

		const char* prefixes[] = {"redwithg_", "redwithb_", "greenwithr_", "greenwithb_", "bluewithr_", "bluewithg_"};

		for (int i = 0; i < 6; i++) {
			img to_convert(working);
			to_convert.transform(funcs[i]);
			char new_filename[20];
			strcpy(new_filename, prefixes[i]);
			strcat(new_filename, filename);
			if (to_convert.write(new_filename) != 1) {
				cerr << "Error writing image." << endl;
			}
		}
	} else {
		cerr << "Gosh diddly darn it, the file didn't load." << endl;
	}

	stbi_image_free(data);

	return 0;
}