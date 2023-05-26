#include "windowManager.h"
#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"
#include "stbi_image_write.h"
#include <cstdio>
#include <assert.h>

inline bool GLLogError(const char* function, const char* file, int line);
inline void GLClearError() {
    while (glGetError() != GL_NO_ERROR);
}

#ifdef _MSC_VER
    #define ASSERT(x) if (!(x)) __debugbreak();// suitible for MSVS
#else
    #define ASSERT(x) if (!(x)) assert(false);
#endif

#define GLCall(x) GLClearError();\
    x;\
    ASSERT(GLLogError(#x, __FILE__, __LINE__))

#define RGBA_SIZE (4)

void bindTexture(const uint32_t texture) {
    GLCall(glBindTexture(GL_TEXTURE_2D, texture));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GLCall(glPixelStorei(GL_UNPACK_ROW_LENGTH, 0));
}

void unbindTexture() {
    GLCall(glBindTexture(GL_TEXTURE_2D, 0));
} 

void WindowManager::onNewFrame() const{
    GLCall(glfwPollEvents());
    GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
    GLCall(glClear(GL_COLOR_BUFFER_BIT));

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    bool is_show = true;
    ImGui::Begin("Image", &is_show);
}

bool WindowManager::shouldClose() const {
    return !glfwWindowShouldClose(window);
}

void WindowManager::changeState(supreme::deviceType& type) {
    bool firstState = static_cast<bool>(type); 
    bool enumValue = firstState;

    ImGui::SetCursorPos(ImVec2(10, 60));
    if (firstState) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 1.0f, 0.0f, 1.0f)); // Set button color to a lighter shade
    }
    if (ImGui::Button("Use CUDA")) {
        enumValue = !enumValue;
    }
    if (firstState) {
        ImGui::PopStyleColor(); // Reset button color
    }
    
    type = static_cast<supreme::deviceType>(enumValue);
}

uint32_t WindowManager::getTextureId(const supreme::deviceType& type) const {
    return type == supreme::deviceType::CPU ? m_texture_cpu : m_texture_cuda;
}

void WindowManager::useFilter(bool& useFilter) const {
    ImGui::SetCursorPos(ImVec2(10, 30));
    if (ImGui::Button("Use Filter")) {
        useFilter = !useFilter;
    }
}

namespace debug {
    const std::vector<uint8_t>& getTextureData(const ImageParams& params, const uint32_t texture) {
        static std::vector<uint8_t> data(params.numberOfPixels() * RGBA_SIZE, 1);
        bindTexture(texture);
        GLCall(glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.data()));
        unbindTexture();
        return data;
    }
    int saveImage(const std::vector<uint8_t>& output, const int width, const int height){
    	return stbi_write_jpg("image.jpg", width, height, 4, output.data(), 85);
    }
    int saveImage(const uint8_t* output, const int width, const int height){
    	return stbi_write_jpg("image.jpg", width, height, 4, output, 85);
    }
}

void WindowManager::saveImage(const uint8_t* data, const int width, const int height) const {
    if (ImGui::Button("Save Image")) {
        debug::saveImage(data, width,height);
    }
}

void WindowManager::createContext() {
    GLCall(glfwMakeContextCurrent(window));
    GLCall(glfwSwapInterval(0));

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

bool WindowManager::init(const ImageParams& params) {
    GLCall(glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE));
    GLCall(glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3));
    GLCall(glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3));
    if (!glfwInit()) {
        throw "glfwInit() FAILED!";
    }

    window = glfwCreateWindow(800, 600, "Image Viewer", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return false;
    }
	createContext();

	GLCall(glGenTextures(1, &m_texture_cuda));
    GLCall(bindTexture(m_texture_cuda));
    GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, params.width, params.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr));
    
    GLCall(glGenTextures(1, &m_texture_cpu));
    GLCall(bindTexture(m_texture_cpu));
    GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, params.width, params.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr));
    return window;
}

void WindowManager::terminate() {
    ImGui_ImplGlfw_Shutdown();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();
    GLCall(glfwTerminate());
    GLCall(glfwDestroyWindow(window));
}

void WindowManager::createSlider(
    const char* slider_name, 
    float& value,
    const float min, 
    const float max
) const {
    ImGui::SetCursorPos(ImVec2(100, 60));
    ImGui::SliderFloat(slider_name, &value, min, max);
}

void WindowManager::createSlider(
    const char* slider_name, 
    int32_t& value,
    const uint32_t min, 
    const uint32_t max
) const {
    ImGui::SetCursorPos(ImVec2(100, 30));
    ImGui::SliderInt(slider_name, &value, min, max);
}

void WindowManager::swapBuffers() const {
    ImGui::End();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    unbindTexture();
    GLCall(glfwSwapBuffers(window));
}

int WindowManager::draw(const uint8_t* output, const ImageParams& params, supreme::deviceType& type) {
    if(output==nullptr) {
        return -1;
    }

    render_primitive = (ImTextureID)(intptr_t)getTextureId(type);
    bindTexture(getTextureId(type));
    if(type == supreme::deviceType::CPU) {
        GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, params.width, params.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, output));
    }
    ImGui::Image(render_primitive, ImVec2(1000,600));
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    return 0;
}

bool GLLogError(const char* function, const char* file, int line) {
    while (GLenum error = glGetError()) {
        fprintf(stderr, "[OPENGL error] (%x)- %s %s:%d\n",
            error,
            function,
            file,
            line);

        return false;
    }
    return true;
}