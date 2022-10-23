#include <stdio.h>
#include <malloc.h>
#include "Stack.h"

//----------------------------------------------------------------------------------------------------------------------

#define CMD_PUSH 1
#define CMD_ADD  2
#define CMD_SUB  3
#define CMD_MUL  4
#define CMD_DIV  5
#define CMD_OUT  6
#define CMD_HLT  7
#define CMD_DUMP 8

//----------------------------------------------------------------------------------------------------------------------

int* read_input_file(FILE* ASM_out)
{
    size_t number_of_lines = 0;
    fscanf(ASM_out, "%zu", &number_of_lines);

    int* code = (int*)calloc (number_of_lines, sizeof(int));

    for (size_t i = 0; i < number_of_lines; i++)
    {
        fscanf(ASM_out, "%d", &code[i]);
    }

    return code;
}

//----------------------------------------------------------------------------------------------------------------------

void run_code(int* code, Stack* stk)
{
    size_t ip = 0;
    int num1, num2 = 0;
    while (code[ip] != CMD_HLT)
    {
        switch(code[ip])
        {
            case CMD_PUSH:
                stackPush(stk, code[ip+1]);
                ip+=2;
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
                stackPush(stk, num1 - num2);
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
                stackPush(stk, num1/num2);
                ip++;
                break;

                //

            case CMD_HLT:
                stackDtor(stk);
                break;
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------
