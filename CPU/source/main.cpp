#include "Stack.h"
#include "CPU.h"

int main()
{
    FILE* ASM_out = fopen("ASM_out.txt", "r");
    if (ASM_out == nullptr)
    {
        return -1;
    }

    int* code = read_input_file(ASM_out);
    fclose(ASM_out);

    Stack stk1 = {};
    stackCtor(&stk1, 1);

    run_code(code, &stk1);

    stackDisplay(&stk1);

    return 0;
}
