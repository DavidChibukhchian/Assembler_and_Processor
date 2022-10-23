#ifndef ASM_ASM_H
#define ASM_ASM_H

struct text_struct
{
    char** array_of_commands;
    size_t number_of_commands;
};

text_struct record_commands_to_buffer(FILE* ASM_in);

struct code_struct
{
    int* pointer;
    size_t number_of_elements;
};

code_struct create_code_array(void* text_of_commands);

#endif //ASM_ASM_H