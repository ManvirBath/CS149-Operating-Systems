#include <stdio.h>
#include <stdlib.h>

int main() {
    // allocate memory using malloc()
    char *line = (char *) malloc(sizeof(char) * 21); // Allocate space for a string of 20 chars + NULL
    printf("Enter a string:\n");
    scanf("%s", line);
    printf("Entered string: %s\n", line);
    // forget to free it (allocated memory using malloc()) before exiting
    return 0;
}
// What happens when this program runs?
/*
 * This program would cause a memory leak. The allocated memory would not be garbage collected.
 * First, we allocate space for a string (an array of chars) with max 21 chars (including null char).
 * Then we ask user to enter string. Then we save it using scanf. Then we print the string.
 * Then we exit without freeing the allocated space, causing a memory leak.
 */

// Can you use gdb to find any problems with it?
/*
 * You cannot use gdb to find any problems with it. It does not report any errors.
 */

// How about valgrind (with the --leak-check=yes flag)?
/* Valgrind can help determine the memory leak with this program.
 * A memory leak is present in this program because we forget to free
 * the allocated memory space before exiting.
 * Here is the report:

==1622== Memcheck, a memory error detector
==1622== Copyright (C) 2002-2017, and GNU GPL'd, by Julian Seward et al.
==1622== Using Valgrind-3.16.1 and LibVEX; rerun with -h for copyright info
==1622== Command: ./a.out
==1622==
Enter a string:
Hello
Entered string: Hello
==1622==
==1622== HEAP SUMMARY:
==1622==     in use at exit: 21 bytes in 1 blocks
==1622==   total heap usage: 3 allocs, 2 frees, 2,069 bytes allocated
==1622==
==1622== 21 bytes in 1 blocks are definitely lost in loss record 1 of 1
==1622==    at 0x4A34ECB: malloc (vg_replace_malloc.c:307)
==1622==    by 0x109176: main (in /home/cs149/Desktop/Test/a.out)
==1622==
==1622== LEAK SUMMARY:
==1622==    definitely lost: 21 bytes in 1 blocks
==1622==    indirectly lost: 0 bytes in 0 blocks
==1622==      possibly lost: 0 bytes in 0 blocks
==1622==    still reachable: 0 bytes in 0 blocks
==1622==         suppressed: 0 bytes in 0 blocks
==1622==
==1622== For lists of detected and suppressed errors, rerun with: -s
==1622== ERROR SUMMARY: 1 errors from 1 contexts (suppressed: 0 from 0)
 */