cmake_minimum_required(VERSION 3.20)
set(CMAKE_CUDA_ARCHITECTURES 50)

project(renderer LANGUAGES CXX)
find_package(OpenGL REQUIRED)

include(${INCLUDES_PATH}/utils.cmake)

#-----------imgui Dependencies----------
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

#-----------glad Dependencies-----------
if(DEFINED ENV{GLAD_DIR})
    set(GLAD_DIR $ENV{GLAD_DIR})
    message("--- ${GLAD_DIR} env is defined")
    set(GLAD_INCLUDE ${GLAD_DIR}/include)
    add_library("glad" STATIC "${GLAD_DIR}/src/glad.c")
    target_include_directories(glad PRIVATE ${GLAD_INCLUDE})
    set(GLAD_LIBS glad)
else()
    checkPath("${SDK_PATH}/glad")
    set(GLAD_DIR "${SDK_PATH}/glad")
    add_library("glad" STATIC "${GLAD_DIR}/src/glad.c")
    target_include_directories("glad" PRIVATE "${GLAD_DIR}/include")
endif()


#-----------glfw Dependencies-----------
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_DIR ${SDK_PATH}/glfw/)
set(GLFW_INCLUDE ${GLFW_DIR}/include)
set(GLFW_LIBRARY_TYPE STATIC)
set(GLFW_LIBS glfw)
add_subdirectory(${GLFW_DIR} glfw)

set(SOURCES
    windowManager.cpp
    ${IMGUI_SOURCES}
)
add_library(${PROJECT_NAME} STATIC ${SOURCES} )

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/
    ${INCLUDES_PATH}
    ${SDK_PATH}
)

target_include_directories(${PROJECT_NAME} PUBLIC
    ${GLAD_INCLUDE}
    ${GLFW_INCLUDE}
    ${IMGUI_INCLUDES}
)

target_link_libraries(${PROJECT_NAME} PUBLIC
    ${OPENGL_LIBRARIES}
	${GLAD_LIBS}
    ${GLFW_LIBS}
)