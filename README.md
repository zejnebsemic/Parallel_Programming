This assignment focused on testing the behavior and portability of several parallel programming models using the StreamTriad example from the Week-10 repository. After cloning the project, I moved through each model and attempted to compile and run the provided implementations, observing which ones worked correctly on the lab machine and which ones did not. I first started in the OpenACC directory, where all versions of the program—both the parallel ones and the kernel-based examples—ran successfully. My terminal outputs looked like this:

student@itcenter-lab128:~/Desktop/ParallelComputing-week-10/openacc$ ./StreamTriad
Average runtime for stream triad loop is 0.079663 secs
student@itcenter-lab128:~/Desktop/ParallelComputing-week-10/openacc$ ./StreamTriad_par1
Average runtime for stream triad loop is 0.072580 secs
student@itcenter-lab128:~/Desktop/ParallelComputing-week-10/openacc$ ./StreamTriad_par2
Average runtime for stream triad loop is 0.073095 secs
student@itcenter-lab128:~/Desktop/ParallelComputing-week-10/openacc$ ./StreamTriad_par3
Average runtime for stream triad loop is 0.072339 secs
student@itcenter-lab128:~/Desktop/ParallelComputing-week-10/openacc$ ./StreamTriad_par4
Average runtime for stream triad loop is 0.072016 secs
student@itcenter-lab128:~/Desktop/ParallelComputing-week-10/openacc$ ./StreamTriad_kern1
Average runtime for stream triad loop is 0.072126 secs
student@itcenter-lab128:~/Desktop/ParallelComputing-week-10/openacc$ ./StreamTriad_kern2
Average runtime for stream triad loop is 0.073978 secs
student@itcenter-lab128:~/Desktop/ParallelComputing-week-10/openacc$ ./StreamTriad_kern3
Average runtime for stream triad loop is 0.072855 secs

All of them produced valid outputs and very similar runtimes. Even though some versions were written using OpenACC kernels meant for GPU execution, they were in fact executed on the CPU. The reason is that the GCC OpenACC compiler on this machine automatically falls back to CPU execution when no supported GPU is available. This fallback mechanism allowed everything to run normally but without GPU acceleration. It demonstrates OpenACC’s portability: the code works even when the hardware is missing.

When I tested OpenMP, the repository did not contain the expected StreamTriad_omp source file. Attempting to compile it resulted in an error saying the file does not exist. Even if the file had been included, GPU offloading would not have worked because the environment lacks an NVIDIA GPU and the required NVPTX backend. OpenMP itself remains portable for CPU multithreading, but GPU support depends heavily on hardware.

For OpenCL, I was able to compile the code, but running it failed immediately with an error during the device discovery step. The clinfo tool confirmed that the system had zero OpenCL platforms installed, meaning neither CPU nor GPU OpenCL drivers were present. Without any actual OpenCL devices, the runtime cannot launch kernels, so the program stops before doing any meaningful work. This shows that OpenCL is theoretically portable but practically dependent on drivers.

The CUDA version compiled without issues but completely failed at runtime. The output repeatedly reported that all computed values were zero across every iteration. This happens when CUDA kernels fail silently due to the absence of an NVIDIA GPU. Since no valid CUDA device exists on this machine, kernel launches never actually execute and memory transfers do not produce correct results. This demonstrates that CUDA offers strong performance only when the right hardware is present, but it is the least portable model among those tested.

Overall, the only implementations that produced correct numerical results were those capable of falling back to CPU execution, such as the OpenACC versions including the kernel variants. OpenCL and CUDA were unable to run due to missing hardware or drivers, and OpenMP could not be tested due to a missing file. This experiment clearly highlights the difference between theoretical portability and practical portability across CPU-only and GPU-capable systems.

![Results](omp/images/kerns.png)
On this screenshot, all OpenACC programs ran successfully with similar execution times because the compiler falls back to CPU parallelization since no GPU is available.


![Results](omp/images/cuda1.png)

![Results](omp/images/cuda2.png)

![Results](omp/images/cuda3.png)

![Results](omp/images/cuda4.png)
On this screenshot, both OpenCL and CUDA programs fail: OpenCL cannot find any GPU device (no OpenCL runtime), while CUDA runs but produces all-zero results due to incorrect memory copying between GPU and CPU.


![Results](omp/images/openmp.png)
On this screenshot, OpenMP compilation fails because the required source file StreamTriad_omp.c is missing from the directory, preventing any GPU offload attempt.

