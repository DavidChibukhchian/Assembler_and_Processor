#ifndef _LOGGER_H_
#define _LOGGER_H_

//----------------------------------------------------------------------------------------------------------------------

#include <stdio.h>

//----------------------------------------------------------------------------------------------------------------------

enum Errors
{
    Done_Successfully                        = 0,
    Failed_To_Create_Logfile                 = 1,
    Incorrect_Number_Of_CMD_Arguments        = 2,
    Failed_To_Open_Input_File                = 3,

    Invalid_Type_Of_Input_File               = 4,
    Outdated_Version_Of_Code                 = 5,
    Incorrect_Version_Of_Code                = 6,
    Input_File_Is_Empty                      = 7,

    Failed_To_Create_Buffer                  = 8,
    Failed_To_Write_Code_To_File             = 9,
    Missed_HLT_Command                       = 10,
    Too_Few_Elements_In_Stack                = 11,

    Division_By_Zero                         = 12,
    Entered_Invalid_Value                    = 13,
    Failed_To_Create_Dump_File               = 14,
    Failed_To_Create_Array_Of_Commands       = 15,

    Failed_To_Create_Array_Of_Code           = 16,
    Failed_To_Resize_Array_Of_Code           = 17,

    Incorrect_Command                        = 18,
    Incorrect_Argument                       = 19,
    Too_Few_Arguments_To_Such_Command        = 20,
    Too_Many_Arguments_To_Such_Command       = 21,
    Invalid_Label_Name                       = 22,
    Too_Many_Labels_In_A_Row                 = 23,
    Incorrect_Jump_Command                   = 24,
    This_Label_Already_Exists                = 25,
    Jump_To_Nonexistent_Label                = 26,
    Nonexistent_Register                     = 27,
    Failed_To_Create_Output_File             = 28,
    Failed_To_Push_Bytes_To_Code             = 29,
    Failed_To_Create_Array_Of_Labels         = 30,
    Failed_To_Create_Array_Of_Jump_Addresses = 31,
    Failed_To_Resize_Array_Of_Jump_Addresses = 32
};

//----------------------------------------------------------------------------------------------------------------------

void dump_to_console(int err);

void dump_to_logfile(FILE* logfile, int err);

//----------------------------------------------------------------------------------------------------------------------

#endif // _LOGGER_H_