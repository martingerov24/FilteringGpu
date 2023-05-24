#include "image.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stbi_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stbi_image_write.h"
#include <assert.h>
#include <algorithm>

namespace supreme {

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

Image::Image(const std::string & fileName) {
	size_t pos = fileName.find_last_of(slashSymbol);
	if (pos != std::string::npos) {
		name = fileName.substr(pos+1);
	}
	loadFromFile(fileName);
}

inline void Image::freeImage() {
	data.clear();
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
	data.resize(width*height*sizeof(Color));
	memcpy(data.data(), rhs.getData(), sizeof(Color)*width*height);
}

inline int Image::allocateImage(const int newWidth, const int newHeight) {
	freeImage();
	data.resize(newWidth*newHeight*sizeof(Color));
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
	return data[y*width+x];
}

Color Image::getPixel(const float u, const float v) const {
	int w = int(u * (width-1));
	int h = int(v * (height-1));
	w = clamp(w, 0, width-1);
	h = clamp(h, 0, height-1);
	return data[h*width+w];
}

const Color* Image::getLine(const int idx) const {
	if (idx < 0 || idx >= height) return NULL;
	return data.data() + idx*width;
}

int Image::load(const std::string & fileName) {
	freeImage();
	return loadFromFile(fileName);
}

const float scalar = 1.f / 255.f;

int Image::loadFromFile(const std::string & fileName) {
	uint8_t* img = stbi_load(fileName.c_str(), &width, &height, &channels, 3);
	if(img == nullptr) {
		data.clear();
		valid = 0;
		return -1;
	}
	
	valid = 1;
	data.resize(width*height);
	size_t stride = channels*width;
	for (int i = 0; i < height; i++) {
		const uint8_t* line = &img[stride*i];
		for (int j = 0; j < width; j++) {
			float r = line[channels*j+0] * scalar;
			float g = line[channels*j+1] * scalar;
			float b = line[channels*j+2] * scalar;
			data[i*width + j] = Color(r, g, b);
		}
	}
	stbi_image_free(img);
	return 0;
}

std::vector<uint32_t> Image::getDataInInt() const{
	std::vector<uint32_t> uint_data(width*height);
	for (int i = 0; i < data.size(); i++) {
		uint_data[i] = data[i].toUINT32();
	}
	return uint_data;
}

int Image::save_jpg(const std::string & fileName) const {
	std::vector<uint32_t> castedData = getDataInInt();
	return stbi_write_jpg(fileName.c_str(), width, height, 4, reinterpret_cast<uint8_t*>(castedData.data()), 85);
}

void        Image::rename(const std::string &fName) { name = fName; }
int         Image::getWidth()    const              { return width; }
int         Image::getHeight()   const              { return height; }
int 		Image::getChannels() const 				{ return channels; }
bool        Image::isValid()     const              { return valid; }
const Color*Image::getData()     const        		{ return data.data(); }
uint64_t    Image::getMemUsage() const              { return width*height*sizeof(Color); }
std::string Image::getName()     const              { return name; }

}
