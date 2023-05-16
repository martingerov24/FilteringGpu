cmake_minimum_required(VERSION 3.20)
set(CMAKE_CUDA_ARCHITECTURES 50)
project(ImageFiltering LANGUAGES CXX)

include(${INCLUDES_PATH}/utils.cmake)
see_visible_environment_variable()
enable_cuda_if_available()
if(DEFINED ENV{CUDA_PATH})
    set(INSTALL_PATH ${CMAKE_CURRENT_SOURCE_DIR}/build)
    set(CudaToolkitDir $ENV{CUDA_PATH})
    set(CUDA_INCLUDE_DIRS ${CudaToolkitDir}/include)
    set(CUDA_LIBS
        ${CudaToolkitDir}/lib/x64
        ${CudaToolkitDir}/lib64
        ${CudaToolkitDir}/lib64/stubs
    )
    build_ptx(kernelnvoke)
else()
    message("$ENV{CUDA_PATH}")
	message(FATAL_ERROR "CUDA PATH not found in environment but is required.")
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
)

set_source_files_properties(${HEADERS} ${SOURCES} PROPERTIES LANGUAGE CXX)
set_source_files_properties(${DEVICE_SOURCES}     PROPERTIES LANGUAGE CUDA)

add_library(${PROJECT_NAME} STATIC ${HEADERS} ${DEVICE_SOURCES})
	
target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CUDA_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/cuda
)

target_compile_options(${PROJECT_NAME} PRIVATE $<$<COMPILE_LANGUAGE:CUDA>:
    --generate-line-info
    --use_fast_math
    >
)

target_link_directories(${PROJECT_NAME} PUBLIC
    ${CUDA_LIBS}
)

target_link_libraries(${PROJECT_NAME} PUBLIC
    cuda
    cudart
)

install(TARGETS ${PROJECT_NAME} DESTINATION ${INSTALL_PATH})