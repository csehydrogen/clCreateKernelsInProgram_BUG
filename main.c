#include <stdio.h>
#include <stdlib.h>
#include <CL/cl.h>

#define CHECK_ERROR(err) \
    if (err != CL_SUCCESS) { \
        printf("[%s:%d] OpenCL error %d\n", __FILE__, __LINE__, err); \
        exit(EXIT_FAILURE); \
    }

cl_int err;

cl_program create_and_build_program(cl_context context, cl_device_id device, const char *file_name) {
    FILE *file = fopen(file_name, "rb");
    if (file == NULL) {
        printf("Failed to open %s\n", file_name);
        exit(EXIT_FAILURE);
    }
    fseek(file, 0, SEEK_END);
    size_t source_size = ftell(file);
    rewind(file);
    char *source_code = (char*)malloc(source_size + 1);
    fread(source_code, sizeof(char), source_size, file);
    source_code[source_size] = '\0';
    fclose(file);

    cl_program program = clCreateProgramWithSource(context, 1, (const char**)&source_code, &source_size, &err);
    CHECK_ERROR(err);
    free(source_code);

    err = clBuildProgram(program, 1, &device, "", NULL, NULL);
    if (err == CL_BUILD_PROGRAM_FAILURE) {
        size_t log_size;
        CHECK_ERROR(clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size));
        char *log = (char*)malloc(log_size + 1);
        CHECK_ERROR(clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, log_size, log, NULL));
        log[log_size] = 0;
        printf("Compile error:\n%s\n", log);
        free(log);
    }
    CHECK_ERROR(err);

    return program;
}

int main() {
    cl_platform_id platform;
    err = clGetPlatformIDs(1, &platform, NULL);
    CHECK_ERROR(err);

    cl_uint num_devices;
    err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 0, NULL, &num_devices);
    CHECK_ERROR(err);

    cl_device_id devices[num_devices];
    err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, num_devices, devices, NULL);
    CHECK_ERROR(err);

    cl_context context;
    context = clCreateContext(NULL, num_devices, devices, NULL, NULL, &err);
    CHECK_ERROR(err);

    for (int i = 0; i < num_devices; ++i) {
        cl_program program = create_and_build_program(context, devices[i], "kernel.cl");

        cl_uint num_kernels_ret;
        err = clCreateKernelsInProgram(program, 0, NULL, &num_kernels_ret);

        printf("devices[%d], ", i);
        if (err != CL_SUCCESS) {
            printf("err = %d", err);
        } else {
            printf("num_kernels_ret = %u", num_kernels_ret);
        }
        printf("\n");

        clReleaseProgram(program);
    }

    clReleaseContext(context);

    return 0;
}
