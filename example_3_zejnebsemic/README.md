Link for the sheet: https://docs.google.com/spreadsheets/d/1OVRZyuS9_sFOiQprMiJ4hdADWcRHoPPAQipJMlnpuvU/edit?usp=sharing

1.Changes I made in C++ file are:
    a. added everything that was missing for the program to work correctly and measure execution time properly.
    Specifically, I added dynamic memory allocation for the array of structures by adding
    this part SoA_type* AoSoA = new SoA_type[num_blocks]; so that the program can store data for each block.

2.The code that uses the rand() function to create the arrays R, G, and B with random values was uncommented and replaced. This ensures that the software does more than just a loop, it really does calculations.
AoSoA[j].R[i] = rand();
                AoSoA[j].G[i] = rand();
                AoSoA[j].B[i] = rand();

3.Added memory deallocation with delete[] AoSoA; for properly freeing the allocated memory and preventing memory leaks.


2.Changes in Makefile:
I just uncommented this line TARGET_SRC = aosoa_measurement.cpp and also I added correct C++ file.

3.The table shows the execution time in milliseconds for different dataset sizes like 1K, 10K, 100K, 1M, and 10M while varying the vector length V.The program's execution time for each dataset size is shown in the columns, while each row represents a single value of V.
These results are visually shown in the diagram below the table, which makes it simpler to understand how performance changes as the amount of data or vector size (V) rises.

![graph_image](/resources/Screenshot%20from%202025-10-23%2015-56-27.png)