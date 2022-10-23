#include <stdio.h>
#include "ASM.h"

int main()
{
    FILE* ASM_in = fopen("C:\\Users\\david\\Desktop\\Projects CLion\\Task4\\ASM\\ASM_in.txt", "r");
    if (ASM_in == nullptr)
    {
        return -1;
    }
    program_text Program = read_input_file(ASM_in);
    fclose(ASM_in);


    FILE* ASM_out = fopen("C:\\Users\\david\\Desktop\\Projects CLion\\Task4\\ASM\\ASM_out.txt", "w");
    if (ASM_out == nullptr)
    {
        return -1;
    }
    record_code_to_file(ASM_out, &Program);
    fclose(ASM_out);

    return 0;
}
