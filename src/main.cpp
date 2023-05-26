#include <stdio.h>
#include <vector>
#include <inttypes.h>
#include "image.h"
#include "demoFilters.h"
#include "windowManager.h"
#include "parameters.h"
#include "stbi_image_write.h"

namespace debug {
    int saveImage(const std::vector<uint8_t>& output, const int width, const int height){
    	return stbi_write_jpg("debug_save.jpg", width, height, 4, output.data(), 85);
    }
}

int32_t processAndDisplay(
    const supreme::Image& image, 
    const ImageParams& params,
    uint8_t* byte_output_data
) {
    supreme::DeviceInfo devInfo;
    supreme::deviceType devType = supreme::deviceType::CUDA;
    int32_t nvhdFilter = 39;
    float sharpen_blur = -1.0f;
    bool use_filter = true;

    WindowManager window;
	window.init(params);
    uint32_t glBuffer = window.getTextureId();
    
    std::vector<uint32_t> cudaImg = image.getDataInInt();
    return_if_false(supreme::initCuda(glBuffer, devInfo));
	return_if_false(supreme::newImageLoaded(image));
    return_if_false(supreme::resizeCudaBuffer(cudaImg.size() * sizeof(cudaImg[0])));
    return_if_false(supreme::uploadCudaBuffer(cudaImg.data(), cudaImg.size() * sizeof(cudaImg[0])));
    
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
        debug::saveImage(debug::getTextureData(params,  window.getTextureId()), params.width, params.height);
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