#include <iostream>
#include <stdlib.h>
#include <string>
#define __CL_ENABLE_EXCEPTIONS
#include "cl.hpp"
#include "utils.h"

using namespace std;

void RunBenchmark(cl::Device& dev, cl::Context& ctx, cl::CommandQueue& queue);

int main(int argc, char *argv[])
{
	const bool verbose = false;
	cldump(verbose);
	int ret = 0;
	try {
		bool output = true;
		// Initialization
		if (verbose) {
			cout << ">> initializing\n";
		}

		cl_int err;
		vector<cl::Platform> platforms;
		err = cl::Platform::get( &platforms );
		CL_CHECK_ERROR(err);

		long nPlatforms = (long)platforms.size();
		if (nPlatforms <= 0) {
			cerr << "No OpenCL platforms found. Exiting." << endl;
			exit(0);
		}
		int platform = 0;
		if (platform < 0 || platform >= nPlatforms) {
			// platform ID out of range
			cerr << "Platform index " << platform << " is out of range. "
			<< "Specify a platform index between 0 and "
			<< nPlatforms - 1 << endl;
			exit(-4);
		}

		string platformName = platforms[platform].getInfo<CL_PLATFORM_NAME>();
		string platformVersion = platforms[platform].getInfo<CL_PLATFORM_VERSION>();

		// query devices
		std::vector<cl::Device> devs;
		err = platforms[platform].getDevices( CL_DEVICE_TYPE_GPU, &devs );
		// CL_CHECK_ERROR( err ); check for error later

		long nDevs = (long)devs.size();
		if (nDevs <= 0) {
			cerr << "No OpenCL devices found. Exiting." << endl;
			exit(0);
		}
		int device = 0;
		if (device < 0 || device >= nDevs) {                                         // platform ID out of range
			cerr << "Device index " << device << " is out of range. "
			<< "Specify a device index between 0 and " << nDevs - 1
			<< endl;
			exit(-5);
		}

		cl::Device &clDevice = devs[0];
		cl_device_id dev_id = clDevice();
		if (output) {
			cout << "Chose device:"
			<< " name='" << clDevice.getInfo<CL_DEVICE_NAME>() << "'"
			<< " index=" << device
			<< " id=" << dev_id
			<< endl;
		}

		cl::Device id = dev_id;
		std::vector<cl::Device> ctxDevices;
		ctxDevices.push_back( id );
		cl::Context ctx( ctxDevices );
		cl::CommandQueue queue( ctx, id, CL_QUEUE_PROFILING_ENABLE );

		// Run the benchmark
		RunBenchmark(id, ctx, queue);
	}catch (cl::Error e) {
		std::cerr << e.what() << '(' << e.err() << ')' << std::endl;

		ret = 1;
	}catch (std::exception & e) {
		std::cerr << e.what() << std::endl;

		ret = 1;
	}catch (...) {
		std::cerr << "unrecognized exception caught" << std::endl;

		ret = 1;
	}
	return ret;
}

