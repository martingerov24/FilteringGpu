cmake_minimum_required(VERSION 3.20)
set(CMAKE_CUDA_ARCHITECTURES 50)
project(ImageFiltering LANGUAGES CXX)

include(${CONFIG_PATH}/utils.cmake)

find_package(CUDAToolkit)

if(CUDAToolkit_FOUND)
    message("CUDA FOUND")
    enable_language(CUDA)
    set(USE_CUDA TRUE)
endif()

if(DEFINED ENV{GLEW})
    set(GLEW $ENV{GLEW})
    set(GLEW_INCLUDES ${GLEW}/include)
else()
	message(FATAL_ERROR "GLEW PATH not found in environment but is required.")
endif()

set(HEADERS 
	${SDK_PATH}/stbi_image.h
	${SDK_PATH}/stbi_image_write.h
)

set(SOURCES
	src/Color.cpp
	src/cpuFilter.cpp
	src/cudaFilter.cpp
	src/filtering.cpp
	src/image.cpp
)

set(DEVICE_SOURCES
    cuda/kernel.cu
    cuda/utils.cuh	
)

set_source_files_properties(${HEADERS} ${SOURCES} PROPERTIES LANGUAGE CXX)

if(USE_CUDA)
    set_source_files_properties(${DEVICE_SOURCES}     PROPERTIES LANGUAGE CUDA)
endif()

add_library(${PROJECT_NAME} STATIC ${HEADERS} ${SOURCES} ${DEVICE_SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/cuda
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${INCLUDES_PATH}
    ${GLEW_INCLUDES}/GL
    ${SDK_PATH}
)

if(USE_CUDA)
    target_compile_options(${PROJECT_NAME} PRIVATE $<$<COMPILE_LANGUAGE:CUDA>:
        --generate-line-info
        --use_fast_math
        >
    )

    target_link_libraries(${PROJECT_NAME} PUBLIC
        CUDA::cudart
    )
endif()

install(TARGETS ${PROJECT_NAME} DESTINATION ${INSTALL_PATH})