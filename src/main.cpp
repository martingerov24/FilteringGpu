#include <stdio.h>
#include <vector>
#include <inttypes.h>
#include "image.h"
#include "demoFilters.h"
#include "windowManager.h"
#include "parameters.h"

int32_t processAndDisplay(
    const supreme::Image& image, 
    const ImageParams& params,
    uint8_t* byte_output_data
) {
    supreme::DeviceInfo devInfo;
    supreme::deviceType devType = supreme::deviceType::CUDA;
    int32_t nvhdFilter = 2;
    float sharpen_blur = -1.0f;
    bool use_filter = true;

    WindowManager window;
	window.init(params, BufferType::BUFFER);
    uint32_t glBuffer = window.getBufferId();

    return_if_false(supreme::initCuda(glBuffer, devInfo));
	return_if_false(supreme::newImageLoaded(image));
    return_if_false(supreme::resizeCudaBuffer(image.getMemUsage()));
    return_if_false(supreme::uploadCudaBuffer(image.getData(), image.getMemUsage()));
    
    uint32_t* image_in_four_bytes = reinterpret_cast<uint32_t*>(byte_output_data);

	while (window.shouldClose()) {
        window.onNewFrame();
        window.useFilter(use_filter);
        window.changeState(devType);
        window.createSlider("Neighbour Filter", nvhdFilter, 0, 100);
		window.createSlider("Sharpen Filter", sharpen_blur, -1.0f, 1.0f);
        supreme::initFilters(nvhdFilter);
        if(use_filter || devType == supreme::deviceType::CUDA) {
            break_if_false(supreme::filterImage(devType, image_in_four_bytes, image, sharpen_blur, nvhdFilter));
        }
        const std::vector<uint8_t>& output = debug::getTextureData(params, BufferType::BUFFER, window.getBufferId(), window.getTextureId()); 
        break_if_false(window.draw(byte_output_data, params, devType));
        use_filter = false;
        window.swapBuffers();
	}
    return_if_false(supreme::deinitCuda());
	window.terminate();
    return 0;
}

int main() {
    supreme::Image image("/home/mgerov/code/fmi/cg2/peacock.png");
    if(image.isValid() == false){
        return -1;
    }
    image.save_jpg("fileSaved.jpg");
    ImageParams params(image.getHeight(), image.getWidth(), image.getWidth(), 0);
    uint8_t* byte_output_data = (uint8_t*)malloc(image.getMemUsage());
    
    processAndDisplay(image, params, byte_output_data);

    free(byte_output_data);
    return 0;
}