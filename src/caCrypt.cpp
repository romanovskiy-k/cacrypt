#include <fstream>
#include <iostream>
#include <limits.h>
#include <math.h>
#include <set>
#include <string.h>
#include <time.h>
#include <string>
#include <sstream>

#ifndef WIN32
#include <sys/time.h>
#endif

#include "Graph.h"
#include "cl.hpp"
#include "utils.h"

#ifdef WIN32
typedef unsigned long long int uint64_t;
#endif

using namespace std;

const size_t kBlockSize = 256;
const size_t kKeySize = 128;

//#define __DEBUG
#define __CHECK

void rts(cl_device_id device, cl_context context, cl_command_queue queue, Graph *G)
{
	srand(time(0));
	cl_int clError;

#ifdef WIN32
	const char * sourceFileName = "../../src/cellauto.cl";
#else
	const char * sourceFileName = "cellauto.cl";
#endif
	std::ifstream file(sourceFileName);
	CL_CHECK_ERROR(!file.is_open());
	std::string kernelSource(std::istreambuf_iterator<char>(file), (std::istreambuf_iterator<char>()));
	const char * source = kernelSource.c_str();
	cl_program program = clCreateProgramWithSource(context, 1, &source, NULL, &clError);
	CL_CHECK_ERROR(clError);

	const char *cFlags = "";
	clError = clBuildProgram(program, 1, &device, cFlags, NULL, NULL);
	if (clError != CL_SUCCESS) {
		size_t buildLogSize = 0;
		std::cout << "Kernel compilation error:\n";
		clError = clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &buildLogSize);
		char * buildLog = new char[buildLogSize + 1];
		clError = clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, buildLogSize, buildLog, NULL);
		buildLog[buildLogSize] = '\0';
		std::cout << " : " << buildLog << "\n";
		delete[] buildLog;
		return;
	}
	CL_CHECK_ERROR(clError);

	cl_uint *adjacencyList = G->GetEdgeList();
	cl_uint adjacencyListLength = G->GetAdjacencyListLength();
	cl_uint vertexCount = G->GetNumVertices();

#ifdef __DEBUG
	unsigned int *adjacencyListCopy = new unsigned int[adjacencyListLength];
	cl_mem d_adjacencyListCopy;
#endif
	cl_mem d_adjacencyList;
	cl_mem d_plainText, d_cipherText, d_key, d_constant;

	const size_t workGroupSize = 256;
	const size_t dataSize = 1024 * 1024 * 50;
	const size_t constantSize = vertexCount - kBlockSize / 2 - kKeySize / 2;
	unsigned char *plainText = new unsigned char[dataSize];
	unsigned char *cipherText = new unsigned char[dataSize];
	unsigned char *key = new unsigned char[kKeySize];
	unsigned char *constant = new unsigned char[constantSize];
	for (size_t i = 0; i < dataSize; ++i)
		plainText[i] = rand() % 2;
	for (size_t i = 0; i < kKeySize; ++i)
		key[i] = rand() % 2;
	for (size_t i = 0; i < constantSize; ++i)
		constant[i] = rand() % 2;

#ifdef __DEBUG
	memset((void *)adjacencyListCopy, 0, adjacencyListLength * sizeof(unsigned int));
#endif
	memset((void *)cipherText, 0, dataSize * sizeof(unsigned char));
	printf("%f mbytes\n", dataSize / (1024. * 1024));

	// TODO: get edge count from graph
	unsigned int vertexEdgeCount = 6;
	unsigned int iterationCount = 12;

	// TODO: try to use COPY_HOST_PTR or USE_HOST_PTR or use pinned memory
	d_adjacencyList = clCreateBuffer(context, CL_MEM_READ_ONLY, adjacencyListLength * sizeof(cl_uint), NULL, &clError);
	CL_CHECK_ERROR(clError);
#ifdef __DEBUG
	d_adjacencyListCopy = clCreateBuffer(context, CL_MEM_READ_WRITE, adjacencyListLength * sizeof(cl_uint), NULL,
	                                     &clError);
	CL_CHECK_ERROR(clError);
#endif

	d_plainText = clCreateBuffer(context, CL_MEM_READ_ONLY, dataSize * sizeof(cl_uchar), NULL, &clError);
	CL_CHECK_ERROR(clError);
	d_cipherText = clCreateBuffer(context, CL_MEM_READ_WRITE, dataSize * sizeof(cl_uchar), NULL, &clError);
	CL_CHECK_ERROR(clError);
	d_key = clCreateBuffer(context, CL_MEM_READ_ONLY, kKeySize * sizeof(cl_uchar), NULL, &clError);
	CL_CHECK_ERROR(clError);
	d_constant = clCreateBuffer(context, CL_MEM_READ_ONLY, constantSize * sizeof(cl_uchar), NULL, &clError);
	CL_CHECK_ERROR(clError);

	cl_kernel kernel = clCreateKernel(program, "ecb_kernel", &clError);
	CL_CHECK_ERROR(clError)

	//index for kernel parameters
	int p = -1;
	clError = clSetKernelArg(kernel, ++p, sizeof(cl_mem), (void *)&d_plainText);
	CL_CHECK_ERROR(clError);
	clError = clSetKernelArg(kernel, ++p, sizeof(cl_mem), (void *)&d_cipherText);
	CL_CHECK_ERROR(clError);
	clError = clSetKernelArg(kernel, ++p, sizeof(cl_mem), (void *)&d_key);
	CL_CHECK_ERROR(clError);
	clError = clSetKernelArg(kernel, ++p, sizeof(cl_mem), (void *)&d_constant);
	CL_CHECK_ERROR(clError);
	clError = clSetKernelArg(kernel, ++p, sizeof(cl_mem), (void *)&d_adjacencyList);
	CL_CHECK_ERROR(clError);
#ifdef __DEBUG
	clError = clSetKernelArg(kernel, ++p, sizeof(cl_mem), (void *)&d_adjacencyListCopy);
	CL_CHECK_ERROR(clError);
#endif
	// clError = clSetKernelArg(kernel, ++p, sizeof(cl_uint) * adjacencyListLength, NULL);
	// CL_CHECK_ERROR(clError);
	clError = clSetKernelArg(kernel, ++p, sizeof(cl_uint), (void *)&vertexCount);
	CL_CHECK_ERROR(clError);
	clError = clSetKernelArg(kernel, ++p, sizeof(cl_uint), (void *)&vertexEdgeCount);
	CL_CHECK_ERROR(clError);

	clError = clSetKernelArg(kernel, ++p, sizeof(cl_uint), (void *)&kBlockSize);
	CL_CHECK_ERROR(clError);
	clError = clSetKernelArg(kernel, ++p, sizeof(cl_uint), (void *)&kKeySize);
	CL_CHECK_ERROR(clError);
	clError = clSetKernelArg(kernel, ++p, sizeof(cl_uint), (void *)&constantSize);
	CL_CHECK_ERROR(clError);


	clError = clEnqueueWriteBuffer(queue, d_adjacencyList, CL_TRUE, 0, adjacencyListLength * sizeof(cl_uint),
	                               (void *)adjacencyList, 0,
	                               NULL, NULL);
	CL_CHECK_ERROR(clError);

	clError = clEnqueueWriteBuffer(queue, d_plainText, CL_TRUE, 0, (dataSize) * sizeof(cl_uchar), (void *)plainText, 0,
	                               NULL, NULL);
	CL_CHECK_ERROR(clError);
	clError = clEnqueueWriteBuffer(queue, d_key, CL_TRUE, 0, (kKeySize) * sizeof(cl_uchar), (void *)key, 0, NULL, NULL);
	CL_CHECK_ERROR(clError);
	clError = clEnqueueWriteBuffer(queue, d_constant, CL_TRUE, 0, (constantSize) * sizeof(cl_uchar), (void *)constant,
	                               0, NULL, NULL);
	CL_CHECK_ERROR(clError);

	std::cout << "Running Benchmark\n";
	size_t gws = dataSize, lws = 256;
	cl_event profilingEvent;

	clError = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &gws, &lws, 0, NULL, &profilingEvent);
	CL_CHECK_ERROR(clError);
	clFinish(queue);
	CL_CHECK_ERROR(clError);

	clError = clEnqueueReadBuffer(queue, d_cipherText, CL_TRUE, 0, dataSize * sizeof(cl_uchar), cipherText, 0, NULL,
	                              NULL);
	CL_CHECK_ERROR(clError);
#ifdef __DEBUG
	clError = clEnqueueReadBuffer(queue, d_adjacencyListCopy, CL_TRUE, 0, adjacencyListLength * sizeof(cl_uint),
	                              adjacencyListCopy, 0, NULL,
	                              NULL);
	CL_CHECK_ERROR(clError);
#endif

	uint64_t start = 0, end = 0;
	clError = clGetEventProfilingInfo(profilingEvent, CL_PROFILING_COMMAND_START, sizeof(uint64_t), &start, NULL);
	CL_CHECK_ERROR(clError);
	clError = clGetEventProfilingInfo(profilingEvent, CL_PROFILING_COMMAND_END, sizeof(uint64_t), &end, NULL);
	CL_CHECK_ERROR(clError);
	double kernelTime = (double)(end - start) / 1e9;
	printf("Profiling: Total kernel time was %5.2f secs.\n", kernelTime);
	double bandwidth = dataSize / (kernelTime * 1024 * 1024);
	printf("Bandwidth: %f MBits\n", bandwidth);

#ifdef __DEBUG
	for (size_t i = 0; i < 512; ++i)
		printf("%d ", adjacencyListCopy[i]);
	printf("\n\n");
#endif
#ifdef __CHECK
	for (size_t i = 0; i < 512; ++i)
		printf("%d ", cipherText[i]);

#endif
	printf("\n");
	clError = clReleaseKernel(kernel);
	CL_CHECK_ERROR(clError);

	clError = clReleaseMemObject(d_plainText);
	CL_CHECK_ERROR(clError);
	clError = clReleaseMemObject(d_cipherText);
	CL_CHECK_ERROR(clError);
	clError = clReleaseMemObject(d_adjacencyList);
	CL_CHECK_ERROR(clError);

	clError = clReleaseProgram(program);
	CL_CHECK_ERROR(clError);
}

void RunBenchmark(
    cl::Device& devcpp,
    cl::Context& ctxcpp,
    cl::CommandQueue& queuecpp)
{
	//Get device context and command queue
	cl_device_id device = devcpp();
	cl_context context = ctxcpp();
	cl_command_queue queue = queuecpp();

	//Get the graph filename
#ifdef WIN32
	string inFileName = "../../graph-metis.ca";
#else 
	string inFileName = "../graph-metis.ca";
#endif

	//Create graph
	Graph *G = new Graph();

	//Load metis graph
	G->LoadMetisGraph(inFileName.c_str());

	std::cout << "Vertices: " << G->GetNumVertices() << endl;
	std::cout << "Edges: " << G->GetNumEdges() << endl;
	rts(device, context, queue, G);

	delete G;
}

