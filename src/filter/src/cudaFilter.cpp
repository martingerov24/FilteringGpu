#include "demoFilters.h"
#include "filtering.h"

#if defined(ALLOW_CUDA)

#include <cuda_runtime.h>
#include <cuda_gl_interop.h>

#include "utils.cuh"

void *imgBuf = NULL; ///< Stores the device pointer returned after image allocation on the CUDA device
void *kernelBuf = NULL; ///< Stores the device pointer returned after allocating the convolution kernel on the CUDA device
cudaGraphicsResource_t resource; ///< handles OpenGL-CUDA exchange
cudaSurfaceObject_t writeSurface;

#define checkError(err) \
if ((err) != cudaSuccess) { \
	cudaDeviceReset();      \
	return (err);           \
}                           \

namespace supreme {


cudaError_t createSurfaceObject() {
	cudaResourceDesc wdsc;
	cudaArray* writeArray;
	if (resource == nullptr) {
		printf("texturePointer == nullptr at %s[%d] in function %s\n", __FILE__, __LINE__, __FUNCTION__);
		return cudaErrorUnknown;
	}

	cudaError_t cudaStatus = cudaGraphicsMapResources(1, &resource);
	checkError(cudaStatus);
	cudaStatus = cudaGraphicsSubResourceGetMappedArray(&writeArray, resource, 0, 0);
	checkError(cudaStatus);
	wdsc.resType = cudaResourceTypeArray;
	wdsc.res.array.array = writeArray;

	cudaStatus = cudaCreateSurfaceObject(&writeSurface, &wdsc);
	checkError(cudaStatus);

	cudaStatus = cudaGraphicsUnmapResources(1, &resource);
	checkError(cudaStatus);
	return cudaStatus;
}

cudaError_t registerImage(uint32_t texture) {
    cudaError_t cudaStatus = cudaGraphicsGLRegisterImage(
        &resource,
        texture,
        GL_TEXTURE_2D,
        cudaGraphicsRegisterFlagsNone
    );
    checkError(cudaStatus);
    return cudaStatus;
}

int resizeCudaBuffer(size_t size) {
	cudaError_t err = cudaSuccess;

	err = cudaFree(imgBuf);
	checkError(err);
	
	err = cudaMalloc(&imgBuf, size);
	checkError(err);
	
	return err;
}

int uploadCudaBuffer(const void* buffer, size_t size) {
	cudaError_t err = cudaSuccess;
	err = cudaMemcpy(imgBuf, buffer, size, cudaMemcpyHostToDevice);
	checkError(err);
	return err;
}

int remapCudaBuffer(uint32 glBuffer) {
	cudaError_t err = cudaSuccess;

	err = cudaGraphicsUnregisterResource(resource);
	checkError(err);

	err = cudaGraphicsGLRegisterBuffer(&resource, glBuffer, cudaGraphicsMapFlagsWriteDiscard);
	checkError(err);

	return err;
}

//cudaGetDeviceProperties() wrapper
cudaError_t getDeviceInfo(const int deviceId, DeviceInfo & devInfo);

int initCuda(uint32 glBuffer, DeviceInfo &devInfo) {
	cudaError_t err = cudaSuccess;
	err = cudaDeviceReset();
	checkError(err);

	int devCnt;
	err = cudaGetDeviceCount(&devCnt);
	checkError(err);

	err = cudaSetDevice(0);
	checkError(err);

	err = getDeviceInfo(0, devInfo);
	checkError(err);

	const size_t kernelBufferSize = SQR(KERNEL_SIZE(maxKernelNeighbourhood))*sizeof(float);
	err = cudaMalloc(&kernelBuf, kernelBufferSize);
	checkError(err);

	checkError(registerImage(glBuffer));

	checkError(createSurfaceObject());

	err = cudaDeviceSynchronize();
	checkError(err);
	
	return err;
}

/// Free any buffers currently in use and resets the device
int deinitCuda() {
	cudaError_t err = cudaSuccess;
	err = cudaFree(imgBuf);
	checkError(err);

    err = cudaDestroySurfaceObject(writeSurface);
    checkError(err);

	err = cudaFree(kernelBuf);
	checkError(err);
	err = cudaGraphicsUnregisterResource(resource);
	checkError(err);

	// cudaDeviceReset causes the driver to clean up all state. While
	// not mandatory in normal operation, it is good practice.  It is also
	// needed to ensure correct operation when the application is being
	// profiled. Calling cudaDeviceReset causes all profile data to be
	// flushed before the application exits
	cudaDeviceReset();

	return err;
}

/// Performs the image convolution on GPU
/// @param buffer Not used
/// @param image The host image we are filtering. Used to get width and height only
/// @param kernel Host buffer holding the kernel to be applied on the image this frame
/// @param nbhd Radius of the kernel
int filterWithCUDA(int imgWidth, int imgHeight, const float *kernel, int nbhd) {
	cudaArray* writeArray;

	cudaError_t cudaStatus = cudaGraphicsMapResources(1, &resource);
	checkError(cudaStatus);

	cudaStatus = cudaGraphicsSubResourceGetMappedArray(&writeArray, resource, 0, 0);
	checkError(cudaStatus);

	size_t size = SQR(KERNEL_SIZE(nbhd))*sizeof(float);
	cudaStatus = cudaMemcpy(kernelBuf, kernel, size, cudaMemcpyHostToDevice);
	checkError(cudaStatus);

	runCudaKernel(writeSurface, imgBuf, imgWidth, imgHeight, kernelBuf, nbhd);
	cudaStatus = cudaPeekAtLastError();
	checkError(cudaStatus);

	cudaStatus = cudaGraphicsUnmapResources(1, &resource, 0);
	checkError(cudaStatus);

	cudaStatus = cudaDeviceSynchronize();
	checkError(cudaStatus);

	return cudaStatus;
}


/// Query device with given id for parameters and returns them to the caller
cudaError_t getDeviceInfo(const int deviceId, DeviceInfo & devInfo)
{
	cudaDeviceProp devProps;
	cudaError_t res = cudaGetDeviceProperties(&devProps, deviceId);
	checkError(res);

	devInfo.devId = deviceId;
	devInfo.name = std::string(devProps.name);
	devInfo.major = devProps.major;
	devInfo.minor = devProps.minor;
	devInfo.maxThreadsPerBlock = devProps.maxThreadsPerBlock;
	devInfo.maxThreadsPerMP = devProps.maxThreadsPerMultiProcessor;
	devInfo.warpSize = devProps.warpSize;
	devInfo.multiProcessorCount = devProps.multiProcessorCount;
	devInfo.memory = devProps.totalGlobalMem;
	devInfo.sharedMemPerBlock = devProps.sharedMemPerBlock;
	devInfo.sharedMemPerMultiprocessor = devProps.sharedMemPerMultiprocessor;

	return res;
}

} //namespace supreme


#else
namespace supreme {

int initCuda(uint32 glBuffer, DeviceInfo &devInfo) {
	memset(&devInfo, 0, sizeof(DeviceInfo));
	return -1;
}
int deinitCuda() { return -1; }
int resizeCudaBuffer(size_t size) { return -1; }
int remapCudaBuffer(uint32 glBuffer) { return -1; }
int uploadCudaBuffer(const void* buffer, size_t size) { return -1; };
int filterWithCUDA(int imgWidth, int imgHeight, const float *kernel, int nbhd) { return -1; }

}
#endif
