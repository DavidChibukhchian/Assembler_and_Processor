#include <stdio.h>
#include <malloc.h>
#include <assert.h>
#include <sys\stat.h>
#include "CPU.h"

//----------------------------------------------------------------------------------------------------------------------

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

size_t get_filesize(FILE* filename)
{
    struct stat file_data{};
    fstat(fileno(filename), &file_data);

    return file_data.st_size;
}

//----------------------------------------------------------------------------------------------------------------------

char* read_code_to_buffer(FILE* ASM_out, int* err)
{
    size_t filesize = get_filesize(ASM_out);
    if (filesize == 0)
    {
        fclose(ASM_out);
        *err = Input_File_Is_Empty;
        return nullptr;
    }

    char* code = (char*)calloc (filesize, sizeof(char));
    if (code == nullptr)
    {
        fclose(ASM_out);
        *err = Failed_To_Create_Buffer;
        return code;
    }

    fread(code, sizeof(char), filesize, ASM_out);

    return code;
}

//----------------------------------------------------------------------------------------------------------------------

void run_code(char* code, Stack* stk)
{
    size_t ip = 0;
    int num1, num2 = 0;
    while (code[ip] != CMD_HLT)
    {
        switch(code[ip])
        {
            case CMD_PUSH:
                ip++;
                stackPush(stk, *(int*)(code + ip));
                ip += sizeof(int);
                break;

            case CMD_ADD:
                stackPop(stk, &num1);
                stackPop(stk, &num2);
                stackPush(stk, num1 + num2);
                ip++;
                break;

            case CMD_SUB:
                stackPop(stk, &num1);
                stackPop(stk, &num2);
                stackPush(stk, num2 - num1);
                ip++;
                break;

            case CMD_MUL:
                stackPop(stk, &num1);
                stackPop(stk, &num2);
                stackPush(stk, num1 * num2);
                ip++;
                break;

            case CMD_DIV:
                stackPop(stk, &num1);
                stackPop(stk, &num2);
                stackPush(stk, num2 / num1);
                ip++;
                break;

            case CMD_OUT:
                ip++;
                break;

            case CMD_IN:
                scanf("%d", &num1);
                stackPush(stk, num1);
                ip++;
                break;

            case CMD_HLT:
                ip++;
                break;

            case CMD_DUMP:
                ip++;
                break;
        }
    }
    free(code);
}

//----------------------------------------------------------------------------------------------------------------------