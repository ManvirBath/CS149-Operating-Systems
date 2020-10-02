#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "CommandNode.h"
#include <sys/wait.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>

int main(int argc, char *argv[]) {
    // ********** Start: Initialize Variables **********
    struct timespec starttime, finishtime; // Time_demo.c
    pid_t pid; // For forking
    int status = 0;
    char filenameOut[20]; // Store filename
    char filenameErr[20]; // Store filename
    CommandNode *node;
    // read_parse_file.c variables:
    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    ssize_t linesize = 0; // Equivalent to: ssize_t read;
    char command[20][20]; // Equivalent to: char newString[20][20];
    // CommandNode variables
    int index = 0;
    CommandNode *headCommand; // First command node
    CommandNode *prevCommand; // Previous command node
    CommandNode *nextCommand; // Next command node
    // ********** End: Initialize Variables **********

    // ********** Start: Read & Parse File (readFile) **********
    fp = fopen(argv[1], "r"); // Open the file
    if (fp == NULL) exit(EXIT_FAILURE); // Close if file cannot be opened (if not found)
    while ((linesize = getline(&line, &len, fp)) >= 0) { // Similar to code from read_parse_file.c
        for (int i = 0; i < 20; i++) for (int j = 0; j < 20; j++) command[i][j] = 0; // Initialize it all to null
        int j = 0;
        int ctr = 0;
        if (line[strlen(line) - 1] == '\n') line[strlen(line) - 1] = '\0'; // Remove newline at the end of string and replace with NULL
        for (int i = 0; i <= (strlen(line)); i++) {
            if (line[i] == ' ' || line[i] == '\0') {
                command[ctr][j] = '\0'; // if space or NULL found, assign NULL into newString[ctr]
                ctr++; //for next word
                j = 0; //for next word, init index to 0
            } else {
                command[ctr][j] = line[i];
                j++;
            }
        }
        // ********** Start: Create Command Nodes **********
        index++;
        if (index == 1) { // For the first node
            headCommand = (CommandNode *) malloc(sizeof(CommandNode)); //initialize
            CreateCommandNode(headCommand, command, index, NULL); //create node
            prevCommand = headCommand;
        } else { // For all other nodes
            CommandNode *nextCommand = (CommandNode *) malloc(sizeof(CommandNode)); //initialize
            CreateCommandNode(nextCommand, command, index, NULL); //create node
            InsertCommandAfter(prevCommand, nextCommand);//insert after headCommand
            prevCommand = nextCommand;
        }
        // ********** End: Create Command Nodes **********
    }
    fclose(fp); // Close input file
    if (line) free(line);
    // ********** End: Read & Parse File (readFile) **********

    // ********** Start: Execute Commands **********
    CommandNode *tmpNode = headCommand; /** This is the beginning of your linked list of CommandNodes **/
    while (tmpNode != NULL) {
        // ********** Start: Open/Create files for output (From Discussion Post) **********
        sprintf(filenameOut, "%d.out", tmpNode->index); // Name of the file
        sprintf(filenameErr, "%d.err", tmpNode->index); // Name of the file
        FILE *outOut = fopen(filenameOut, "w"); // Open/Create file
        FILE *outErr = fopen(filenameErr, "w"); // Open/Create file
        // ********** End: Open/Create files for output (From Discussion Post) **********

        clock_gettime(CLOCK_MONOTONIC, &starttime); // save the startime
        tmpNode->starttime = (int) starttime.tv_sec; // tmpNode.startime = starttime
        pid = fork(); // Fork process
        if (pid < 0) { // Fork failed
            fprintf(stderr, "error forking");
            exit(2); // Exit with error: Error code = 2
        } else if (pid == 0) { // Child process
            char *buff[20]; // Array for command[0] and its parameters
            for (int k = 0; k < 20; k++)
                if (tmpNode->command[k][0] != 0) buff[k] = tmpNode->command[k]; // Copy parameter pointers to buff
                else buff[k] = (char *) 0; // If no more parameters, point to NULL
            // The exec() functions only return if an error has occurred.
            // The return value is -1, and errno is set to indicate the error.
            if (execvp(buff[0], buff) < 0) { // executes the command in the specific node and catch error
                fprintf(outErr, "%s: %s\n", command[0], strerror(errno)); // strerror returns the error string
                exit(2); // Exit with error 2
            }
            exit(0); // If executed normally, return with no error
        } else if (pid > 0) {  // parent goes to the next node
            tmpNode->PID = pid;
            fprintf(outOut, "Starting command %d: child %d pid of parent %d.\n", tmpNode->index, pid, getpid());
            tmpNode = tmpNode->nextCommandPtr; // Go to next node
        }
        fclose(outOut); // Close X.out file
        fclose(outErr); // Close X.err file
    } /* end of while loop */
    // ********** End: Execute Commands **********

    // ********** Start: Print Results From Execution Processes **********
    // Final while loop: waits until anything has completed.
    // This will exit (wait returns -1) when there is no more child process. Then your parent process exits.
    while ((pid = wait(&status)) >= 0) {
        if (pid > 0) {
            clock_gettime(CLOCK_MONOTONIC, &finishtime); // finishtime = get the finish time
            // search your linked list for the node that corresponds to pid
            node = FindCommand(headCommand, pid); // The function FindNode was provided: node = FindNode(pid)

            // ********** Start: Open/Create files for output (From Discussion Post) **********
            sprintf(filenameOut, "%d.out", node->index); // Name file
            sprintf(filenameErr, "%d.err", node->index); // Name file
            FILE *outOut = fopen(filenameOut, "a"); // Append created file
            FILE *outErr = fopen(filenameErr, "a"); // Append created file
            // ********** End: Open/Create files for output (From Discussion Post) **********

            // ********** Start: Print Exit or Errors **********
            fprintf(stderr, "Process with PID %d terminated.\n", pid);
            // if (WIFEXITED(status)) fprintf(outErr, "Child %d terminated normally with exit code: %d\n", pid, WEXITSTATUS(status));
            // else if (WIFSIGNALED(status)) fprintf(outErr, "Child %d terminated abnormally with signal number: %d\n", pid, WTERMSIG(status));
            if (WIFEXITED(status)) fprintf(outErr, "Exited with exitcode = %d\n", WEXITSTATUS(status));
            else if (WIFSIGNALED(status)) fprintf(outErr, "Killed with signal %d\n", WTERMSIG(status));
            // ********** End: Print Exit or Errors **********

            // ********** Start: Calculate & Print Time **********
            // to compute the elapsed time you subtract: elapsedtime = finishtime - node.start_time
            int elapsedtime = ((int) finishtime.tv_sec - node->starttime);
            fprintf(outOut, "Finished at %d, runtime duration %d\n", (int) finishtime.tv_sec, elapsedtime);
            // ********** End: Calculate & Print Time **********

            // ********** Start: Restart For Time > 2 **********
            if (elapsedtime > 2) { // decide if you will restart
                fprintf(stdout, "Process %d Restarted: Elapsed Time = %d\n", node->index, elapsedtime); // For debugging
                clock_gettime(CLOCK_MONOTONIC, &starttime); // record the new starttime
                node->starttime = starttime.tv_sec; // node.starttime = new starttime
                pid = fork();
                if (pid < 0) { //error
                    fprintf(stderr, "error forking");
                    exit(2);
                } else if (pid == 0) { //child
                    char *buff[20]; // Array with command and its parameters
                    for (int k = 0; k < 20; k++)
                        if (node->command[k][0] != 0) buff[k] = node->command[k]; // Copy parameter pointers to buff
                        else buff[k] = (char *) 0; // If no more parameters, point to NULL
                    // The exec() functions only return if an error has occurred.
                    // The return value is -1, and errno is set to indicate the error.
                    if (execvp(buff[0], buff) < 0) { // executes the command in the specific node and catch error
                        fprintf(outErr, "%s: %s\n", command[0], strerror(errno)); // strerror returns the error string
                        exit(2); // Exit with error 2
                    }
                    exit(0); // If executed normally, return with no error
                } else if (pid > 0) {
                    fprintf(outOut, "Starting command %d: child %d pid of parent %d.\n", node->index, pid, getpid());
                    node->PID = pid;
                }
            } else fprintf(outErr, "spawning too fast\n");
            // ********** End: Restart For Time > 2 **********
            fclose(outOut); // Close X.out file
            fclose(outErr); // Close X.err file
        }
    }
    // ********** End: Print Results From Execution Processes **********
    return 0;
}