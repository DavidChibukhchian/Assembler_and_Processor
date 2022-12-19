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
    Input_File_Is_Empty                      = 4,
    Failed_To_Create_Array_Of_Code           = 5,

    Failed_To_Create_Output_File             = 6,
    Failed_To_Create_Buffer                  = 7,
    Failed_To_Create_Array_Of_Commands       = 8,
    Failed_To_Resize_Array_Of_Code           = 9,
    Failed_To_Create_Array_Of_Labels         = 10,
    Failed_To_Resize_Array_Of_Labels         = 11,
    Failed_To_Create_Array_Of_Jump_Addresses = 12,
    Failed_To_Resize_Array_Of_Jump_Addresses = 13,
    Failed_To_Write_Code_To_File             = 14,
    Incorrect_Command                        = 15,
    Incorrect_Argument                       = 16,
    Too_Few_Arguments_To_Such_Command        = 17,
    Too_Many_Arguments_To_Such_Command       = 18,
    Invalid_Label_Name                       = 19,
    Too_Many_Labels_In_A_Row                 = 20,
    Incorrect_Jump_Command                   = 21,
    This_Label_Already_Exists                = 22,
    Jump_To_Nonexistent_Label                = 23,
    Operating_With_Nonexistent_Register      = 24,

    Invalid_Type_Of_Input_File               = 25,
    Outdated_Version_Of_Code                 = 26,
    Incorrect_Version_Of_Code                = 27,
    Too_Few_Elements_In_Stack                = 28,
    Stack_Is_Empty                           = 29,
    Call_Stack_Is_Empty                      = 30,
    Address_Is_Bigger_Than_Size_Of_RAM       = 31,
    Infinite_Loop_Was_Detected               = 32,
    Division_By_Zero                         = 33,
    Entered_Invalid_Value                    = 34,
    Failed_To_Create_Dump_File               = 35
};

//----------------------------------------------------------------------------------------------------------------------

void dump_to_console(int err, size_t err_line = 0);

void dump_to_logfile(FILE* logfile, int err, size_t err_line = 0);

//----------------------------------------------------------------------------------------------------------------------

#endif // _LOGGER_H