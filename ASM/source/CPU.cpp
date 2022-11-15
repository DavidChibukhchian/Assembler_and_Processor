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

void fprintf_bytes(FILE* dump_file, int size)
{
    for (size_t i = 1; i <= size; i++)
    {
        (i / 10) ? (fprintf(dump_file, "%zu ", i)) : (fprintf(dump_file, "0%zu ", i));
    }

    fprintf(dump_file, "\n");
}

//----------------------------------------------------------------------------------------------------------------------

void fprintf_commands(FILE* dump_file, char* code, int size)
{
    for (size_t i = 0; i < size; i++)
    {
        (code[i] / 10) ? (fprintf(dump_file, "%d ", code[i])) : (fprintf(dump_file, "0%d ", code[i]));
    }
    fprintf(dump_file, "\n");
}

//----------------------------------------------------------------------------------------------------------------------

void fprintf_line(FILE* dump_file, size_t ip)
{
    for (size_t i = 0; i < ip - 1; i++)
    {
        fprintf(dump_file, "~~~");
    }
    fprintf(dump_file, "^ ip = %zu\n", ip);
}
//----------------------------------------------------------------------------------------------------------------------

void CPU_Dump(char* code, FILE* dump_file, size_t ip)
{
    int size = _msize(code);

    fprintf_bytes(dump_file, size);

    fprintf_commands(dump_file, code, size);

    fprintf_line(dump_file, ip);
}

//----------------------------------------------------------------------------------------------------------------------

int Dump(char* code, const Stack* stk, size_t ip)
{
    FILE* dump_file = fopen("CPU_dump.txt", "w");
    if (dump_file == nullptr)
    {
        return Failed_To_Create_Dump_File;
    }

    CPU_Dump(code, dump_file, ip);
    stackDump(stk, dump_file);

    fclose(dump_file);
    return Done_Successfully;
}

//----------------------------------------------------------------------------------------------------------------------

static int check_stack(const Stack* stk)
{
    if (stk->size < 2)
    {
        return Too_Few_Elements_In_Stack;
    }

    return Done_Successfully;
}

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

    fclose(ASM_out);
    return code;
}

//----------------------------------------------------------------------------------------------------------------------

int run_code(char* code, Stack* stk, int* registers)
{
    size_t ip = 0;
    int num1, num2 = 0;

    int err = 0;
    while (code[ip] != CMD_HLT)
    {
        switch(code[ip])
        {
            case CMD_PUSH:
                ip++;
                switch(code[ip])
                {
                    case ARG_IMMED:
                        ip++;
                        stackPush(stk, *(int*)(code + ip));
                        ip += sizeof(int);
                        break;

                    case ARG_REG:
                        ip++;
                        stackPush(stk, *(registers + code[ip]));
                        ip++;
                        break;

                    case ARG_MEM:
                        ip++;
                        break;
                }
                break;

            case CMD_ADD:
                err = check_stack(stk);
                if (err)
                    return err;

                stackPop(stk, &num1);
                stackPop(stk, &num2);
                stackPush(stk, num1 + num2);
                ip++;
                break;

            case CMD_SUB:
                err = check_stack(stk);
                if (err)
                    return err;

                stackPop(stk, &num1);
                stackPop(stk, &num2);
                stackPush(stk, num2 - num1);
                ip++;
                break;

            case CMD_MUL:
                err = check_stack(stk);
                if (err)
                    return err;

                stackPop(stk, &num1);
                stackPop(stk, &num2);
                stackPush(stk, num1 * num2);
                ip++;
                break;

            case CMD_DIV:
                err = check_stack(stk);
                if (err)
                    return err;

                stackPop(stk, &num1);
                stackPop(stk, &num2);
                stackPush(stk, num2 / num1);
                ip++;
                break;

            case CMD_OUT:
                printf("\nStack top element: [%d]\n\n", *stk->top_elem);
                ip++;
                break;

            case CMD_IN:
                err = scanf("%d", &num1);
                if (!err)
                    return Entered_Invalid_Value;

                stackPush(stk, num1);
                ip++;
                break;

            case CMD_HLT:
                break;

            case CMD_DUMP:
                ip++;
                err = Dump(code, stk, ip);
                if (err)
                    return err;
                break;

            default:
                return Invalid_Syntax;
                break;
        }
    }
    free(code);

    return Done_Successfully;
}

//----------------------------------------------------------------------------------------------------------------------