#include "image.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stbi_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stbi_image_write.h"
#include <assert.h>
#include <algorithm>

typedef unsigned long long uint64;

namespace CG2 {

template<typename T>
inline T clamp(const T& val, const T& minVal, const T& maxVal) { return std::min(std::max(val, minVal), maxVal); }

#ifdef _WIN32
	static const char slashSymbol = '\\';
#else
	static const char slashSymbol = '/';
#endif

Image::~Image() {
	freeImage();
}

Image::Image() {
	data=NULL;
	width = height = 0;
	valid = 0;
	name = std::string("Unnamed");
}

Image::Image(const Image &rhs) {
	data=NULL;
	copyImage(rhs);
}

Image::Image(const int width, const int height, const std::string &fName) {
	data=NULL;
	allocateImage(width, height);
	name = fName;
}

Image::Image(const int width, const int height, Color* buffer, const std::string &fName) {
	data=NULL;
	allocateImage(width, height);
	data = buffer;
	name = fName;
}

Image::Image(const std::string & fileName) {
	size_t pos = fileName.find_last_of(slashSymbol);
	if (pos != std::string::npos) {
		name = fileName.substr(pos+1);
	}
	loadFromFile(fileName);
}

inline void Image::freeImage() {
	delete [] data;
	data = NULL;
	valid = 0;
	width = height = 0;
}

inline void Image::copyImage(const Image &rhs) {
	if (this == &rhs) return;
	freeImage();
	valid = rhs.isValid();
	width  = rhs.getWidth();
	height = rhs.getHeight();
	name = rhs.name;
	data = new Color[width*height];
	memcpy(data, rhs.getData(), sizeof(Color)*width*height);
}

inline int Image::allocateImage(const int newWidth, const int newHeight) {
	freeImage();
	data = new Color[newWidth*newHeight];
	if (!data) return -1;
	width  = newWidth;
	height = newHeight;
	valid = 1;
	return 0;
}

int Image::allocate(const int width, const int height) {
	return allocateImage(width, height);
}

Image& Image::operator=(const Image &rhs) {
	copyImage(rhs);
	return *this;
}

Color Image::getPixel(const int x, const int y) const {
	int w = clamp(x, 0, width-1);
	int h = clamp(y, 0, height-1);
	return data[h*width+w];
}

Color Image::getPixel(const float u, const float v) const {
	int w = int(u * (width-1));
	int h = int(v * (height-1));
	w = clamp(w, 0, width-1);
	h = clamp(h, 0, height-1);
	return data[h*width+w];
}

Color* Image::getLine(const int idx) const {
	if (idx < 0 || idx >= height) return NULL;
	return &data[idx*width];
}

int Image::load(const std::string & fileName) {
	freeImage();
	return loadFromFile(fileName);
}

const float scalar = 1.f / 255.f;

int Image::loadFromFile(const std::string & fileName) {
	unsigned char* img = stbi_load(fileName.c_str(), &width, &height, &channels, 1);
	if (img == NULL) { return 0; }
	unsigned char* image_in_bytes = reinterpret_cast<unsigned char*>(data);	
	memcpy(&image_in_bytes[0], img, getMemUsage());
	stbi_image_free(img);
	return 0;
}

int Image::save(const std::string & fileName) const {
	stbi_write_jpg("rawFileViewer.jpg", width, height, channels, data, width * sizeof(int));
	return 0;
}

void        Image::rename(const std::string &fName) { name = fName; }
int         Image::getWidth()    const              { return width; }
int         Image::getHeight()   const              { return height; }
int 		Image::getChannels() const 				{ return channels; }
bool        Image::isValid()     const              { return valid; }
Color*      Image::getData()     const              { return data; }
uint64      Image::getMemUsage() const              { return width*height*sizeof(Color); }
std::string Image::getName()     const              { return name; }

}
