#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "Buffer.h"
#include "ASM.h"

//----------------------------------------------------------------------------------------------------------------------

#define MULTIPLIER 1.5
#define SPACE 1
#define BUFF_SIZE 25

enum Commands
{
    CMD_HLT  = 0,
    CMD_PUSH = 1,
    CMD_ADD  = 2,
    CMD_SUB  = 3,
    CMD_MUL  = 4,
    CMD_DIV  = 5,
    CMD_OUT  = 6,
    CMD_IN   = 7,
    CMD_DUMP = 8
};

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

    code->offset += sizeof(char);
}

//----------------------------------------------------------------------------------------------------------------------

static void codePushInt(code_struct* code, int value)
{
    if ((code->size - code->offset) < sizeof(int))
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

    code->offset += sizeof(int);
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
    int scanned_symbols = 0;

    for (size_t i = 0; i < commands->number_of_commands; i++)
    {
        sscanf(commands->array_of_commands[i], "%s%n", cmd, &scanned_symbols);

        if (strcmp(cmd, "PUSH") == 0)
        {
            codePushChar(code, CMD_PUSH);
            if (code->err)
            {
                free_buffer(commands);
                return code->err;
            }

            sscanf(commands->array_of_commands[i] + scanned_symbols + SPACE, "%d", &value);
            codePushInt(code, value);
        }

        else if (strcmp(cmd, "ADD") == 0)
        {
            codePushChar(code, CMD_ADD);
        }
        else if (strcmp(cmd, "SUB") == 0)
        {
            codePushChar(code, CMD_SUB);
        }
        else if (strcmp(cmd, "MUL") == 0)
        {
            codePushChar(code, CMD_MUL);
        }
        else if (strcmp(cmd, "DIV") == 0)
        {
            codePushChar(code, CMD_DIV);
        }
        else if (strcmp(cmd, "OUT") == 0)
        {
            codePushChar(code, CMD_OUT);
        }
        else if (strcmp(cmd, "IN") == 0)
        {
            codePushChar(code, CMD_IN);
        }
        else if (strcmp(cmd, "DUMP") == 0)
        {
            codePushChar(code, CMD_DUMP);
        }
        else if (strcmp(cmd, "HLT") == 0)
        {
            codePushChar(code, CMD_HLT);
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