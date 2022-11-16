#include "Stack.h"
#include "CPU.h"

#define NUMBER_OF_REGISTERS 5

int main(int argc, char* argv[])
{
    int err = 0;
    int reg[NUMBER_OF_REGISTERS] = {0};

    FILE* logfile = fopen("CPU_logfile.txt",  "w");
    CHECK_LOGFILE(logfile);

    ASSERT(argc == 2, Incorrect_Number_Of_CMD_Arguments);

    FILE* ASM_out = fopen(argv[1], "rb");
    ASSERT(ASM_out != nullptr, Failed_To_Open_Input_File);

    err = check_file(ASM_out);
    VERIFY(err);

    char* code = read_code_to_buffer(ASM_out, &err);
    VERIFY(err);

    Stack stk = {};
    stackCtor(&stk);

    err = run_code(code, &stk, reg);
    VERIFY(err);

    printf("%d %d %d %d", reg[1], reg[2], reg[3], reg[4]);

    stackDisplay(&stk);
    stackDtor(&stk);

    return 0;
}