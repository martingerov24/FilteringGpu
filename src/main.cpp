#include <stdio.h>
#include <vector>
#include <inttypes.h>
#include "image.h"
#include "demoFilters.h"
#include "windowManager.h"
#include "parameters.h"

void processAndDisplay(
    const supreme::Image& image, 
    const ImageParams& params,
    uint8_t* byte_output_data
) {
    supreme::DeviceInfo devInfo;
    supreme::deviceType devType = supreme::deviceType::CPU;
    int32_t nvhdFilter = 2;
    float sharpen_blur = 0.0f;
    bool use_filter = true;

    WindowManager window;
	window.init();
    uint32_t glBuffer = window.getTextureId();
    supreme::initCuda(glBuffer, devInfo);
	supreme::newImageLoaded(image);
    supreme::uploadCudaBuffer(image.getData(), image.getMemUsage());

    uint32_t* image_in_four_bytes = reinterpret_cast<uint32_t*>(byte_output_data);
    
	while (window.shouldClose()) {
        window.onNewFrame();
        window.useFilter(use_filter);
        window.changeState(devType);
        if(use_filter) {
            supreme::filterImage(devType, image_in_four_bytes, image, sharpen_blur, nvhdFilter);
        }
        window.createSlider("Neighbour Filter", nvhdFilter, 0, 100);
		window.createSlider("Sharpen Filter", sharpen_blur, -1.0f, 1.0f);
        if(window.draw(byte_output_data, params, devType) == false) {
			break;
		}
        use_filter = false;
        window.swapBuffers();
	}
    supreme::deinitCuda();
	window.terminate();
}

int main() {
    supreme::Image image("/home/mgerov/code/fmi/cg2/peacock-feather-1638181.jpg");
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