#include "Logger.h"

//----------------------------------------------------------------------------------------------------------------------

const char* error_message[] = { "Done Successfully",                          // 0 both
                                "Failed to create logfile",                   // 1 both
                                "Incorrect number of command line arguments", // 2 both
                                "Failed to open input file",                  // 3 both

                                "Invalid type of input file",                 // 4 C
                                "Outdated version of code",                   // 5 C
                                "Incorrect version of code",                  // 6 C
                                "Input file is empty",                        // 7 both

                                "Failed to create buffer",                    // 8 A
                                "Failed to write code to file",               // 9 A
                                "Too few elements in stack",                  // 10 C

                                "Stack is empty",                             // 11 C
                                "Address is bigger than size of RAM",         // 12 C
                                "Infinite loop detected",                     // 13 C
                                "Division by zero",                           // 14 C
                                "Entered invalid value",                      // 15 C
                                "Failed to create dump file",                 // 16 C
                                "Failed to create array of commands",         // 17 A

                                "Failed to create array of code",             // 18 both
                                "Failed to resize array of code",             // 19 A

                                "Incorrect command",                          // 20 A
                                "Incorrect argument",                         // 21 A
                                "Too few arguments to such command",          // 22 A
                                "Too many arguments to such command",         // 23 A
                                "Invalid label name",                         // 24 A
                                "Too many labels in a row",                   // 25 A
                                "Incorrect jump command",                     // 26 A
                                "This label already exists",                  // 27 A
                                "Jump to nonexistent label",                  // 28 A
                                "Operating with nonexistent register",        // 29 A

                                "Failed to create output file",               // 30 A
                                "Failed to push bytes to code",               // 31 A
                                "Failed to create array of labels",           // 32 A
                                "Failed to resize array of labels",           // 33 A
                                "Failed to create array of jump addresses",   // 34 A
                                "Failed to resize array of jump addresses" }; // 35 A

//----------------------------------------------------------------------------------------------------------------------

void dump_to_console(int err, size_t err_line)
{
    printf("---\nERROR: ");
    printf(error_message[err]);

    if (err_line != 0)
        printf(" (line %zu)", err_line);

    printf("\n---");
}

//----------------------------------------------------------------------------------------------------------------------

void dump_to_logfile(FILE* logfile, int err, size_t err_line)
{
    fprintf(logfile, "ERROR: ");
    fprintf(logfile, error_message[err]);

    if (err_line != 0)
        fprintf(logfile, " (line %zu)", err_line);

    fprintf(logfile, "\n");
}

//----------------------------------------------------------------------------------------------------------------------