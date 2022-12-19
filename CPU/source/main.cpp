#include "CPU.h"

int RAM[SIZE_OF_RAM] = {0};
int REG[NUMBER_OF_REGISTERS] = {0};

int main(int argc, char* argv[])
{
    files_struct files = {};
    int err = open_logfile(&files);
    if (err) return err;

    ASSERT(argc == 2, Incorrect_Number_Of_CMD_Arguments);

    err = open_files(&files, argv);
    VERIFY_err;

    Code code = nullptr;
    err = read_code(&code, &files);
    VERIFY_err;

    Stack stack = {};
    stack_Ctor(&stack);

    err = run_code(code, &stack, REG, RAM);
    VERIFY_err;

    close_files(&files);
    
    stack_Dtor(&stack);

    return 0;
}
