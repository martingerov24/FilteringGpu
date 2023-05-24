
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
    void useFilter(bool& useFilter);
    void changeState(supreme::deviceType& type);
    inline uint32_t getTextureId() const { return texture; }; 
    bool draw(
        const uint8_t* output, 
        const ImageParams& params,
        supreme::deviceType& type
    );
    void terminate();
private:
    GLFWwindow* window = nullptr;
    GLuint texture = -1;
};

