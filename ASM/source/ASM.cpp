#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "Buffer.h"
#include "ASM.h"

//----------------------------------------------------------------------------------------------------------------------

#define MULTIPLIER 1.5
#define BUFF_SIZE 25
#define LENGTH_OF_PUSH 5

//TODO enum
//#define CMD_PUSH 1
#define CMD_ADD  2
#define CMD_SUB  3
#define CMD_MUL  4
#define CMD_DIV  5
#define CMD_OUT  6
#define CMD_HLT  7
#define CMD_DUMP 8

char CMD_PUSH = 1;

//----------------------------------------------------------------------------------------------------------------------

static void codeShrinkToFit(code_struct* code)
{
    char* new_pointer = (char*)realloc (code->pointer, code->offset);
    if (new_pointer == nullptr)
    {
        free(code->pointer);
        code->err = Failed_To_Create_Array_Of_Code;
    }
}

//----------------------------------------------------------------------------------------------------------------------

static void codeExpand(code_struct* code)
{
    char* new_pointer = (char*)realloc (code->pointer, code->size * MULTIPLIER);
    if (new_pointer == nullptr)
    {
        code->err = Failed_To_Resize_Array_Of_Code;
        return;
    }

    if (code->pointer != new_pointer)
        free(code->pointer);

    code->pointer = new_pointer;
    code->size *= MULTIPLIER;
}

//----------------------------------------------------------------------------------------------------------------------

static void codeCtor(code_struct* code, size_t number_of_commands)
{
    code->err    = 0;
    code->offset = 0;
    code->size   = number_of_commands;

    code->pointer = (char*)calloc (number_of_commands, sizeof(char));
    if (code->pointer == nullptr)
    {
        code->err = Failed_To_Create_Array_Of_Code;
        return;
    }
}

//----------------------------------------------------------------------------------------------------------------------

static void codePushChar(code_struct* code, char value)
{
    if (code->offset == code->size)
    {
        codeExpand(code);
        if (code->err)
        {
            free(code->pointer);
            return;
        }
    }

    code->pointer[code->offset] = value;

    code->offset++;
}

//----------------------------------------------------------------------------------------------------------------------

static void codePushInt(code_struct* code, int value)
{
    if ((code->size - code->offset) < 4)
    {
        codeExpand(code);
        if (code->err)
        {
            free(code->pointer);
            return;
        }
    }

    int* ptr = (int*)(code->pointer + code->offset);
    *ptr = value;

    code->offset += 4;
}

//----------------------------------------------------------------------------------------------------------------------

void free_buffer(commands_struct* commands)
{
    free(commands->buffer);
    free(commands->array_of_commands);
}

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
    commands->buffer = buffer;

    commands->number_of_commands = count_lines(buffer);
   
    char** array_of_commands = split_buffer(buffer, commands->number_of_commands, filesize);
    if (array_of_commands == nullptr)
    {
        free(buffer);
        return Failed_To_Create_Array_Of_Commands;
    }

    commands->array_of_commands = array_of_commands;

    return Done_Successfully;
}

//----------------------------------------------------------------------------------------------------------------------

int create_code_array(commands_struct* commands, code_struct* code)
{
    int err = 0;
    codeCtor(code, commands->number_of_commands);
    if (code->err)
    {
        free_buffer(commands);
        return code->err;
    }

    char cmd[BUFF_SIZE] = "";
    int value = 0;

    for (size_t i = 0; i < commands->number_of_commands; i++)
    {
        sscanf(commands->array_of_commands[i], "%s", cmd);

        if (strcmp(cmd, "PUSH") == 0)
        {
            codePushChar(&code, CMD_PUSH);
            if (code->err)
            {
                free_buffer(commands);
                return code->err;
            }

            sscanf(commands->array_of_commands[i] + LENGTH_OF_PUSH, "%d", &value);
        }




        else if (strcmp(cmd, "ADD") == 0)
        {
            codePushChar(cmd);

//            data[counter] = CMD_ADD;
//            counter++;
        }
        else if (strcmp(cmd, "SUB") == 0)
        {
//            data[counter] = CMD_SUB;
//            counter++;
        }
        else if (strcmp(cmd, "MUL") == 0)
        {
//            data[counter] = CMD_MUL;
//            counter++;
        }
        else if (strcmp(cmd, "DIV") == 0)
        {
//            data[counter] = CMD_DIV;
//            counter++;
        }
        else if (strcmp(cmd, "OUT") == 0)
        {
//            data[counter] = CMD_OUT;
//            counter++;
        }
        else if (strcmp(cmd, "HLT") == 0)
        {
//            data[counter] = CMD_HLT;
//            counter++;
        }
        else if (strcmp(cmd, "DUMP") == 0)
        {
//            data[counter] = CMD_DUMP;
//            counter++;
        }
        else
        {
            free_buffer(commands);
            free(code->pointer);
            return Invalid_Syntax;
        }
    }

    codeShrinkToFit(code);
    if (code->err)
    {
        free_buffer(commands);
        return code->err;
    }

    free_buffer(commands);
    return Done_Successfully;
}

//----------------------------------------------------------------------------------------------------------------------
