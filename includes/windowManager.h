
#pragma once
#include <GLFW/glfw3.h>
#include "parameters.h"

class WindowManager {
    void bindTexture();
    void createContext();
public:
    WindowManager() = default;
    //init OpenGl, create textures to render on.
    bool init();
    //used in main loop, so it can be closed on X button.
    bool shouldClose() const;
    //draws everything on the screen and swaps with the next window buffer.
    void swapBuffers() const;
    //OpenGl's, GLFW's on new frame call.
    void onNewFrame() const;
    //creates a slider with the given name @slider_name
    //@value is changed, based on the value passed by the user.
    //@min minumumn value of the slider
    //@max maximumn value of the slider
    void createSlider(
        const char* slider_name, 
        float& value,
        const float min, 
        const float max
    ) const;
    void createSlider(
        const char* slider_name, 
        int32_t& value,
        const uint32_t min, 
        const uint32_t max
    ) const;
    //changes the state of @useFilter on button click
    void useFilter(bool& useFilter) const;
    //whether to use CPU or GPU.
    void changeState(supreme::deviceType& type) const;
    //get the id of member texture.
    inline uint32_t getTextureId() const { return texture; };
    //draws the class member texture
    //if GPU, we expext the code to save with cuda_interop, so buffer transfer from CPU->texture
    //won't be done on the draw function. 
    bool draw(
        const uint8_t* output, 
        const ImageParams& params,
        supreme::deviceType& type
    );
    //shutdown GLFW and OpenGL.
    void terminate();
private:
    GLFWwindow* window = nullptr;
    GLuint texture = -1;
};

