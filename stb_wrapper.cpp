#include "stb_image.h"
#include "stb_image_write.h"
#include "stb_wrapper.h"

#include <iostream>

using namespace std;

rgba::rgba(const int r, const int g, const int b, const int a) {
	this->r = r;
	this->g = g;
	this->b = b;
	this->a = a;
}

rgba::rgba() {
	this->r = 0;
	this->g = 0;
	this->b = 0;
	this->a = 0;
}

bool rgba::operator==(const rgba obj) const {
	return (r == obj.r) && (g == obj.g) && (b == obj.b) && (a == obj.a);
}

[[nodiscard]] double rgba::brightness() const {
	return r * R_BRIGHTNESS_COEF + g * G_BRIGHTNESS_COEF + b * B_BRIGHTNESS_COEF;
}

bool rgba::operator<(const rgba& obj) const {
	return brightness() < obj.brightness();
}

bool rgba::operator>(const rgba& obj) const {
	return brightness() > obj.brightness();
}

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

img::img(const unsigned char* raw_dat, const int* width, const int* height) {
	//cout << "Constructing image." << endl;
	data = new rgba[*width * *height];
	pixel_num = *width * *height;
	from_raw_dat(raw_dat, pixel_num, data);
	this->width = *width;
	this->height = *height;
}

img::~img() {
	//cout << "Destructing image." << endl;
	delete[] data;
}

img::img(const img& other) {
	//cout << "Copy constructing image." << endl;
	pixel_num = other.pixel_num;
	width = other.width;
	height = other.height;
	data = new rgba[pixel_num];
	for (int i = 0; i < pixel_num; i++) {
		data[i] = other.data[i];
	}
}

img& img::operator=(const img& other) {
	//cout << "Copy assigning image." << endl;

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

ostream& operator<<(ostream &stream, const rgba obj) {
	stream << "Red: " << obj.r << " Green: " << obj.g << " Blue: " << obj.b << " Alpha: " << obj.a;
	return stream;
}

void to_raw_dat(const rgba* data, const int num_pix, unsigned char* write_into) {
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

void img::transform(const function<rgba(rgba)>& funct) {

	for (int i = 0; i < pixel_num; i++) {
		data[i] = funct(data[i]);
	}

}

rgba rgba_combine(const rgba first, const rgba second) {
	return {(first.r + second.r) / 2, (first.g + second.g) / 2, (first.b + second.b) / 2, (first.a + second.a) / 2};
}

void img::combine(const img& other) const {

	const rgba* other_data = other.data;

	for (int i = 0; i < pixel_num; i++) {
		data[i] = rgba_combine(data[i], other_data[i]);
	}

}

img::img(const char* filename, int* code) {
	//cout << "Constructing image." << endl;
	width = 0;
	height = 0;
	int num_channels;
	const unsigned char* raw_dat = stbi_load(filename, &width, &height, &num_channels, 4);
	data = new rgba[width * height];
	pixel_num = width * height;
	from_raw_dat(raw_dat, pixel_num, data);
	*code = raw_dat != nullptr;
	stbi_image_free((void*) raw_dat);
}