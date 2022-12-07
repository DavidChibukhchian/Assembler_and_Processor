#include "Logger.h"

//----------------------------------------------------------------------------------------------------------------------

const char* error_message[] = { "Done Successfully",                           // 0
                                "Failed to create logfile",                    // 1
                                "Incorrect number of command line arguments",  // 2
                                "Failed to open input file",                   // 3

                                "Invalid type of input file",                  // 4
                                "Outdated version of code",                    // 5
                                "Incorrect version of code",                   // 6
                                "Input file is empty",                         // 7

                                "Failed to create buffer",                     // 8
                                "Failed to write code to file",                // 9
                                "Missed HLT command",                          // 10
                                "Too few elements in stack",                   // 11

                                "Division by zero",                            // 12
                                "Entered invalid value",                       // 13
                                "Failed to create dump file",                  // 14
                                "Failed to create array of commands",          // 15

                                "Failed to create array of code",              // 16
                                "Failed to resize array of code",              // 17

                                "Syntax error: incorrect command",             // 18
                                "Syntax error: incorrect argument",            //19
                                "Syntax error: too few arguments to such command", // 20
                                "Syntax error: too many arguments to such command", // 21

                                "Syntax error: invalid label name",           // 22
                                "Syntax error: too many labels in a row",     // 23
                                "Syntax error: incorrect jump command",       // 24
                                "This label already exists",                  // 25
                                "Jump to nonexistent label",                  // 26

                                "Nonexistent register",                       // 27
                                "Failed to create output file",               // 28
                                "Failed to push bytes to code",               // 29
                                "Failed to create array of labels",           // 30
                                "Failed to create array of jump addresses",   // 31
                                "Failed to resize array of jump addresses" }; // 32

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