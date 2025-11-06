
After I added the flags -fno-trapping-math and -fno-math-errno to the Makefile, vectorization started working properly. For example, when running /stream_opt3, the performance counters show:

Almost all double-precision floating-point operations are now vectorized.

The vectorization ratio reaches 99.9999%, indicating that nearly all relevant instructions are executed using SIMD instructions.

Packed operations (FP_COMP_OPS_EXE_SSE_FP_PACKED_DOUBLE) dominate, while scalar operations are almost zero.

This confirms that the compiler is able to safely vectorize the code when these flags are enabled.

./stream_opt3
--------------------------------------------------------------------------------
CPU name:       Intel(R) Core(TM) i3-2120 CPU @ 3.30GHz
CPU type:       Intel Core SandyBridge processor
CPU clock:      3.29 GHz
--------------------------------------------------------------------------------
Minimum dt is 0.016964
--------------------------------------------------------------------------------
Group 1: FLOPS_DP
+--------------------------------------+---------+------------+
|                 Event                | Counter | HWThread 0 |
+--------------------------------------+---------+------------+
|           INSTR_RETIRED_ANY          |  FIXC0  |  137751733 |
|         CPU_CLK_UNHALTED_CORE        |  FIXC1  |  528876648 |
|         CPU_CLK_UNHALTED_REF         |  FIXC2  |  528875886 |
| FP_COMP_OPS_EXE_SSE_FP_PACKED_DOUBLE |   PMC0  |   45205496 |
| FP_COMP_OPS_EXE_SSE_FP_SCALAR_DOUBLE |   PMC1  |         26 |
|       SIMD_FP_256_PACKED_DOUBLE      |   PMC2  |          5 |
+--------------------------------------+---------+------------+

+-------------------------+--------------+
|          Metric         |  HWThread 0  |
+-------------------------+--------------+
|   Runtime (RDTSC) [s]   |       0.4532 |
|   Runtime unhalted [s]  |       0.1606 |
|       Clock [MHz]       |    3292.2942 |
|           CPI           |       3.8393 |
|       DP [MFLOP/s]      |     199.4842 |
|     AVX DP [MFLOP/s]    | 4.412829e-05 |
|     Packed [MUOPS/s]    |      99.7421 |
|     Scalar [MUOPS/s]    |       0.0001 |
| Vectorization ratio [%] |      99.9999 |
+-------------------------+--------------+



For this assignment, I worked on improving the Example_03 code from the official repository. I created three versions of the program: timestep_opt1.c, timestep_opt2.c, and timestep_opt3.c. Each version includes incremental changes aimed at enabling vectorization and adding OpenMP directives. The programs were compiled using GCC with the optimization flags -O3 and -fopenmp. Additionally, timestep_opt3.c was compiled with the flags -fno-trapping-math and -fno-math-errno to allow safe vectorization.

All tests were executed on an Intel Core i3-2120 (Sandy Bridge) processor running at 3.29 GHz, using the LIKWID tool with the FLOPS_DP performance group.

The measured results are as follows:

timestep_opt1: Runtime = 0.5179 s, DP [MFLOP/s] = 174.69, CPI = 3.89, Vectorization ratio = 0%

timestep_opt2: Runtime = 0.4821 s, DP [MFLOP/s] = 187.69, CPI = 3.83, Vectorization ratio = 0%

timestep_opt3: Runtime = 0.4994 s, DP [MFLOP/s] = 181.26, CPI = 3.88, Vectorization ratio = 99.9999%

From these results, it is clear that vectorization was enabled only in timestep_opt3, as indicated by the vectorization ratio of 99.9999%. The earlier versions (opt1 and opt2) were not vectorized, likely due to data dependencies within the main loops. The use of -fno-trapping-math and -fno-math-errno in opt3 allowed the compiler to safely vectorize the code, even on the Sandy Bridge CPU, which has limited AVX double-precision support.

Although all versions are fairly close in performance, timestep_opt3 showed slightly better results in terms of runtime and MFLOP/s, likely due to minor optimizations unrelated to vectorization.