#ifndef COLORLESS_COLOR_STB_WRAPPER_H
#define COLORLESS_COLOR_STB_WRAPPER_H

#include <functional>
#include <iosfwd>

#define R_BRIGHTNESS_COEF 0.2126
#define G_BRIGHTNESS_COEF 0.7152
#define B_BRIGHTNESS_COEF 0.0722

using namespace std;

struct rgba {
	int r;
	int g;
	int b;
	int a;
	rgba(int r, int g, int b, int a = 255);
	rgba();
	bool operator==(rgba obj) const;
	[[nodiscard]] double brightness() const;
	bool operator<(const rgba& obj) const;
	bool operator>(const rgba& obj) const;
	[[nodiscard]] inline rgba complement() const {
		return {255 - r, 255 - g, 255 - b};
	};
};

int compare_rgba(const void* first, const void* second);

ostream& operator<<(ostream &stream, rgba obj);

void to_raw_dat(const rgba* data, int num_pix, unsigned char* write_into);

void from_raw_dat(const unsigned char* data, int num_pix, rgba* write_into);

class img {
protected:
	rgba* data;
	int pixel_num;
	int width;
	int height;
public:
	img(const unsigned char* raw_dat, const int* width, const int* height);
	img(const char* filename, int* code);
	virtual ~img();
	img(const img& other);
	img& operator=(const img& other);
	int write(const char* filename) const;
	void transform(const function<rgba(rgba)>& funct);
	void combine(const img& other) const;
};

rgba rgba_combine(rgba first, rgba second);

#endif //COLORLESS_COLOR_STB_WRAPPER_H