#include <stdio.h>
#include <string.h>
#include "Buffer.h"
#include "ASM.h"

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

program_text read_input_file(FILE* ASM_in)
{
    program_text Program = {};

    size_t filesize = get_file_size(ASM_in);

    char* buffer = read_file_to_buffer(ASM_in, filesize);

    Program.number_of_commands = count_lines(buffer);

    change_delimiter(buffer, '\n', '\0');

    char** text = split_buffer(buffer, Program.number_of_commands, filesize);
    Program.text = text;

    return Program;
}

//----------------------------------------------------------------------------------------------------------------------

void record_code_to_file(FILE* ASM_out, void* Program_struct)
{
    program_text* Program = (program_text*) Program_struct;
    fprintf(ASM_out, "%zu", Program->number_of_commands);

    size_t number_of_lines = Program->number_of_commands;
    char cmd[100] = ""; // !
    for (size_t i = 0; i < Program->number_of_commands; i++)
    {
        sscanf(Program->text[i], "%s", cmd);

        if (strcmp(cmd, "PUSH") == 0)
        {
            int value = 0;
            sscanf(Program->text[i] + 5, "%d", &value);
            fprintf(ASM_out, "\n%d\n%d", CMD_PUSH, value);
            number_of_lines++;
        }
        else if (strcmp(cmd, "ADD") == 0)
        {
            fprintf(ASM_out, "\n%d", CMD_ADD);
        }
        else if (strcmp(cmd, "SUB") == 0)
        {
            fprintf(ASM_out, "\n%d", CMD_SUB);
        }
        else if (strcmp(cmd, "MUL") == 0)
        {
            fprintf(ASM_out, "\n%d", CMD_MUL);
        }
        else if (strcmp(cmd, "DIV") == 0)
        {
            fprintf(ASM_out, "\n%d", CMD_DIV);
        }
        else if (strcmp(cmd, "OUT") == 0)
        {
            fprintf(ASM_out, "\n%d", CMD_OUT);
        }
        else if (strcmp(cmd, "HLT") == 0)
        {
            fprintf(ASM_out, "\n%d", CMD_HLT);
        }
        else if (strcmp(cmd, "DUMP") == 0)
        {
            fprintf(ASM_out, "\n%d", CMD_DUMP);
        }
        else
        {
            printf("\nSyntax error\n");
            fclose(ASM_out);
        }
    }

    fseek(ASM_out, 0, SEEK_SET);
    fprintf(ASM_out, "%zu", number_of_lines);
}