cmake_minimum_required(VERSION 3.20)
project(Renderer LANGUAGES CXX)
find_package(OpenGL REQUIRED)

include(${CONFIG_PATH}/utils.cmake)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
clone(glfw https://github.com/glfw/glfw.git 3.3.2)
clone(glad https://github.com/Dav1dde/glad.git df8e9e16110b305479a875399cee13daa0ccadd9)

#----------imgui Dependencies----------
checkPath(${SDK_PATH}/imgui)
set(IMGUI_DIR ${SDK_PATH}/imgui)
set(IMGUI_SOURCES
    "${IMGUI_DIR}/imgui_demo.cpp"
    "${IMGUI_DIR}/imgui_draw.cpp"
    "${IMGUI_DIR}/backends/imgui_impl_glfw.cpp"
    "${IMGUI_DIR}/backends/imgui_impl_glfw.h"
    "${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp"
    "${IMGUI_DIR}/backends/imgui_impl_opengl3.h"
    "${IMGUI_DIR}/imgui_tables.cpp"
    "${IMGUI_DIR}/imgui_widgets.cpp"
    "${IMGUI_DIR}/imgui.cpp"
)

set(IMGUI_INCLUDES 
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends 
)

set(SOURCES
    windowManager.cpp
    ${IMGUI_SOURCES}
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${OPENGL_INCLUDE_DIR} 
    ${IMGUI_INCLUDES}
    ${INCLUDES_PATH}
    ${SDK_PATH}
)

target_link_libraries(${PROJECT_NAME} PUBLIC
    ${OPENGL_LIBRARIES}
    ${IMGUI_LIBRARIES}
    glad 
    glfw
)