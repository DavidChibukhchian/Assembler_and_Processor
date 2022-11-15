#include <stdio.h>
#include "Logger.h"

//----------------------------------------------------------------------------------------------------------------------

const char* error_messages[] = { "Done Successfully",                           // 0
                                 "Failed to create logfile",                    // 1
                                 "Incorrect number of command line arguments",  // 2
                                 "Failed to open input file",                   // 3
                                 "Input file is empty",                         // 4
                                 "Failed to create buffer",                     // 5
                                 "Missed HLT command",                          // 6
                                 "Too few elements in stack",                   // 7
                                 "Entered invalid value",                       // 8
                                 "Failed to create dump file",                  // 9
                                 "Failed to create array of commands",          // 10
                                 "Failed to create array of code",              // 11
                                 "Failed to resize array of code",              // 12
                                 "Invalid syntax",                              // 13
                                 "Failed to create output file" };              // 14

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