#include "Logger.h"

//----------------------------------------------------------------------------------------------------------------------

const char* error_message[] = { "Done Successfully",                          // 0 both
                                "Failed to create logfile",                   // 1 both
                                "Incorrect number of command line arguments", // 2 both
                                "Failed to open input file",                  // 3 both

                                "Invalid type of input file",                 // 4
                                "Outdated version of code",                   // 5
                                "Incorrect version of code",                  // 6
                                "Input file is empty",                        // 7

                                "Failed to create buffer",                    // 8
                                "Failed to write code to file",               // 9 A
                                "Too few elements in stack",                  // 10 C

                                "Division by zero",                           // 11 C
                                "Entered invalid value",                      // 12 C
                                "Failed to create dump file",                 // 13 C
                                "Failed to create array of commands",         // 14 A

                                "Failed to create array of code",             // 15
                                "Failed to resize array of code",             // 16

                                "Incorrect command",                          // 17 A
                                "Incorrect argument",                         // 18 A
                                "Too few arguments to such command",          // 19 A
                                "Too many arguments to such command",         // 20 A
                                "Invalid label name",                         // 21 A
                                "Too many labels in a row",                   // 22 A
                                "Incorrect jump command",                     // 23 A
                                "This label already exists",                  // 24 A
                                "Jump to nonexistent label",                  // 25 A
                                "Nonexistent register",                       // 26 A

                                "Failed to create output file",               // 27
                                "Failed to push bytes to code",               // 28 A
                                "Failed to create array of labels",           // 29 A
                                "Failed to resize array of labels",           // 30 A
                                "Failed to create array of jump addresses",   // 31 A
                                "Failed to resize array of jump addresses" }; // 32 A

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