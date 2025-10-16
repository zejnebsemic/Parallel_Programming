# Parallel_Programming

Example 1 - Cairo & Hwloc-picture named pic1Week3

Cairo is a 2D graphics library used for rendering high-quality images, shapes, and text across various formats (screens, PNG, PDF, etc.). It's a dependency for hwloc, which uses Cairo to visually represent hardware topology.

Hwloc is a system library that identifies your computer's hardware topology, including CPUs, caches, memory nodes, and I/O devices.

To install, the following dependencies were required: build-essential, pkg-config, libcairo2-dev, and others.

Cairo and hwloc were compiled from source using commands like ./configure, make, and make install.

Example 2 - STREAM Benchmarking - picture named pic2Week3

STREAM is a micro-benchmark used to measure memory bandwidth.

The benchmark was cloned from GitHub and compiled using make. Running ./stream_c.exe


Example 5 - CloverLeaf-picture named task4

CloverLeaf is a mini-app used to identify performance bottlenecks using Callgrind and Valgrind.

After installing Valgrind and KCacheGrind, a call graph for CloverLeaf was generated, and the tool helped identify hot spots for optimization.

Picture named task5 did not work well so it was not completed
