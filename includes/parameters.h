#pragma once
#include <cinttypes>
#include <cstddef>

namespace supreme{
	enum deviceType : bool {
		CPU = 0,
		CUDA = 1,
	};
}

struct ImageParams {
	ImageParams(
		const int32_t _height,
		const int32_t _width,
		const int32_t _stride,
        const int32_t _bpp
	);

	inline size_t size() const { 
        return (height * stride); 
    }

    inline size_t numberOfPixels() const {
        return (height * width);
    }

	const int32_t height;
	const int32_t width;
	const int32_t stride;
    const int32_t bpp;
	const int8_t channels = 3;
};

#define return_if_false(x) do { \
    int retval = (x); \
    if (retval != 0) { \
        printf("Runtime error: %s returned %d at %s:%d\n", #x, retval, __FILE__, __LINE__); \
        return retval; \
    } \
} while (0)

#define break_if_false(x) do { \
    int retval = (x); \
    if (retval != 0) { \
        printf("Runtime error: %s returned %d at %s:%d\n", #x, retval, __FILE__, __LINE__); \
        break; \
    } \
} while (0)

#define return_if_nullptr(x, msg) do { \
    int retval = -1; \
    if (x == nullptr) { \
        printf("Runtime error:%s, %s returned %d at %s:%d\n", #msg, #x, retval, __FILE__, __LINE__); \
        return retval; \
    } \
} while (0)
