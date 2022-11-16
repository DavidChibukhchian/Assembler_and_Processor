#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "Buffer.h"

#include "ASM.h"

//----------------------------------------------------------------------------------------------------------------------

#define MULTIPLIER 1.5
#define BUFF_SIZE 35
#define SPACE 1

//----------------------------------------------------------------------------------------------------------------------

static const int SIGNATURE = 0x123ABCD;
static const char  VERSION = 7;

#define SIZEOF_SIGNATURE sizeof(int)
#define SIZEOF_VERSION   sizeof(char)

//----------------------------------------------------------------------------------------------------------------------

enum Commands
{
    CMD_HLT  = 0,
    CMD_PUSH = 1,
    CMD_POP  = 2,
    CMD_ADD  = 3,
    CMD_SUB  = 4,
    CMD_MUL  = 5,
    CMD_DIV  = 6,
    CMD_OUT  = 7,
    CMD_IN   = 8,
    CMD_DUMP = 9
};

enum ARG_type
{
    ARG_IMMED = 0,
    ARG_REG   = 1,
    ARG_MEM   = 2
};

enum REG
{
    REG_RAX = 0,
    REG_RBX = 1,
    REG_RCX = 2,
    REG_RDX = 3
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
    char* new_pointer = nullptr;
    if (code->size > 1)
    {
        new_pointer =(char*)realloc (code->pointer, code->size * MULTIPLIER);
        code->size *= MULTIPLIER;
    }
    else
    {
        new_pointer = (char*)realloc (code->pointer, code->size + sizeof(int));
        code->size += sizeof(int);
    }

    if (new_pointer == nullptr)
    {
        code->err = Failed_To_Resize_Array_Of_Code;
        return;
    }

    if (code->pointer != new_pointer)
        free(code->pointer);

    code->pointer = new_pointer;
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

static void codePushSignature(code_struct* code)
{
    codePushInt(code, SIGNATURE);
}

//----------------------------------------------------------------------------------------------------------------------

static void codePushVersion(code_struct* code)
{
    codePushChar(code, VERSION);
}

//----------------------------------------------------------------------------------------------------------------------

static void codeCtor(code_struct* code, size_t number_of_commands)
{
    code->err    = 0;
    code->offset = 0;
    code->size   = SIZEOF_SIGNATURE + SIZEOF_VERSION + number_of_commands;

    code->pointer = (char*)calloc (code->size, sizeof(char));
    if (code->pointer == nullptr)
    {
        code->err = Failed_To_Create_Array_Of_Code;
    }

    codePushSignature(code);
    codePushVersion(code);
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
        fclose(ASM_in);
        return Input_File_Is_Empty;
    }

    char* buffer = read_file_to_buffer(ASM_in, filesize);
    if (buffer == nullptr)
    {
        fclose(ASM_in);
        return Failed_To_Create_Buffer;
    }
    commands->buffer = buffer;

    commands->number_of_commands = count_lines(buffer);
   
    char** array_of_commands = split_buffer(buffer, commands->number_of_commands, filesize);
    if (array_of_commands == nullptr)
    {
        free(buffer);
        fclose(ASM_in);
        return Failed_To_Create_Array_Of_Commands;
    }

    commands->array_of_commands = array_of_commands;

    fclose(ASM_in);
    return Done_Successfully;
}

//----------------------------------------------------------------------------------------------------------------------

int create_code_array(commands_struct* commands, code_struct* code)
{
    int err = 0;
    int HLT_was_set = false;

    codeCtor(code, commands->number_of_commands);
    if (code->err)
    {
        free_buffer(commands);
        return code->err;
    }

    char cmd[BUFF_SIZE] = "";
    int value = 0;
    size_t scanned_symbols = 0;

    for (size_t i = 0; i < commands->number_of_commands; i++)
    {
        sscanf(commands->array_of_commands[i], "%s%n", cmd, &scanned_symbols);

        if (strcmp(cmd, "push") == 0)
        {
            codePushChar(code, CMD_PUSH);

            if (sscanf(commands->array_of_commands[i] + scanned_symbols + SPACE, "%d", &value))
            {
                codePushChar(code, ARG_IMMED);
                codePushInt(code, value);
            }
            else
            {
                sscanf(commands->array_of_commands[i] + scanned_symbols + SPACE, "%s%n", cmd, &scanned_symbols);

                if ((scanned_symbols == 3) && (cmd[0] == 'r') && (cmd[2] == 'x'))
                {
                    codePushChar(code, ARG_REG);
                    codePushChar(code, cmd[1] - 'a' + 1);
                }
                else if (strcmp(cmd, "mem") == 0)
                {
                    codePushChar(code, ARG_MEM);
                }
                else
                {
                    free_buffer(commands);
                    free(code->pointer);
                    return Invalid_Syntax;
                }
            }

            if (code->err)
            {
                free_buffer(commands);
                return code->err;
            }
        }

        else if (strcmp(cmd, "pop") == 0)
        {
            codePushChar(code, CMD_POP);

            sscanf(commands->array_of_commands[i] + scanned_symbols + SPACE, "%s%n", cmd, &scanned_symbols);
            if ((scanned_symbols == 3) && (cmd[0] == 'r') && (cmd[2] == 'x'))
            {
                codePushChar(code, ARG_REG);
                codePushChar(code, cmd[1] - 'a' + 1);
            }
            else
            {
                free_buffer(commands);
                free(code->pointer);
                return Invalid_Syntax;
            }

            if (code->err)
            {
                free_buffer(commands);
                return code->err;
            }
        }

        else if (strcmp(cmd, "add") == 0)
        {
            codePushChar(code, CMD_ADD);
            if (code->err)
            {
                free_buffer(commands);
                return code->err;
            }
        }

        else if (strcmp(cmd, "sub") == 0)
        {
            codePushChar(code, CMD_SUB);

            if (code->err)
            {
                free_buffer(commands);
                return code->err;
            }
        }

        else if (strcmp(cmd, "mul") == 0)
        {
            codePushChar(code, CMD_MUL);
            if (code->err)
            {
                free_buffer(commands);
                return code->err;
            }
        }

        else if (strcmp(cmd, "div") == 0)
        {
            codePushChar(code, CMD_DIV);
        }

        else if (strcmp(cmd, "out") == 0)
        {
            codePushChar(code, CMD_OUT);
        }

        else if (strcmp(cmd, "in") == 0)
        {
            codePushChar(code, CMD_IN);
        }

        else if (strcmp(cmd, "dump") == 0)
        {
            codePushChar(code, CMD_DUMP);
        }

        else if (strcmp(cmd, "hlt") == 0)
        {
            HLT_was_set = true;
            codePushChar(code, CMD_HLT);
        }

        else
        {
            free_buffer(commands);
            free(code->pointer);
            return Invalid_Syntax;
        }
    }

    if ((code->pointer[code->offset - 1] != CMD_HLT) | (!HLT_was_set))
    {
        free_buffer(commands);
        free(code->pointer);
        return Missed_HLT_Command;
    }

    codeShrinkToFit(code);
    if (code->err)
    {
        free_buffer(commands);
        return code->err;
    }

    printf("%zu", code->offset);
    free_buffer(commands);
    return Done_Successfully;
}

//----------------------------------------------------------------------------------------------------------------------

int write_code_to_file(code_struct* code, FILE* ASM_out)
{
    size_t written_bytes = 0;
    written_bytes = fwrite(code->pointer, sizeof(char), code->offset, ASM_out);

    if (written_bytes != code->offset)
    {
        fclose(ASM_out);
        free(code->pointer);
        return Failed_To_Write_Code_To_File;
    }

    fclose(ASM_out);
    free(code->pointer);

    return Done_Successfully;
}

//----------------------------------------------------------------------------------------------------------------------