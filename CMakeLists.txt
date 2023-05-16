cmake_minimum_required(VERSION 3.20)
set(CMAKE_CUDA_ARCHITECTURES 50)
project (cg2_Demo LANGUAGES CXX)

set(SDK_PATH ${CMAKE_SOURCE_DIR}/sdk)
set(INCLUDES_PATH ${CMAKE_SOURCE_DIR}/includes)

# set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_CRT_SECURE_NO_WARNINGS=1 -D_UNICODE -DUNICODE")
# set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_CRT_SECURE_NO_WARNINGS=1 -D_UNICODE -DUNICODE")

add_subdirectory (src/filter)
add_subdirectory (src/renderer)