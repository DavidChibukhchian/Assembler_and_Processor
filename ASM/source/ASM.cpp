#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "Buffer.h"
#include "ASM.h"

//----------------------------------------------------------------------------------------------------------------------

#define BUFF_SIZE 100
#define LENGTH_OF_PUSH 5

//TODO enum
#define CMD_PUSH 1
#define CMD_ADD  2
#define CMD_SUB  3
#define CMD_MUL  4
#define CMD_DIV  5
#define CMD_OUT  6
#define CMD_HLT  7
#define CMD_DUMP 8

//----------------------------------------------------------------------------------------------------------------------

int record_commands_to_buffer(FILE* ASM_in, commands_struct* commands)
{
    size_t filesize = get_filesize(ASM_in);
    if (filesize == 0)
    {
        return Input_File_Is_Empty;
    }

    char* buffer = read_file_to_buffer(ASM_in, filesize);
    if (buffer == nullptr)
    {
        return Failed_To_Create_Buffer;
    }

    commands->number_of_commands = count_lines(buffer);
   
    char** array_of_commands = split_buffer(buffer, commands->number_of_commands, filesize);
    if (array_of_commands == nullptr)
    {
        return Failed_To_Create_Array_Of_Commands;
    }

    commands->array_of_commands = array_of_commands;

    return Done_Successfully;
}

//----------------------------------------------------------------------------------------------------------------------

int create_code_array(commands_struct* commands, code_struct* code)
{
    int* data = (int*)calloc (2 * commands->number_of_commands, sizeof(int));
    if (data == nullptr)
    {
        return Failed_To_Create_Array_Of_Code;
    }

    size_t counter = 0;

    char cmd[BUFF_SIZE] = "";
    for (size_t i = 0; i < commands->number_of_commands; i++)
    {
        sscanf(commands->array_of_commands[i], "%s", cmd);

        if (strcmp(cmd, "PUSH") == 0)
        {
            int value = 0;
            sscanf(commands->array_of_commands[i] + LENGTH_OF_PUSH, "%d", &value);
            data[counter] = CMD_PUSH;
            data[counter + 1] = value;
            counter += 2;
        }
        else if (strcmp(cmd, "ADD") == 0)
        {
            data[counter] = CMD_ADD;
            counter++;
        }
        else if (strcmp(cmd, "SUB") == 0)
        {
            data[counter] = CMD_SUB;
            counter++;
        }
        else if (strcmp(cmd, "MUL") == 0)
        {
            data[counter] = CMD_MUL;
            counter++;
        }
        else if (strcmp(cmd, "DIV") == 0)
        {
            data[counter] = CMD_DIV;
            counter++;
        }
        else if (strcmp(cmd, "OUT") == 0)
        {
            data[counter] = CMD_OUT;
            counter++;
        }
        else if (strcmp(cmd, "HLT") == 0)
        {
            data[counter] = CMD_HLT;
            counter++;
        }
        else if (strcmp(cmd, "DUMP") == 0)
        {
            data[counter] = CMD_DUMP;
            counter++;
        }
        else
        {
            free(data);
            return Invalid_Syntax;
        }
    }

    code->number_of_elements = counter;

    int* code_pointer = (int*)realloc (data, (counter - 1) * sizeof(int));
    if (code_pointer == nullptr)
    {
        free(data);
        return Failed_To_Create_Array_Of_Code;
    }
    code->pointer = code_pointer;

    free(commands->array_of_commands);
    return Done_Successfully;
}

//----------------------------------------------------------------------------------------------------------------------