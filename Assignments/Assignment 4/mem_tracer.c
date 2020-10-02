//
// Created by Abdurrahman Mohammad on 7/26/20.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <fcntl.h>
#include <unistd.h>

// TRACE_NODE_STRUCT is a linked list of
// pointers to function identifiers
// TRACE_TOP is the head of the list is the top of the stack
struct TRACE_NODE_STRUCT {
    char *functionid; // ptr to function identifier
    int index;
    struct TRACE_NODE_STRUCT *next; // ptr to next frama
};
typedef struct TRACE_NODE_STRUCT TRACE_NODE;
static TRACE_NODE *TRACE_TOP = NULL;       // ptr to the top of the stack


/* --------------------------------*/
/* function PUSH_TRACE */
void PUSH_TRACE(char *p) { // push p on the stack
    TRACE_NODE *tnode;
    static char glob[] = "global";
    if (TRACE_TOP == NULL) {
        // initialize the stack with "global" identifier
        TRACE_TOP = (TRACE_NODE *) malloc(sizeof(TRACE_NODE));
        // no recovery needed if allocation failed, this is only
        // used in debugging, not in production
        if (TRACE_TOP == NULL) {
            printf("PUSH_TRACE: memory allocation error\n");
            exit(1);
        }
        TRACE_TOP->functionid = glob;
        TRACE_TOP->next = NULL;
    }
    // create the node for p
    tnode = (TRACE_NODE *) malloc(sizeof(TRACE_NODE));
    // no recovery needed if allocation failed, this is only
    // used in debugging, not in production
    if (tnode == NULL) {
        printf("PUSH_TRACE: memory allocation error\n");
        exit(1);
    }
    tnode->functionid = p;
    tnode->next = TRACE_TOP; // insert fnode as the first in the list
    TRACE_TOP = tnode; // point TRACE_TOP to the first node

} /*end PUSH_TRACE*/

/* --------------------------------*/
/* function POP_TRACE */
void POP_TRACE() { // remove the op of the stack
    TRACE_NODE *tnode;
    tnode = TRACE_TOP;
    TRACE_TOP = tnode->next;
    free(tnode);
} /*end POP_TRACE*/


/* ---------------------------------------------- */
/* function PRINT_TRACE prints out the sequence of function calls that are on the stack at this instance */
/* For example, it returns a string that looks like: funcA:funcB:funcC.  */
char *PRINT_TRACE() {
    int depth = 50; //A max of 50 levels in the stack will be combined in a string for printing out.
    int i, length, j;
    TRACE_NODE *tnode;
    static char buf[100];
    if (TRACE_TOP == NULL) {     // stack not initialized yet, so we are
        strcpy(buf, "global");   // still in the `global' area
        return buf;
    }
    /* peek at the depth top entries on the stack, but do not
       go over 100 chars and do not go over the bottom of the
       stack */
    sprintf(buf, "%s", TRACE_TOP->functionid);
    length = strlen(buf);                  // length of the string so far
    for (i = 1, tnode = TRACE_TOP->next;
         tnode != NULL && i < depth;
         i++, tnode = tnode->next) {
        j = strlen(tnode->functionid);             // length of what we want to add
        if (length + j + 1 < 100) {              // total length is ok
            sprintf(buf + length, ":%s", tnode->functionid);
            length += j + 1;
        } else                                // it would be too long
            break;
    }
    return buf;
}

// -----------------------------------------
// function REALLOC calls realloc
// TODO REALLOC should also print info about memory usage.
// For instance, example of print out:
// "File tracemem.c, line X, function F reallocated the memory segment at address A to a new size S"
// Information about the function F should be printed by printing the stack (use PRINT_TRACE)
void *REALLOC(void *p, int t, char *file, int line) {
    p = realloc(p, t);
    // ********** Start: My REALLOC **********
    printf("File %s, line %d, function %s ", file, line, PRINT_TRACE());
    printf("reallocated the memory segment at address %p to a new size %d\n", p, t);
    // ********** End: My REALLOC **********
    return p;
}

// -------------------------------------------
// function MALLOC calls malloc
// TODO MALLOC should also print info about memory usage.
// For instance, example of print out:
// "File tracemem.c, line X, function F allocated new memory segment at address A to size S"
// Information about the function F should be printed by printing the stack (use PRINT_TRACE)
void *MALLOC(int t, char *file, int line) {
    void *p;
    p = malloc(t);
    // ********** Start: My MALLOC **********
    printf("File %s, line %d, function %s ", file, line, PRINT_TRACE());
    printf("allocated new memory segment at address %p of size %d\n", p, t);
    // ********** End: My MALLOC **********
    return p;
}

// ----------------------------------------------
// function FREE calls free
// TODO FREE should also print info about memory usage.
// For instance, example of print out:
// "File tracemem.c, line X, function F deallocated the memory segment at address A"
// Information about the function F should be printed by printing the stack (use PRINT_TRACE)
void FREE(void *p, char *file, int line) {
    free(p);
    // ********** Start: My FREE **********
    printf("File %s, line %d, function %s ", file, line, PRINT_TRACE());
    printf("deallocated the memory segment at address %p\n", p);
    // ********** End: My FREE **********
}

#define realloc(a, b) REALLOC(a,b,__FILE__,__LINE__)
#define malloc(a) MALLOC(a,__FILE__,__LINE__)
#define free(a) FREE(a,__FILE__,__LINE__)


// -----------------------------------------
// function add_column will add an extra column to a 2d array of ints.
// This function is intended to demonstrate how memory usage tracing of realloc is done
void add_column(int **array, int rows, int columns) {
    PUSH_TRACE("add_column");
    for (int i = 0; i < rows; i++) {
        array[i] = (int *) realloc(array[i], sizeof(int) * (columns + 1));
        array[i][columns] = 10 * i + columns;
    }
    POP_TRACE();
    return;
}// end add_column


// ------------------------------------------
// function make_extend_array
// Example of how the memory trace is done
// This function is intended to demonstrate how memory usage tracing of malloc and free is done
void make_extend_array() {
    PUSH_TRACE("make_extend_array");
    int i, j;
    int **array;

    //make array
    array = (int **) malloc(sizeof(int *) * 4);  // 4 rows
    for (i = 0; i < 4; i++) {
        array[i] = (int *) malloc(sizeof(int) * 3);  // 3 columns
        for (j = 0; j < 3; j++)
            array[i][j] = 10 * i + j;
    }

    //display array
    for (i = 0; i < 4; i++)
        for (j = 0; j < 3; j++)
            printf("array[%d][%d]=%d\n", i, j, array[i][j]);

    // and a new column
    add_column(array, 4, 3);

    // now display the array again
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
            printf("array[%d][%d]=%d\n", i, j, array[i][j]);

    // Now deallocate it
    for (i = 0; i < 4; i++) free((void *) array[i]);
    free((void *) array);

    POP_TRACE();
    return;
}//end make_extend_array

// ################################################################################
// #################### Personal Defined Functions Below ####################
// ################################################################################
// Define the initial size of array
#define ROWS     3
#define COLUMNS 10

// ********** Initialize Array **********
// Initialize the array memory with malloc to an initial size.
void InitializeArray(char ***inputArray, int rows, int columns) {
    PUSH_TRACE("InitializeArray");
    char **array; // initial size = rows x columns
    array = (char **) malloc(sizeof(char *) * rows); // Initialize rows
    for (int i = 0; i < rows; i++)
        array[i] = (char *) malloc(sizeof(char) * columns); // Initialize columns
    POP_TRACE();
    *inputArray = array;
    POP_TRACE();
}

// ********** Print Array **********
void PrintArray(char **array, int rows) {
    PUSH_TRACE("PrintArray");
    for (int i = 0; i < rows; i++) printf("array[%d] = %s\n", i, array[i]); // Print each row
    POP_TRACE();
}


// ********** Create and Add Node **********
// Store each line in a linked list where a node contains the line as a string (char *) and index number
void AddNode(TRACE_NODE **head, char *line, int index) {
    PUSH_TRACE("AddNode");
    TRACE_NODE *newNode = (TRACE_NODE *) malloc(sizeof(TRACE_NODE));
    newNode->functionid = (char *) malloc(strlen(line) + 1);
    strncpy(newNode->functionid, line, strlen(line) + 1);
    newNode->index = index;
    newNode->next = NULL;
    // Add node to Linked List
    if (*head == NULL) *head = newNode; // If head is null, newNode becomes head
    else { // Else traverse the list and insert at the end
        TRACE_NODE *current = *head;
        while (current->next != NULL) current = current->next; // Go to last node
        current->next = newNode; // Insert newNode at the end of list
    }
    POP_TRACE();
}

// ********** Print Nodes **********
// A recursive function PrintNodes that prints out the content of all nodes in the linked list
// Printing just the line and index number are sufficient
void PrintNodes(TRACE_NODE *node) {
    PUSH_TRACE("PrintNodes");
    // Start with node = head
    // End after printing the last node which is node = NULL
    if (node != NULL) { // Check if this is the last node
        printf("LinkedList[%d] = %s\n", node->index, node->functionid); // Print node
        PrintNodes(node->next); // Print next node
    }
    POP_TRACE(); // If reach last node, pop trace
}

// ********** Deallocate Linked List **********
// Your allocation and deallocation for the char ** array and your linked list
// should make use of your memory tracing in order to print messages describing
// the memory management and usage.
void DeallocateLinkedList(TRACE_NODE **head) {
    PUSH_TRACE("DeallocateLinkedList");
    TRACE_NODE *current = *head; // Node to delete
    while (current != NULL) { // Iteratively deallocate Linked List
        TRACE_NODE *temp = current->next; // Store next node
        free(current->functionid); // Delete current node
        free(current); // Delete current node
        current = temp; // Set next node as current node
    }
    POP_TRACE();
}


// ----------------------------------------------
// function main
int main(int argc, char *argv[]) {
    PUSH_TRACE("main");
    // ********** Create the out file and use the dup2() method **********
    int fdout = open("memtrace.out", O_WRONLY | O_CREAT | O_TRUNC);
    dup2(fdout, 1);

    // ********** Check If No Files Passed **********
    if (argc < 2) { // If no files are entered
        fprintf(stderr, "No files entered\n"); // Print error
        return 1;
    }

    // ********** Create and Initialize Array **********
    // Initialize the array memory with malloc to an initial size.
    char **array;
    InitializeArray(&array, ROWS, COLUMNS); // Initial size = ROWS x COLUMNS
    // PrintArray(array, ROWS); // Print array (debugging)

    // ********** Initialize Variables for Reading File **********
    ssize_t read;
    size_t len;
    char *line = NULL;

    // ********** Open File and Check if Error **********
    FILE *fp = fopen(argv[1], "r"); // Open file
    if (fp == NULL) { // If file does not exist
        fprintf(stderr, "File cannot be opened!\n"); // Print error
        exit(EXIT_FAILURE); // Exit with error
    }

    // ********** Read File and Populate Array **********
    int rowNumber = 0; // Keeps track of the number of rows read
    while ((read = getline(&line, &len, fp)) >= 0) {
        rowNumber++; // Increment rowNumber because we just read a line
        if (line[strlen(line) - 1] == '\n')
            line[strlen(line) - 1] = '\0'; // Remove newline at the end of string and replace with NULL
        //printf("Line %d: %s\n", rowNumber, line); // Print line (debugging)

        // ********** Expand Array if Necessary **********
        int columns = strlen(line) + 1; // Get column length = string length + 1 (+1 for NULL char)
        if (ROWS < rowNumber) { // Check of need to expand rows
            array = (char **) realloc(array, sizeof(char *) * rowNumber); // Reallocate rows
            array[rowNumber - 1] = (char *) malloc(sizeof(char) * columns); // Allocate columns
        }
        if (COLUMNS < columns)  // Check of need to expand columns
            array[rowNumber - 1] = (char *) realloc(array[rowNumber - 1], sizeof(char) * columns); // Reallocate columns

        // ********** Copy Line to Array **********
        strncpy(array[rowNumber - 1], line, columns); // Copy line from file to array (index = rowNumber - 1)
        // printf("array[%d] = %s\n", rowNumber - 1, array[rowNumber - 1]); // Print array (debugging)
    }

    // ********** Close File and Deallocate Line **********
    fclose(fp); // Close file
    if (line) free(line);

    // ********** Print Resulting array **********
    PrintArray(array, rowNumber);

    // ********** Create Linked List **********
    // Store each line in a linked list where a node contains the line as a string (char *) and index number
    TRACE_NODE *head = NULL; // Make Linked List (start with head node)
    for (int i = 0; i < rowNumber; i++)
        AddNode(&head, array[i], i); // Create node and add to linked list with corresponding command from array

    // ********** Print Linked List **********
    PrintNodes(head); // Print Linked List

    // ********** Deallocate Linked List **********
    DeallocateLinkedList(&head); // Deallocate Linked List

    // ********** Deallocate Array **********
    //DeallocateArray(&array, rowNumber); // Deallocate array
    for (int i = 0; i < rowNumber; i++) free(array[i]); // Deallocate rows
    free(array); // Deallocate columns

    close(fdout); // Close the file descriptor
    POP_TRACE(); // Pop the stack
    return (0);
} // end main