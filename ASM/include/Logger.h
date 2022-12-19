#ifndef _LOGGER_H
#define _LOGGER_H

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
    Too_Few_Elements_In_Stack                = 10,

    Stack_Is_Empty                           = 11,
    Address_Is_Bigger_Than_Size_Of_RAM       = 12,
    Infinite_Loop_Detected                   = 13,
    Division_By_Zero                         = 14,
    Entered_Invalid_Value                    = 15,
    Failed_To_Create_Dump_File               = 16,
    Failed_To_Create_Array_Of_Commands       = 17,

    Failed_To_Create_Array_Of_Code           = 18,
    Failed_To_Resize_Array_Of_Code           = 19,

    Incorrect_Command                        = 20,
    Incorrect_Argument                       = 21,
    Too_Few_Arguments_To_Such_Command        = 22,
    Too_Many_Arguments_To_Such_Command       = 23,
    Invalid_Label_Name                       = 24,
    Too_Many_Labels_In_A_Row                 = 25,
    Incorrect_Jump_Command                   = 26,
    This_Label_Already_Exists                = 27,
    Jump_To_Nonexistent_Label                = 28,
    Operating_With_Nonexistent_Register      = 29,

    Failed_To_Create_Output_File             = 30,
    Failed_To_Push_Bytes_To_Code             = 31,
    Failed_To_Create_Array_Of_Labels         = 32,
    Failed_To_Resize_Array_Of_Labels         = 33,
    Failed_To_Create_Array_Of_Jump_Addresses = 34,
    Failed_To_Resize_Array_Of_Jump_Addresses = 35
};

//----------------------------------------------------------------------------------------------------------------------

void dump_to_console(int err, size_t err_line = 0);

void dump_to_logfile(FILE* logfile, int err, size_t err_line = 0);

//----------------------------------------------------------------------------------------------------------------------

#endif // _LOGGER_H