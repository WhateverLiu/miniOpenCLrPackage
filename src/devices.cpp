# include <Rcpp.h>
using namespace Rcpp;
# define VIENNACL_WITH_OPENCL 1


#include <viennacl/ocl/backend.hpp>
#include <viennacl/ocl/platform.hpp>




// extern "C"
// {
    void gpuExp(int *A, int *B, int *C, int *sizePtr)
    {
        int LIST_SIZE = *sizePtr;


        // Load the kernel source code into the array source_str
        FILE *fp;
        char *source_str;
        size_t source_size;


        fp = fopen("src/kernels/vector_add_kernel.txt", "r");
        if (!fp)
        {
            // fprintf(stderr, "Failed to load kernel.\n");
            Rprintf("%s", "Failed to load kernel.\n");
            return;
        }
        source_str = (char*)malloc(8192);
        source_size = fread( source_str, 1, 8192, fp);
        fclose( fp );


        // Get platform and device information
        cl_platform_id platform_id = NULL;
        cl_device_id device_id = NULL;
        cl_uint ret_num_devices;
        cl_uint ret_num_platforms;
        cl_int ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
        ret = clGetDeviceIDs( platform_id, CL_DEVICE_TYPE_DEFAULT, 1,
                              &device_id, &ret_num_devices);


        // Create an OpenCL context
        Rprintf("device_id = %d\n", device_id);
        Rprintf("platform_id = %d\n", platform_id);
        Rprintf("ret_num_devices = %d\n", ret_num_devices);
        Rprintf("ret_num_platforms = %d\n", ret_num_platforms);


        cl_context context = clCreateContext( NULL, 1, &device_id, NULL, NULL, &ret);


        // Create a command queue
        // cl_command_queue command_queue = clCreateCommandQueue(context, device_id, 0, &ret);
        // clCreateCommandQueue is deprecated as of OpenCL 2.0
        cl_command_queue command_queue = clCreateCommandQueueWithProperties(context, device_id, NULL, &ret);

        // Create memory buffers on the device for each vector
        cl_mem a_mem_obj = clCreateBuffer(context, CL_MEM_READ_WRITE,
                                          LIST_SIZE * sizeof(int), NULL, &ret);
        cl_mem b_mem_obj = clCreateBuffer(context, CL_MEM_READ_WRITE,
                                          LIST_SIZE * sizeof(int), NULL, &ret);
        cl_mem c_mem_obj = clCreateBuffer(context, CL_MEM_READ_WRITE,
                                          LIST_SIZE * sizeof(int), NULL, &ret);


        // Copy the lists A and B to their respective memory buffers
        ret = clEnqueueWriteBuffer(command_queue, a_mem_obj, CL_TRUE, 0,
                                   LIST_SIZE * sizeof(int), A, 0, NULL, NULL);
        ret = clEnqueueWriteBuffer(command_queue, b_mem_obj, CL_TRUE, 0,
                                   LIST_SIZE * sizeof(int), B, 0, NULL, NULL);


        // Create a program from the kernel source
        cl_program program = clCreateProgramWithSource(
            context, 1, (const char **)&source_str, (const size_t *)&source_size, &ret);
        Rprintf("CL_SUCCESS = %d\n", CL_SUCCESS);
        Rprintf("program generated = %d\n", ret);


        Rprintf("%s\n\n", source_str);
        Rprintf("source_size = %d\n", source_size);


        // Build the program
        ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
        Rprintf("program compiled = %d\n", ret);


        // Create the OpenCL kernel
        cl_kernel kernel = clCreateKernel(program, "vector_add", &ret);


        // Set the arguments of the kernel
        ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&a_mem_obj);
        ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&b_mem_obj);
        ret = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&c_mem_obj);


        // Execute the OpenCL kernel on the list
        size_t global_item_size = LIST_SIZE; // Process the entire lists
        size_t local_item_size = 64; // Divide work items into groups of 64
        ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL,
                                     &global_item_size, &local_item_size, 0, NULL, NULL);


        // Read the memory buffer C on the device to the local variable C
        ret = clEnqueueReadBuffer(command_queue, c_mem_obj, CL_TRUE, 0,
                                  LIST_SIZE * sizeof(int), C, 0, NULL, NULL);


        // Clean up
        ret = clFlush(command_queue);
        ret = clFinish(command_queue);
        ret = clReleaseKernel(kernel);
        ret = clReleaseProgram(program);
        ret = clReleaseMemObject(a_mem_obj);
        ret = clReleaseMemObject(b_mem_obj);
        ret = clReleaseMemObject(c_mem_obj);
        ret = clReleaseCommandQueue(command_queue);
        ret = clReleaseContext(context);
    }
// }




// [[Rcpp::export]]
IntegerVector gpuExpCpp(IntegerVector x, IntegerVector y)
{
    IntegerVector z(x.size());
    int xsize = z.size();
    gpuExp(&x[0], &y[0], &z[0], &xsize);
    return z;
}










































