#ifndef _LOGGER_H_
#define _LOGGER_H_

enum Errors
{
    Done_Successfully                  = 0,
    Failed_To_Create_Logfile           = 1,
    Incorrect_CMD_Arguments            = 2,
    Failed_To_Open_Input_File          = 3,
    Input_File_Is_Empty                = 4,
    Failed_To_Create_Buffer            = 5,
    Failed_To_Create_Array_Of_Commands = 6,
    Failed_To_Create_Array_Of_Code     = 7,
    Failed_To_Resize_Array_Of_Code     = 8,
    Invalid_Syntax                     = 9,
    Failed_To_Create_Output_File       = 10
};

void dump(FILE* logfile, int err);

#endif
