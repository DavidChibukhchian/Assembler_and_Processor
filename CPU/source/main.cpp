#include "CPU.h"

static const size_t NUMBER_OF_REGISTERS = 5;
//static const size_t SIZE_OF_RAM = 1024;

int main(int argc, char* argv[])
{
    int err = 0;

    int REG[NUMBER_OF_REGISTERS] = {0};
//    int RAM[SIZE_OF_RAM] = {0};

//    char str[] = "1234   ";
//    sscanf(str, "%s%n", );

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

    err = run_code(code, &stk, REG);
    VERIFY(err);

    printf("RAX: %d\n", REG[1]); printf("RBX: %d\n", REG[2]); printf("RCX: %d\n", REG[3]); printf("RDX: %d\n", REG[4]);

    stackDisplay(&stk);
    stackDtor(&stk);

    return 0;
}