#include <stdio.h>
#include <vector>
#include <inttypes.h>
#include "image.h"
#include "demoFilters.h"
#include "windowManager.h"

void processAndDisplay(
    const supreme::Image& image, 
    const ImageParams& params,
    uint8_t* byte_output_data
) {
    supreme::DeviceInfo devInfo;
    uint32 glBuffer;

    WindowManager window;
	window.init();
    supreme::initCuda();


	supreme::newImageLoaded(image);

	while (window.shouldClose()) {
		if(window.draw(byte_output_data, params) == false) {
			break;
		}
	}
	window.terminate();
}

int main() {
    supreme::Image image("thing.png");
    ImageParams params(image.getHeight(), image.getWidth(), image.getWidth(), 0);
    uint8_t* byte_output_data = (uint8_t*)malloc(image.getMemUsage());
    
    processAndDisplay(image, params, byte_output_data);

    free(byte_output_data);
    return 0;
}