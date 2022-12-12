#include "Logger.h"

//----------------------------------------------------------------------------------------------------------------------

const char* error_message[] = { "Done Successfully",                           // 0 both
                                "Failed to create logfile",                    // 1
                                "Incorrect number of command line arguments",  // 2
                                "Failed to open input file",                   // 3

                                "Invalid type of input file",                  // 4
                                "Outdated version of code",                    // 5
                                "Incorrect version of code",                   // 6
                                "Input file is empty",                         // 7

                                "Failed to create buffer",                     // 8
                                "Failed to write code to file",                // 9
                                "Too few elements in stack",                   // 10 C

                                "Division by zero",                            // 11
                                "Entered invalid value",                       // 12 C
                                "Failed to create dump file",                  // 13 C
                                "Failed to create array of commands",          // 14

                                "Failed to create array of code",              // 15
                                "Failed to resize array of code",              // 16

                                "Syntax error: incorrect command",             // 17 A
                                "Syntax error: incorrect argument",            //18 A
                                "Syntax error: too few arguments to such command", // 19 A
                                "Syntax error: too many arguments to such command", // 20 A

                                "Syntax error: invalid label name",           // 21 A
                                "Syntax error: too many labels in a row",     // 22 A
                                "Syntax error: incorrect jump command",       // 23 A
                                "This label already exists",                  // 24 A
                                "Jump to nonexistent label",                  // 25 A

                                "Nonexistent register",                       // 26 A
                                "Failed to create output file",               // 27
                                "Failed to push bytes to code",               // 28
                                "Failed to create array of labels",           // 29
                                "Failed to create array of jump addresses",   // 30
                                "Failed to resize array of jump addresses" }; // 31

//----------------------------------------------------------------------------------------------------------------------

void dump_to_console(int err)
{
    printf("---\nERROR: ");
    printf(error_message[err]);
    printf("\n---");
}

//----------------------------------------------------------------------------------------------------------------------

void dump_to_logfile(FILE* logfile, int err)
{
    fprintf(logfile, "ERROR: ");
    fprintf(logfile, error_message[err]);
    fprintf(logfile, "\n");
}

//----------------------------------------------------------------------------------------------------------------------