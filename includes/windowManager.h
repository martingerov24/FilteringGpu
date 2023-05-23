
#pragma once
#include <GLFW/glfw3.h>
#include "parameters.h"

class WindowManager {
    void bindTexture();
    void createContext();
    void onNewFrame() const;
public:
    WindowManager() = default;
    bool init();
    bool shouldClose();
    inline uint32_t getTextureId() const { return texture; }; 
    bool draw(
        const uint8_t* output, 
        const ImageParams& params
    ) const;
    void terminate();
private:
    GLFWwindow* window = nullptr;
    GLuint texture = -1;
};

