#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(__cplusplus)
#define __CL_ENABLE_EXCEPTIONS
#include "cl.hpp"
#endif /* __cplusplus */
#include <iostream>

using std::cout;

// macros to reduce pollution
#define P(obj, w) cout << #w << ": " << obj->getInfo<w>() << "\n";
#define Pmb(obj, w) cout << #w << ": " << obj->getInfo<w>() / (1024 * 1024) <<" MB \n";
#define Pkb(obj, w) cout << #w << ": " << obj->getInfo<w>() / (1024 ) << " KB \n";
#define Pbool(obj, w) cout << std::boolalpha << #w << ": " << static_cast<bool>(obj->getInfo<w>()) << "\n";

#define PbitmapStart(obj, w) { unsigned bitmap = obj->getInfo<w>(); cout << #w << ": ";
#define PbitmapTest(w) if (bitmap & w) cout << #w " ";
#define PbitmapEnd cout << "\n"; }

#define PconstStart(obj, w) { unsigned constant = obj->getInfo<w>(); cout << #w << ": ";
#define PconstTest(w) if (constant == w) cout << #w "\n";
#define PconstEnd }

inline const char *CLErrorString(cl_int err)
{
	switch (err) {
	case CL_SUCCESS:
		return "CL_SUCCESS";                                                                                                    // break;
	case CL_DEVICE_NOT_FOUND:
		return "CL_DEVICE_NOT_FOUND";                                                                                           // break;
	case CL_DEVICE_NOT_AVAILABLE:
		return "CL_DEVICE_NOT_AVAILABLE";                                                                                       // break;
	case CL_COMPILER_NOT_AVAILABLE:
		return "CL_COMPILER_NOT_AVAILABLE";                                                                                     // break;
	case CL_MEM_OBJECT_ALLOCATION_FAILURE:
		return "CL_MEM_OBJECT_ALLOCATION_FAILURE";                                                                              // break;
	case CL_OUT_OF_RESOURCES:
		return "CL_OUT_OF_RESOURCES";                                                                                           // break;
	case CL_OUT_OF_HOST_MEMORY:
		return "CL_OUT_OF_HOST_MEMORY";                                                                                         // break;
	case CL_PROFILING_INFO_NOT_AVAILABLE:
		return "CL_PROFILING_INFO_NOT_AVAILABLE";                                                                               // break;
	case CL_MEM_COPY_OVERLAP:
		return "CL_MEM_COPY_OVERLAP";                                                                                           // break;
	case CL_IMAGE_FORMAT_MISMATCH:
		return "CL_IMAGE_FORMAT_MISMATCH";                                                                                      // break;
	case CL_IMAGE_FORMAT_NOT_SUPPORTED:
		return "CL_IMAGE_FORMAT_NOT_SUPPORTED";                                                                                 // break;
	case CL_BUILD_PROGRAM_FAILURE:
		return "CL_BUILD_PROGRAM_FAILURE";                                                                                      // break;
	case CL_MAP_FAILURE:
		return "CL_MAP_FAILURE";                                                                                                // break;
	case CL_INVALID_VALUE:
		return "CL_INVALID_VALUE";                                                                                              // break;
	case CL_INVALID_DEVICE_TYPE:
		return "CL_INVALID_DEVICE_TYPE";                                                                                        // break;
	case CL_INVALID_PLATFORM:
		return "CL_INVALID_PLATFORM";                                                                                           // break;
	case CL_INVALID_DEVICE:
		return "CL_INVALID_DEVICE";                                                                                             // break;
	case CL_INVALID_CONTEXT:
		return "CL_INVALID_CONTEXT";                                                                                            // break;
	case CL_INVALID_QUEUE_PROPERTIES:
		return "CL_INVALID_QUEUE_PROPERTIES";                                                                                   // break;
	case CL_INVALID_COMMAND_QUEUE:
		return "CL_INVALID_COMMAND_QUEUE";                                                                                      // break;
	case CL_INVALID_HOST_PTR:
		return "CL_INVALID_HOST_PTR";                                                                                           // break;
	case CL_INVALID_MEM_OBJECT:
		return "CL_INVALID_MEM_OBJECT";                                                                                         // break;
	case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:
		return "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";                                                                            // break;
	case CL_INVALID_IMAGE_SIZE:
		return "CL_INVALID_IMAGE_SIZE";                                                                                         // break;
	case CL_INVALID_SAMPLER:
		return "CL_INVALID_SAMPLER";                                                                                            // break;
	case CL_INVALID_BINARY:
		return "CL_INVALID_BINARY";                                                                                             // break;
	case CL_INVALID_BUILD_OPTIONS:
		return "CL_INVALID_BUILD_OPTIONS";                                                                                      // break;
	case CL_INVALID_PROGRAM:
		return "CL_INVALID_PROGRAM";                                                                                            // break;
	case CL_INVALID_PROGRAM_EXECUTABLE:
		return "CL_INVALID_PROGRAM_EXECUTABLE";                                                                                 // break;
	case CL_INVALID_KERNEL_NAME:
		return "CL_INVALID_KERNEL_NAME";                                                                                        // break;
	case CL_INVALID_KERNEL_DEFINITION:
		return "CL_INVALID_KERNEL_DEFINITION";                                                                                  // break;
	case CL_INVALID_KERNEL:
		return "CL_INVALID_KERNEL";                                                                                             // break;
	case CL_INVALID_ARG_INDEX:
		return "CL_INVALID_ARG_INDEX";                                                                                          // break;
	case CL_INVALID_ARG_VALUE:
		return "CL_INVALID_ARG_VALUE";                                                                                          // break;
	case CL_INVALID_ARG_SIZE:
		return "CL_INVALID_ARG_SIZE";                                                                                           // break;
	case CL_INVALID_KERNEL_ARGS:
		return "CL_INVALID_KERNEL_ARGS";                                                                                        // break;
	case CL_INVALID_WORK_DIMENSION:
		return "CL_INVALID_WORK_DIMENSION";                                                                                     // break;
	case CL_INVALID_WORK_GROUP_SIZE:
		return "CL_INVALID_WORK_GROUP_SIZE";                                                                                    // break;
	case CL_INVALID_WORK_ITEM_SIZE:
		return "CL_INVALID_WORK_ITEM_SIZE";                                                                                     // break;
	case CL_INVALID_GLOBAL_OFFSET:
		return "CL_INVALID_GLOBAL_OFFSET";                                                                                      // break;
	case CL_INVALID_EVENT_WAIT_LIST:
		return "CL_INVALID_EVENT_WAIT_LIST";                                                                                    // break;
	case CL_INVALID_EVENT:
		return "CL_INVALID_EVENT";                                                                                              // break;
	case CL_INVALID_OPERATION:
		return "CL_INVALID_OPERATION";                                                                                          // break;
	case CL_INVALID_GL_OBJECT:
		return "CL_INVALID_GL_OBJECT";                                                                                          // break;
	case CL_INVALID_BUFFER_SIZE:
		return "CL_INVALID_BUFFER_SIZE";                                                                                        // break;
	case CL_INVALID_MIP_LEVEL:
		return "CL_INVALID_MIP_LEVEL";                                                                                          // break;
	case CL_INVALID_GLOBAL_WORK_SIZE:
		return "CL_INVALID_GLOBAL_WORK_SIZE";                                                                                   // break;
	case CL_INVALID_PROPERTY:
		return "CL_INVALID_PROPERTY";                                                                                           // break;
	default:
		return "UNKNOWN";                                                                                                       // break;
	}
}

inline void cldump(bool verbose) {
	try {
		std::vector<cl::Platform> platforms;
		(void)cl::Platform::get(&platforms);
		cout << "Number of platforms: " << platforms.size() << "\n";

		// dump platform information
		for (auto platform = platforms.begin(); platform != platforms.end(); ++platform) {
			std::vector<cl::Device> devices;
			(void)platform->getDevices(CL_DEVICE_TYPE_ALL, &devices);
			cout << "Number of devices: " << devices.size() << "\n";

			if (verbose) P(platform, CL_PLATFORM_PROFILE);
			P(platform, CL_PLATFORM_VERSION);
			P(platform, CL_PLATFORM_NAME);
			P(platform, CL_PLATFORM_VENDOR);
			if (verbose) P(platform, CL_PLATFORM_EXTENSIONS);

			// dump device information
			for (auto device = devices.begin(); device != devices.end(); ++device) {
				P(device, CL_DEVICE_NAME);
				P(device, CL_DEVICE_VENDOR);
				P(device, CL_DRIVER_VERSION);
				if (verbose) P(device, CL_DEVICE_PROFILE);
				P(device, CL_DEVICE_VERSION);
				P(device, CL_DEVICE_OPENCL_C_VERSION);

				PbitmapStart(device, CL_DEVICE_TYPE);
				PbitmapTest(CL_DEVICE_TYPE_CPU);
				PbitmapTest(CL_DEVICE_TYPE_GPU);
				PbitmapTest(CL_DEVICE_TYPE_ACCELERATOR);
				PbitmapTest(CL_DEVICE_TYPE_DEFAULT);
				PbitmapEnd;

				P(device, CL_DEVICE_VENDOR_ID);
				Pmb(device, CL_DEVICE_MAX_MEM_ALLOC_SIZE);
				Pmb(device, CL_DEVICE_GLOBAL_MEM_SIZE);
				Pkb(device, CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE);
				Pkb(device, CL_DEVICE_LOCAL_MEM_SIZE);
				P(device, CL_DEVICE_MAX_COMPUTE_UNITS);
				P(device, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS);
				P(device, CL_DEVICE_MAX_WORK_GROUP_SIZE);
				std::vector<size_t> sizes = device->getInfo<CL_DEVICE_MAX_WORK_ITEM_SIZES>();
				cout << "CL_DEVICE_MAX_WORK_ITEM_SIZES: ";
				for (auto size = sizes.begin(); size != sizes.end(); ++size) { cout << *size << " "; }
				cout << "\n";
				
				P(device, CL_DEVICE_MAX_CONSTANT_ARGS);
				PconstStart(device, CL_DEVICE_LOCAL_MEM_TYPE);
				PconstTest(CL_NONE);
				PconstTest(CL_LOCAL);
				PconstTest(CL_GLOBAL);
				PconstEnd;


				if (verbose) {
					P(device, CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR);
					P(device, CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT);
					P(device, CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT);
					P(device, CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG);
					P(device, CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT);
					P(device, CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE);
					P(device, CL_DEVICE_PREFERRED_VECTOR_WIDTH_HALF);
					P(device, CL_DEVICE_NATIVE_VECTOR_WIDTH_CHAR);
					P(device, CL_DEVICE_NATIVE_VECTOR_WIDTH_SHORT);
					P(device, CL_DEVICE_NATIVE_VECTOR_WIDTH_INT);
					P(device, CL_DEVICE_NATIVE_VECTOR_WIDTH_LONG);
					P(device, CL_DEVICE_NATIVE_VECTOR_WIDTH_FLOAT);
					P(device, CL_DEVICE_NATIVE_VECTOR_WIDTH_DOUBLE);
					P(device, CL_DEVICE_NATIVE_VECTOR_WIDTH_HALF);
					P(device, CL_DEVICE_MAX_CLOCK_FREQUENCY);
					P(device, CL_DEVICE_ADDRESS_BITS);
					Pbool(device, CL_DEVICE_IMAGE_SUPPORT);
					P(device, CL_DEVICE_MAX_READ_IMAGE_ARGS);
					P(device, CL_DEVICE_MAX_WRITE_IMAGE_ARGS);
					P(device, CL_DEVICE_IMAGE2D_MAX_WIDTH);
					P(device, CL_DEVICE_IMAGE2D_MAX_HEIGHT);
					P(device, CL_DEVICE_IMAGE3D_MAX_WIDTH);
					P(device, CL_DEVICE_IMAGE3D_MAX_HEIGHT);
					P(device, CL_DEVICE_IMAGE3D_MAX_DEPTH);
					P(device, CL_DEVICE_MAX_SAMPLERS);
					P(device, CL_DEVICE_MAX_PARAMETER_SIZE);
					P(device, CL_DEVICE_MEM_BASE_ADDR_ALIGN);
					PbitmapStart(device, CL_DEVICE_SINGLE_FP_CONFIG);
					PbitmapTest(CL_FP_DENORM);
					PbitmapTest(CL_FP_INF_NAN);
					PbitmapTest(CL_FP_ROUND_TO_NEAREST);
					PbitmapTest(CL_FP_ROUND_TO_ZERO);
					PbitmapTest(CL_FP_ROUND_TO_INF);
					PbitmapTest(CL_FP_FMA);
					//PbitmapTest(CL_FP_CORRECTLY_ROUNDED_DIVIDE_SQRT);
					PbitmapTest(CL_FP_SOFT_FLOAT);
					PbitmapEnd;
					PbitmapStart(device, CL_DEVICE_DOUBLE_FP_CONFIG);
					PbitmapTest(CL_FP_DENORM);
					PbitmapTest(CL_FP_INF_NAN);
					PbitmapTest(CL_FP_ROUND_TO_NEAREST);
					PbitmapTest(CL_FP_ROUND_TO_ZERO);
					PbitmapTest(CL_FP_ROUND_TO_INF);
					PbitmapTest(CL_FP_FMA);
					//PbitmapTest(CL_FP_CORRECTLY_ROUNDED_DIVIDE_SQRT);
					PbitmapTest(CL_FP_SOFT_FLOAT);
					PbitmapEnd;
					PconstStart(device, CL_DEVICE_GLOBAL_MEM_CACHE_TYPE);
					PconstTest(CL_NONE);
					PconstTest(CL_READ_ONLY_CACHE);
					PconstTest(CL_READ_WRITE_CACHE);
					PconstEnd;
					P(device, CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE);
					P(device, CL_DEVICE_GLOBAL_MEM_CACHE_SIZE);

					Pbool(device, CL_DEVICE_ERROR_CORRECTION_SUPPORT);
					Pbool(device, CL_DEVICE_HOST_UNIFIED_MEMORY);
					P(device, CL_DEVICE_PROFILING_TIMER_RESOLUTION);
					Pbool(device, CL_DEVICE_ENDIAN_LITTLE);
					Pbool(device, CL_DEVICE_AVAILABLE);
					Pbool(device, CL_DEVICE_COMPILER_AVAILABLE);
					PbitmapStart(device, CL_DEVICE_EXECUTION_CAPABILITIES);
					PbitmapTest(CL_EXEC_KERNEL);
					PbitmapTest(CL_EXEC_NATIVE_KERNEL);
					PbitmapEnd;
					PbitmapStart(device, CL_DEVICE_QUEUE_PROPERTIES);
					PbitmapTest(CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE);
					PbitmapTest(CL_QUEUE_PROFILING_ENABLE);
					PbitmapEnd;
					P(device, CL_DEVICE_PLATFORM);
					P(device, CL_DEVICE_EXTENSIONS);
				}
			}
		}
	}
	catch (...) {
		std::cerr << "ERROR!\n";
	}
}

#define CL_CHECK_ERROR(err) \
	{                       \
		if (err != CL_SUCCESS) {                  \
			std::cerr << "Error: "              \
			<< CLErrorString(err)     \
			<< " in " << __FILE__     \
			<< " line " << __LINE__   \
			<< std::endl; }             \
	}
