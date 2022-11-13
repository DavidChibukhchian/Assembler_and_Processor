#include "Stack.h"
#include "CPU.h"
#include "Logger.h"

int main(int argc, char* argv[])
{
    int err = 0;

    FILE* logfile = fopen("CPU_logfile.txt",  "w");
    if (logfile == nullptr)
    {
        printf("---\nERROR: Failed to create logfile\n---");
        return Failed_To_Create_Logfile;
    }

    ASSERT(argc == 2, Incorrect_Number_Of_CMD_Arguments,);

    FILE* ASM_out = fopen(argv[1], "rb");
    ASSERT(ASM_out != nullptr, Failed_To_Open_Input_File,);

    char* code = read_code_to_buffer(ASM_out, &err);
    VERIFY(err);
    fclose(ASM_out);

    Stack stk = {};
    stackCtor(&stk);

    run_code(code, &stk);

    stackDisplay(&stk);
    stackDtor(&stk);

    return 0;
}