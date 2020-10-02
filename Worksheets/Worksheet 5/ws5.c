#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
int main(int argc, char *argv[]) {
    int rc = fork();
    int rc1 = fork();
    // 2^2 = 4 processes
    if (rc < 0 || rc1 < 0) { // fork failed; exit
        fprintf(stderr, "fork failed\n");
        exit(1);
    } else {
        printf("hello world from PID %d!\n", (int) getpid());
    }
    return 0;
}
