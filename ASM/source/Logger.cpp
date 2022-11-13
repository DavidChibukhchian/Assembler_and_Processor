#include <stdio.h>
#include "Logger.h"

//----------------------------------------------------------------------------------------------------------------------

const char* error_message[] = { "Done Successfully",
                                "Failed to create logfile",
                                "Incorrect number of command line arguments",
                                "Failed to open input file",
                                "Input file is empty",
                                "Failed to create buffer",
                                "Failed to create array of commands",
                                "Failed to create array of code",
                                "Failed to resize array of code",
                                "Invalid syntax",
                                "Failed to create output file" };

//----------------------------------------------------------------------------------------------------------------------

void dump(FILE* logfile, int err)
{
    printf("---\nERROR: ");
    printf(error_message[err]);
    printf("\n---");

    fprintf(logfile, "ERROR: ");
    fprintf(logfile, error_message[err]);
    fprintf(logfile, "\n");
}

//----------------------------------------------------------------------------------------------------------------------