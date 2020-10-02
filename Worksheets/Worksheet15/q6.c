#include <stdio.h>
#include <stdlib.h>

int main() {
    int *data = (int *) malloc(sizeof(int) * 3); // Array of 3 integers
    data[0] = 0;
    data[1] = 1;
    data[2] = 2;
    free(data); // Free the allocated array
    printf("data[1] = %d\n", data[1]);
    return 0;
}

// Does the program run?
/*
 * The program runs with no errors. But the value of output is "data[1] = 0"
 * when we defined it to 1. This is probably because in the line before, we deallocated/freed the memory
 * and the space is now null (therefore we see a 0).
 */

// What happens when you use valgrind on it?
/*
 * We see that we are attempting an invalid read. This is because we deallocated the address space
 * pointed by array data so now the space is uninitialized. We are attempting to read an address space which
 * was freed.
 *
==1897== Memcheck, a memory error detector
==1897== Copyright (C) 2002-2017, and GNU GPL'd, by Julian Seward et al.
==1897== Using Valgrind-3.16.1 and LibVEX; rerun with -h for copyright info
==1897== Command: ./a.out
==1897==
==1897== Invalid read of size 4
==1897==    at 0x1091A5: main (in /home/cs149/Desktop/Test/a.out)
==1897==  Address 0x4e4e044 is 4 bytes inside a block of size 12 free'd
==1897==    at 0x4A36078: free (vg_replace_malloc.c:538)
==1897==    by 0x10919C: main (in /home/cs149/Desktop/Test/a.out)
==1897==  Block was alloc'd at
==1897==    at 0x4A34ECB: malloc (vg_replace_malloc.c:307)
==1897==    by 0x109166: main (in /home/cs149/Desktop/Test/a.out)
==1897==
data[1] = 1
==1897==
==1897== HEAP SUMMARY:
==1897==     in use at exit: 0 bytes in 0 blocks
==1897==   total heap usage: 2 allocs, 2 frees, 1,036 bytes allocated
==1897==
==1897== All heap blocks were freed -- no leaks are possible
==1897==
==1897== For lists of detected and suppressed errors, rerun with: -s
==1897== ERROR SUMMARY: 1 errors from 1 contexts (suppressed: 0 from 0)
 */