# clCreateKernelsInProgram_BUG

1. Create NVIDIA multi-GPU context.
1. Build program for one device.
1. Call clCreateKernelsInProgram, which results in CL_INVALID_KERNEL_DEFINITION, except the first GPU.
