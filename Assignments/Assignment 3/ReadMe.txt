This project contains the same CommandNode.h and CommandNode.c from Canvas.

This project has an active debugging line on line 138 "fprintf(stdout, "Process %d Restarted: Elapsed Time = %d\n", node->index, elapsedtime);" which prints the index and previous time of the restarted command.

When you start the program and the processes restart, you may kill the command using kill -9 pid where pid is the pid of the restarted command. This will be reflected in the output file.

Typing in an invalid command will fail execvp and the exit code is 2. The error command and its description will be printed to the corresponding err file.