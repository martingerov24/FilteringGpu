cmake_minimum_required(VERSION 3.20)
project (cg2_Demo LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
set(CONFIG_PATH ${CMAKE_SOURCE_DIR}/config)
set(SDK_PATH ${CMAKE_SOURCE_DIR}/sdk)
set(INCLUDES_PATH ${CMAKE_SOURCE_DIR}/includes)

set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_CRT_SECURE_NO_WARNINGS=1 -D_UNICODE -DUNICODE")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_CRT_SECURE_NO_WARNINGS=1 -D_UNICODE -DUNICODE")

add_subdirectory (src/filter)
add_subdirectory (src/renderer)

set(SOURCES src/main.cpp)
add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} PUBLIC
    ImageFiltering
    Renderer
    ${CMAKE_DL_LIBS}
)