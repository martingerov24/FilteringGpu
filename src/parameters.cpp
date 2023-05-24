#include "parameters.h"

ImageParams::ImageParams(
    const int32_t _height,
    const int32_t _width,
    const int32_t _stride,
    const int32_t _bpp
) : height(_height)
, width(_width)
, stride(_stride)
, bpp(_bpp) {}