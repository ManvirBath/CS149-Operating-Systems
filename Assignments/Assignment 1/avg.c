#include <stdio.h>
#include <stdlib.h>
int main(int argc, char *argv[]) {
    double i = 0, n= 0, avg = 0, total = 0; //i = counter, n = numbers,  avg = average, total = sum of the numbers 
    FILE *fp = fopen(argv[1], "r");
if (fp == NULL) {
    printf("file cannot be opened\n");
    exit(1);
}
    while(fscanf(fp, "%lf", &n) != EOF) {//read integers to number 
      
        total += n; //getting the total sum of numbers 
        ++i; //Incrementing the counters
        avg = (total / i); //the average of the total numbers 
    }
    printf("%lf\n", avg); //printing the average of numbers 
    fclose(fp);
    return 0;
}
