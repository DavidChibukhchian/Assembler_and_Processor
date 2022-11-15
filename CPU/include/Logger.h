#ifndef _LOGGER_H_
#define _LOGGER_H_

enum Errors
{
    Done_Successfully                  = 0,
    Failed_To_Create_Logfile           = 1,
    Incorrect_Number_Of_CMD_Arguments  = 2,
    Failed_To_Open_Input_File          = 3,
    Input_File_Is_Empty                = 4,
    Failed_To_Create_Buffer            = 5,
    Missed_HLT_Command                 = 6,
    Too_Few_Elements_In_Stack          = 7,
    Entered_Invalid_Value              = 8,
    Failed_To_Create_Dump_File         = 9,
    Failed_To_Create_Array_Of_Commands = 10,
    Failed_To_Create_Array_Of_Code     = 11,
    Failed_To_Resize_Array_Of_Code     = 12,
    Invalid_Syntax                     = 13,
    Failed_To_Create_Output_File       = 14
};

void dump(FILE* logfile, int err);

#endif
