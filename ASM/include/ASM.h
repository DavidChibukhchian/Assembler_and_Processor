#ifndef ASM_ASM_H
#define ASM_ASM_H

struct program_text
{
    char** text;
    size_t number_of_commands;
};

program_text read_input_file(FILE* ASM_in);

void record_code_to_file(FILE* ASM_out, void* Program);

#endif //ASM_ASM_H
