student@itcenter-lab128:~/Desktop/assignment-1$ make valgrind
valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./memory_demo
==9754== Memcheck, a memory error detector
==9754== Copyright (C) 2002-2017, and GNU GPL'd, by Julian Seward et al.
==9754== Using Valgrind-3.18.1 and LibVEX; rerun with -h for copyright info
==9754== Command: ./memory_demo
==9754== 
==9754== Invalid write of size 4
==9754==    at 0x1091C6: main (main.c:7)
==9754==  Address 0x4a9e068 is 0 bytes after a block of size 40 alloc'd
==9754==    at 0x4848899: malloc (in /usr/libexec/valgrind/vgpreload_memcheck-amd64-linux.so)
==9754==    by 0x109185: main (main.c:4)
==9754== 
==9754== Conditional jump or move depends on uninitialised value(s)
==9754==    at 0x1091F4: main (main.c:9)
==9754==  Uninitialised value was created by a stack allocation
==9754==    at 0x109169: main (main.c:2)
==9754== 
==9754== Invalid read of size 4
==9754==    at 0x1091EF: main (main.c:9)
==9754==  Address 0x4a9e068 is 0 bytes after a block of size 40 alloc'd
==9754==    at 0x4848899: malloc (in /usr/libexec/valgrind/vgpreload_memcheck-amd64-linux.so)
==9754==    by 0x109185: main (main.c:4)
==9754== 
==9754== 
==9754== HEAP SUMMARY:
==9754==     in use at exit: 40 bytes in 1 blocks
==9754==   total heap usage: 1 allocs, 0 frees, 40 bytes allocated
==9754== 
==9754== 40 bytes in 1 blocks are definitely lost in loss record 1 of 1
==9754==    at 0x4848899: malloc (in /usr/libexec/valgrind/vgpreload_memcheck-amd64-linux.so)
==9754==    by 0x109185: main (main.c:4)
==9754== 
==9754== LEAK SUMMARY:
==9754==    definitely lost: 40 bytes in 1 blocks
==9754==    indirectly lost: 0 bytes in 0 blocks
==9754==      possibly lost: 0 bytes in 0 blocks
==9754==    still reachable: 0 bytes in 0 blocks
==9754==         suppressed: 0 bytes in 0 blocks
==9754== 
==9754== For lists of detected and suppressed errors, rerun with: -s
==9754== ERROR SUMMARY: 14 errors from 4 contexts (suppressed: 0 from 0)

Explanation


// int ipos=-1, ival=0; 
In line 6 I just initialized variables because they were not initialized and ipos is for saving position and ival is for saving the input value. Before I initialized this error Valgrind reported: ==9754== Conditional jump or move depends on uninitialised value(s)

// for (int i = 0; i<10; i++); 
In line 7 the problem was that the program tried to write outsided array. The loop was trying to iterate to 10 because it said i<=10 but that would mean there are 11 elements in array and we need only 10, so I corrected to i<10.

//  for (int i = 0; i<10; i++){ 
    iarray[i] = i; }  
    In lines 16 and 17 there is the same problem as previous one but only differece was the program tried to read memory that does not exist. Error that Valgrind reported: ==9754== Invalid read of size 4

//free(iarray); 
  return 0; 
In the code, memory was allocated but was never released, so in the line 26 I added free(iarray) for that purpose.This is done because Valgrid could show the leak of memory if it was not freed. Error: ==9754== 40 bytes in 1 blocks are definitely lost in loss record 1 of 1
==9754==    at 0x4848899: malloc (in /usr/libexec/valgrind/vgpreload_memcheck-amd64-linux.so)
==9754==    by 0x109185: main (main.c:4)














Corrected code:

student@itcenter-lab128:~/Desktop/assignment-1$ make valgrind
gcc -Wall -Wextra -g -std=c99 -o memory_demo main.c
main.c: In function ‘main’:
main.c:24:18: warning: format ‘%d’ expects a matching ‘int’ argument [-Wformat=]
   24 |   printf("Value %d found at position %d\n");//
      |                 ~^
      |                  |
      |                  int
main.c:24:39: warning: format ‘%d’ expects a matching ‘int’ argument [-Wformat=]
   24 |   printf("Value %d found at position %d\n");//
      |                                      ~^
      |                                       |
      |                                       int
main.c:6:7: warning: variable ‘ipos’ set but not used [-Wunused-but-set-variable]
    6 |   int ipos=-1, ival=0; //
      |       ^~~~
valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./memory_demo
==10428== Memcheck, a memory error detector
==10428== Copyright (C) 2002-2017, and GNU GPL'd, by Julian Seward et al.
==10428== Using Valgrind-3.18.1 and LibVEX; rerun with -h for copyright info
==10428== Command: ./memory_demo
==10428== 
Value -16778264 found at position 36
==10428== 
==10428== HEAP SUMMARY:
==10428==     in use at exit: 0 bytes in 0 blocks
==10428==   total heap usage: 2 allocs, 2 frees, 1,064 bytes allocated
==10428== 
==10428== All heap blocks were freed -- no leaks are possible
==10428== 
==10428== For lists of detected and suppressed errors, rerun with: -s
==10428== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)