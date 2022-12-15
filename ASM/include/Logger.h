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
    Too_Few_Elements_In_Stack                = 10,

    Division_By_Zero                         = 11,
    Entered_Invalid_Value                    = 12,
    Failed_To_Create_Dump_File               = 13,
    Failed_To_Create_Array_Of_Commands       = 14,

    Failed_To_Create_Array_Of_Code           = 15,
    Failed_To_Resize_Array_Of_Code           = 16,

    Incorrect_Command                        = 17,
    Incorrect_Argument                       = 18,
    Too_Few_Arguments_To_Such_Command        = 19,
    Too_Many_Arguments_To_Such_Command       = 20,
    Invalid_Label_Name                       = 21,
    Too_Many_Labels_In_A_Row                 = 22,
    Incorrect_Jump_Command                   = 23,
    This_Label_Already_Exists                = 24,
    Jump_To_Nonexistent_Label                = 25,
    Operating_With_Nonexistent_Register      = 26,

    Failed_To_Create_Output_File             = 27,
    Failed_To_Push_Bytes_To_Code             = 28,
    Failed_To_Create_Array_Of_Labels         = 29,
    Failed_To_Resize_Array_Of_Labels         = 30,
    Failed_To_Create_Array_Of_Jump_Addresses = 31,
    Failed_To_Resize_Array_Of_Jump_Addresses = 32
};

//----------------------------------------------------------------------------------------------------------------------

void dump_to_console(int err, size_t err_line = 0);

void dump_to_logfile(FILE* logfile, int err, size_t err_line = 0);

//----------------------------------------------------------------------------------------------------------------------

#endif // _LOGGER_H_