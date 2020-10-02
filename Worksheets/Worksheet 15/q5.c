#include <stdlib.h>

int main() {
    // creates an array of integers called data of size 100 using malloc
    int *data = (int *) malloc(sizeof(int) * 100); // Array of 100 integers
    // set data[100] to zero
    data[100] = 0;
    free(data); // Free the allocated space. If we don't, it causes a memory leak.
    return 0;
}
// What happens when you run this program?
/*
 * There are no errors when we run this program, but we are trying to access an unallocated index.
 * We create a pointer to array *data and then we allocate space in memory for it for indices 0 to 99 inclusive.
 * We then set data[100] to 0, but index 100 is not allocated, causing an error when we use valgrind.
 * At the end, we free the allocated data.
 */

// What happens when you run this program using valgrind?
/*
 * We will get an error (Invalid write) because we are accessing an unallocated index in array data.
 * We allocated the array for indices 0 to 99.
 * Index 100 is unallocated, so it will cause an error since we are trying to set data[100] to 0.
 * This causes an invalid write error in memory as we are accessing unallocated memory.
 * Here is the report:

==1831== Memcheck, a memory error detector
==1831== Copyright (C) 2002-2017, and GNU GPL'd, by Julian Seward et al.
==1831== Using Valgrind-3.16.1 and LibVEX; rerun with -h for copyright info
==1831== Command: ./a.out
==1831==
==1831== Invalid write of size 4
==1831==    at 0x109165: main (in /home/cs149/Desktop/Test/a.out)
==1831==  Address 0x4e4e1d0 is 0 bytes after a block of size 400 alloc'd
==1831==    at 0x4A34ECB: malloc (vg_replace_malloc.c:307)
==1831==    by 0x109156: main (in /home/cs149/Desktop/Test/a.out)
==1831==
==1831==
==1831== HEAP SUMMARY:
==1831==     in use at exit: 0 bytes in 0 blocks
==1831==   total heap usage: 1 allocs, 1 frees, 400 bytes allocated
==1831==
==1831== All heap blocks were freed -- no leaks are possible
==1831==
==1831== For lists of detected and suppressed errors, rerun with: -s
==1831== ERROR SUMMARY: 1 errors from 1 contexts (suppressed: 0 from 0)
 */

// Is the program correct?
/*
 * No because if we are to use data[100], we must first allocate space for it like the rest of the indices.
 * Then we can set data[100] to 0. We are accessing memory space for something other than the array data.
 */

