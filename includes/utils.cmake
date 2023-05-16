function(checkPath ARG1)
    if (NOT EXISTS ${ARG1} AND NOT IS_DIRECTORY ${ARG1})
        message(FATAL_ERROR "------ function checkPath() ---- ${ARG1} is not a directory")
    endif()
endfunction()

function(BUILD_PTX name)
    if (WIN32)
        set(NVCC "$ENV{CUDA_PATH}/bin/nvcc.exe")
    else()
        set(NVCC "$ENV{CUDA_PATH}/bin/nvcc")
    endif()

    set(NVCC_FLAGS
        -m64
        --use_fast_math
        -cudart static
        --gpu-architecture=compute_${CMAKE_CUDA_ARCHITECTURES}
        --gpu-code=sm_${CMAKE_CUDA_ARCHITECTURES},compute_${CMAKE_CUDA_ARCHITECTURES}
        -O4
    )
    set(SOURCE_FILE ${CMAKE_CURRENT_SOURCE_DIR}/${name}.cu)
    set(OUTPUT_FILE ${CMAKE_BINARY_DIR}/${name}.ptx)

    set(EXECUTE_COMMAND
        ${NVCC}
        ${NVCC_FLAGS}
        -ccbin ${CMAKE_CXX_COMPILER}
        ${GPU_INCLUDES}
        -ptx -o ${OUTPUT_FILE} ${SOURCE_FILE}
    )
    add_custom_target(${name} ALL DEPENDS ${OUTPUT_FILE})

    add_custom_command(
        COMMAND    ${EXECUTE_COMMAND}
        DEPENDS    ${SOURCE_FILE}
        OUTPUT     ${OUTPUT_FILE}
    )
    install(FILES ${OUTPUT_FILE} DESTINATION ${INSTALL_PATH})
endfunction(BUILD_PTX)

function(enable_cuda_if_available)
    # Here CUDA is properly found and variable are correctly set
    include(CheckLanguage)
    check_language(CUDA)
    if (CMAKE_CUDA_COMPILER)
        enable_language(CUDA)
    else ()
        message(WARNING "CUDA not found: GPU features won't be available.")
    endif ()
endfunction()

function(see_visible_environment_variable)
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E environment
    )
endfunction()