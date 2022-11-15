#include <stdio.h>
#include "Logger.h"

//----------------------------------------------------------------------------------------------------------------------

const char* error_messages[] = { "Done Successfully",                           // 0
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
                                 "Invalid syntax",                              // 18
                                 "Failed to create output file" };              // 19

//----------------------------------------------------------------------------------------------------------------------

void dump(FILE* logfile, int err)
{
    printf("---\nERROR: ");
    printf(error_messages[err]);
    printf("\n---");

    fprintf(logfile, "ERROR: ");
    fprintf(logfile, error_messages[err]);
    fprintf(logfile, "\n");
}

//----------------------------------------------------------------------------------------------------------------------