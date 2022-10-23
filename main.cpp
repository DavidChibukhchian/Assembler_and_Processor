#include <stdio.h>
#include "ASM.h"

int main()
{
    FILE* ASM_in = fopen("C:\\Users\\david\\Desktop\\Projects CLion\\Task4\\ASM\\ASM_in.txt", "r");
    if (ASM_in == nullptr)
    {
        return -1;
    }
    text_struct text = record_commands_to_buffer(ASM_in);
    fclose(ASM_in);

    code_struct code = create_code_array(&text);

    FILE* ASM_out = fopen("C:\\Users\\david\\Desktop\\Projects CLion\\Task4\\ASM\\ASM_out.bin", "wb");
    if (ASM_out == nullptr)
    {
        return -1;
    }
    fwrite(code.pointer, sizeof(int), code.number_of_elements, ASM_out);
    fclose(ASM_out);

    return 0;
}