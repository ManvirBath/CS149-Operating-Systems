#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

// Data structure for a tuple = <sum_inputs, num_inputs>
typedef struct S {
    double sum_inputs;
    int num_inputs;
} tuple;


// Returns tuple <sum of inputs, number of inputs>
tuple processFile(char *filename) {
    FILE *file = fopen(filename, "r"); // Open file
    if (file == NULL) { // If file does not exist
        fprintf(stderr, "File cannot be opened!\n"); // Print error
        exit(1); // Exit with error
    }
    tuple result; // Create tuple to store result
    result.sum_inputs = 0; // Store sum of inputs
    result.num_inputs = 0; // Store number of inputs
    double input; // Stores the input read on each line in file
    while (fscanf(file, "%lf", &input) != EOF) { // While not end of file, read inputs on each line
        result.sum_inputs += input; // Sum input on each line
        result.num_inputs++; // Increment number of inputs
    }
    return result;
}

// Store the sum and number of inputs from each file in array of pipes
// Create parallel child processes using recursion
// Parent waits for all processes to end after executing the recursion
// Children are run in parallel to parent as function quickly recurses and creates a process for each file
void getInputs(int count, int fd[][2], char *argv[]) {
    if (count == 0) return; // If 0 inputs, return to main. If end of recursion, return to caller.
    else { // For count > 1
        pid_t pid = fork(); // Create subprocesses to process all passed files
        if (pid < 0) { // Fork failed
            fprintf(stderr, "Fork failed!\n");
            exit(1); // Exit with error
        } else if (pid == 0) { // Children process for each file
            close(fd[count][0]); // Close reading end of ith pipe (from child's side)
            getInputs(count - 1, fd, argv); // Recursion: operate on every file
            tuple result = processFile(argv[count]); // Variable to store tuple of file
            write(fd[count][1], &result, sizeof(result)); // Store tuple in pipe
            close(fd[count][1]); // Close writing end of ith pipe (from child's side)
            exit(0); // Exit child process
        } else wait(NULL); // *** Parent waits for children ***
    }
}

int main(int argc, char *argv[]) {
    // argc = num of arguments - 1
    // argv = array of arguments/files
    // *** Step 0: Check if no inputs passed
    if (argc == 1) { // If no inputs passed, print 0 and return
        printf("0.0000"); // Print 0
        return 0;
    }
    // *** Step 1: Calculate number of files
    int count = argc - 1; // count = number of files
    // *** Step 2: Create and initialize pipes
    int fd[argc][2]; // Pipes
    for (int i = 1; i <= count; i++) // Initialize pipes
        if (pipe(fd[i]) == -1) { // Check if pipe failed
            fprintf(stderr, "Pipe Failed");
            return 1;
        }
    // *** Step 3: Retrieve sum of inputs and number of inputs in pipes for each file
    getInputs(count, fd, argv); // Retrieve sum of inputs and number of inputs in pipes
    // *** Step 4: Initialize variables used in total average
    double total_sum = 0.0; // Sum of all inputs from all files
    int total_inputs = 0; // Total number of all inputs from all files
    // *** Step 5: Retrieve tuple = <sum_inputs, num_inputs> per file and add to total_sum and total_inputs respectively
    for (int i = 1; i <= count; i++) {
        close(fd[i][1]); // Close writing end of ith pipe
        tuple result; // Store resulting tuple from each pipe
        read(fd[i][0], &result, sizeof(result)); // Retrieve tuple = <sum_inputs, num_inputs> per file
        close(fd[i][0]); // Close reading end of ith pipe
        total_sum += result.sum_inputs; // Add sum_inputs to total sum
        total_inputs += result.num_inputs; // Add num_inputs to main total number of inputs
    }
    // *** Step 6: Calculate and print average until 4 decimal places
    printf("%.4f\n", total_sum / total_inputs); // Calculate and print average until 4 decimal places
    return 0;
}