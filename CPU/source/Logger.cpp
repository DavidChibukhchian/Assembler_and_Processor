#include "Logger.h"

//----------------------------------------------------------------------------------------------------------------------

const char* error_message[] = { "Done Successfully",                          // 0 A & C
                                "Failed to create logfile",                   // 1 A & C
                                "Incorrect number of command line arguments", // 2 A & C
                                "Failed to open input file",                  // 3 A & C
                                "Input file is empty",                        // 4 A & C
                                "Failed to create array of code",             // 5 A & C

                                "Failed to create output file",               // 6  A
                                "Failed to create buffer",                    // 7  A
                                "Failed to create array of commands",         // 8  A
                                "Failed to resize array of code",             // 9  A
                                "Failed to create array of labels",           // 10 A
                                "Failed to resize array of labels",           // 11 A
                                "Failed to create array of jump addresses",   // 12 A
                                "Failed to resize array of jump addresses",   // 13 A
                                "Failed to write code to file",               // 14 A
                                "Incorrect command",                          // 15 A
                                "Incorrect argument",                         // 16 A
                                "Too few arguments to such command",          // 17 A
                                "Too many arguments to such command",         // 18 A
                                "Invalid label name",                         // 19 A
                                "Too many labels in a row",                   // 20 A
                                "Incorrect jump command",                     // 21 A
                                "This label already exists",                  // 22 A
                                "Jump to nonexistent label",                  // 23 A
                                "Operating with nonexistent register",        // 24 A

                                "Invalid type of input file",                 // 25 C
                                "Outdated version of code",                   // 26 C
                                "Incorrect version of code",                  // 27 C
                                "Too few elements in stack",                  // 28 C
                                "Stack is empty",                             // 29 C
                                "Address is bigger than size of RAM",         // 30 C
                                "Infinite loop was detected",                 // 31 C
                                "Division by zero",                           // 32 C
                                "Entered invalid value",                      // 33 C
                                "Failed to create dump file" };               // 34 C

//----------------------------------------------------------------------------------------------------------------------

void dump_to_console(int err, size_t err_line)
{
    printf("---\nERROR: ");
    printf(error_message[err]);

    if (err_line)
        printf(" (line %zu)", err_line);

    printf("\n---");
}

//----------------------------------------------------------------------------------------------------------------------

void dump_to_logfile(FILE* logfile, int err, size_t err_line)
{
    fprintf(logfile, "ERROR: ");
    fprintf(logfile, error_message[err]);

    if (err_line)
        fprintf(logfile, " (line %zu)", err_line);

    fprintf(logfile, "\n");
}

//----------------------------------------------------------------------------------------------------------------------