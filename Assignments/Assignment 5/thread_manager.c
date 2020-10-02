#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>

//thread mutex lock for access to the log index
pthread_mutex_t tlock1 = PTHREAD_MUTEX_INITIALIZER;
//thread mutex lock for critical sections of allocating THREADDATA
pthread_mutex_t tlock2 = PTHREAD_MUTEX_INITIALIZER;
//thread mutex lock for access to the linked list (head)
pthread_mutex_t tlock3 = PTHREAD_MUTEX_INITIALIZER;

void *thread_runner(void *);

pthread_t tid1, tid2;
struct THREADDATA_STRUCT {
    pthread_t creator;
};
typedef struct THREADDATA_STRUCT THREADDATA;

THREADDATA *p = NULL;

//variable for indexing of messages by the logging function
int logindex = 0;
int *logip = &logindex;


//A flag to indicate if the reading of input is complete,
//so the other thread knows when to stop
bool is_reading_complete = false;

// #################### Start: My Declarations ####################
// ********** Function Signatures **********
void printMessage(int thread_index, pid_t PID, char *msg);

void AddNode(char *line);

// ********** Node for Linked List **********
typedef struct command_struct {
    char *command;
    struct command_struct *nextCommandPtr;
} CommandNode;
CommandNode *head = NULL; // Ptr to head node of linked list

// ********** Deallocate Linked List **********
void DeallocateLinkedList(CommandNode **head) {
    CommandNode *current = *head; // Node to delete
    while (current != NULL) { // Iteratively deallocate Linked List
        CommandNode *temp = current->nextCommandPtr; // Store next node
        free(current->command); // Delete line in current node
        free(current); // Delete current node
        current = temp; // Set next node as current node
    }
}

void AddNode(char *line) {
    if (head == NULL) { // If creating first node
        head = (CommandNode *) malloc(sizeof(CommandNode)); // Allocate space for head node
        head->command = (char *) malloc(strlen(line) + 1); // Allocate space for command
        // Valgrind causes an error if we do not initialize head->command - done using memset
        memset(head->command, '\0', strlen(line) + 1); // Initialize memory location to null
        strncpy(head->command, line, strlen(line)); // Copy line to command node
        // memset(head->command, '\0', 20); // Initialize memory location to null
        // strncpy(head->command, line, 20); // Copy line to command node
        head->nextCommandPtr = NULL; // Set next node as null
    } else { // Create new command node and add to list
        CommandNode *newNode = (CommandNode *) malloc(sizeof(CommandNode)); // Allocate space for new node
        newNode->command = (char *) malloc(strlen(line) + 1); // Allocate space for command
        // Valgrind causes an error if we do not initialize newNode->command - done using memset
        memset(newNode->command, '\0', strlen(line) + 1); // Initialize memory location to null
        strncpy(newNode->command, line, strlen(line)); // Copy line to command node
        // memset(newNode->command, '\0', 20); // Initialize memory location to null
        // strncpy(newNode->command, line, 20); // Copy line to command node
        newNode->nextCommandPtr = head; // Set next node as head
        head = newNode; // This node becomes the head
    }
}

// #################### End: My Declarations ####################

/*********************************************************
// function main  -------------------------------------------------
*********************************************************/
int main() {
    printf("create first thread\n");
    pthread_create(&tid1, NULL, thread_runner, NULL);

    printf("create second thread\n");
    pthread_create(&tid2, NULL, thread_runner, NULL);

    printf("wait for first thread to exit\n");
    pthread_join(tid1, NULL);
    printf("first thread exited\n");

    printf("wait for second thread to exit\n");
    pthread_join(tid2, NULL);
    printf("second thread exited\n");

    exit(0);
} //end main


/**********************************************************************
// function thread_runner runs inside each thread --------------------------------------------------
**********************************************************************/
void *thread_runner(void *x) {
    pthread_t me = pthread_self();
    printf("This is thread %ld (p=%p)\n", me, p);

    // Lock 1: logindex
    // Lock 2: THREADDATA
    // Lock 3: Linked List Head

    pthread_mutex_lock(&tlock2); // critical section starts (access to THREADDATA)
    if (p == NULL) {
        p = (THREADDATA *) malloc(sizeof(THREADDATA));
        p->creator = me;
    }
    pthread_mutex_unlock(&tlock2);  // critical section ends (access to THREADDATA)

    if (p != NULL && p->creator == me) {
        printf("This is thread %ld and I created the THREADDATA %p\n", me, p);

        // ******************** Instance Variables (for thread 1) ********************
        char *line = NULL; // Ptr to store line read
        char buf[21]; // Buffer to store line (Assume each line is at most 20 characters)
        char msg[61]; // Message to print out in print function (max size 60 chars)

        // ******************** Start: Read Input, Create Nodes, & Print Messages ********************
        while (is_reading_complete == false) { // While we are reading
            // ******************** Read Input ********************
            line = fgets(buf, 21, stdin); // Get line
            if (line == NULL) { // Check if end of file (line is null)
                is_reading_complete = true; // Change reading flag to false if EOF
                break; // Leave the loop
            }
            // If we put locks above, we cannot detect EOF (Ctrl + D)
            pthread_mutex_lock(&tlock3); // critical section starts (access to linked list)
            pthread_mutex_lock(&tlock1); // critical section starts (access to index log)

            // ******************** Create Node ********************
            // If not EOF, add to linked list and print message (need head and logindex)
            AddNode(line); // Add line to linked list

            // ******************** Print Messages ********************
            snprintf(msg, 61, "Read line %s", head->command); // Set message (msg = "Read line ...")
            printMessage(1, getpid(), msg); // Print message: Thread 1, get PID of thread 1, print message

            pthread_mutex_unlock(&tlock1); // critical section ends (access to index log)
            pthread_mutex_unlock(&tlock3); // critical section ends (access to linked list)
        }
        // ******************** End: Read Input, Create Nodes, & Print Messages ********************
        free(line); // Deallocate line
    } else {
        printf("This is thread %ld and I can access the THREADDATA %p\n", me, p);

        // ******************** Instance Variables (for thread 2) ********************
        CommandNode *printed = NULL; // Ptr to the last printed head node
        char msg[61]; // Message to print out in print function (max size 60 chars)

        // ******************** Start: Read Nodes and Print Messages ********************
        while (is_reading_complete == false) { // While still reading input
            pthread_mutex_lock(&tlock3); // critical section starts (access to linked list)
            pthread_mutex_lock(&tlock1); // critical section starts (access to index log)
            if (printed != head) { // Check if content changes (check if we added a new node/head to list)
                printed = head; // Update last printed node
                memset(msg, '\0', 61); // Empty entire message string of 60 chars - for Valgrind
                // Write message to print using snprintf - snprintf specifies number of chars to print (60)
                snprintf(msg, 61, "Head of linked list contains line %s", printed->command); // Set msg string to print
                printMessage(2, getpid(), msg); // Thread 2, get PID of thread 2, print message
            }
            pthread_mutex_unlock(&tlock1); // critical section ends (access to index log)
            pthread_mutex_unlock(&tlock3); // critical section ends (access to linked list)
        }
        // ******************** End: Read Nodes and Print Messages ********************
    }

    // ******************** Start: Deallocate THREADDATA & Linked List ********************
    pthread_mutex_lock(&tlock2); // critical section starts (access to THREADATA)
    pthread_mutex_lock(&tlock3); // critical section starts (access to linked list)
    if (p != NULL && p->creator == me)
        printf("This is thread %ld and I did not touch THREADDATA\n", me);
    else { // if (p != NULL && p->creator != me)  (the other thread from the one that created it)
        // ********** Deallocate THREADDATA **********
        free(p); // Deallocate p
        p = NULL; // Lose reference to p
        // ********** Deallocate Linked List **********
        DeallocateLinkedList(&head); // Deallocate Linked List
        head = NULL;
        printf("This is thread %ld and I deleted the THREADDATA\n", me);
    }
    pthread_mutex_unlock(&tlock3); // critical section ends (access to linked list)
    pthread_mutex_unlock(&tlock2); // critical section ends (access to THREADATA)
    // ******************** End: Deallocate THREADDATA & Linked List ********************

    pthread_exit(NULL);
    return NULL;
} //end thread_runner

/*
 * Each thread should print messages to stdout describing what it is doing at each step.
 * Each message printed to stdout should be prefixed with a log index, thread index, PID, and date and time
 * (you may check the sample time.c provided).
 * The log index gets incremented for each message printed to stdout.
 * An example of a typical message:
 * "Logindex 1, thread 2, PID 5435, 21/04/2020 09:23:25 pm: Head of linked list contains line foo".
 */
void printMessage(int thread_index, pid_t PID, char *msg) {
    int hours, minutes, seconds, day, month, year; // variables to store date and time components
    time_t now; // time_t is arithmetic time type
    time(&now); // Obtain current time: time() returns the current time of the system as a time_t value

    // localtime converts a time_t value to calendar time and
    // returns a pointer to a tm structure with its members filled with the corresponding values
    struct tm *local = localtime(&now);

    hours = local->tm_hour; // get hours since midnight (0-23)
    minutes = local->tm_min; // get minutes passed after the hour (0-59)
    seconds = local->tm_sec; // get seconds passed after minute (0-59)

    day = local->tm_mday; // get day of month (1 to 31)
    month = local->tm_mon + 1; // get month of year (0 to 11)
    year = local->tm_year + 1900; // get year since 1900

    // Each thread should print messages to stdout describing what it is doing at each step. (included in msg)
    // Each message printed to stdout should be prefixed with a log index, thread index, PID, and date and time
    // Ex: "Logindex 1, thread 2, PID 5435, 21/04/2020 09:23:25 pm: Head of linked list contains line foo".
    if (hours < 12) // print local time
        fprintf(stdout, "Logindex %d, thread %d, PID %d, %02d/%02d/%d %02d:%02d:%02d am: %s",
                logindex, thread_index, PID, day, month, year, hours, minutes, seconds, msg); // before midday
    else
        fprintf(stdout, "Logindex %d, thread %d, PID %d, %02d/%02d/%d %02d:%02d:%02d pm: %s",
                logindex, thread_index, PID, day, month, year, hours - 12, minutes, seconds, msg);
    logindex++; // The log index gets incremented for each message printed to stdout.
}