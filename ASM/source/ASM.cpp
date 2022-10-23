#include <stdio.h>
#include <string.h>
#include <malloc.h>
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

text_struct record_commands_to_buffer(FILE* ASM_in)
{
    size_t filesize = get_file_size(ASM_in);

    char* buffer = read_file_to_buffer(ASM_in, filesize);

    text_struct text = {};

    text.number_of_commands = count_lines(buffer);

    change_delimiter(buffer, '\n', '\0');

    char** array_of_commands = split_buffer(buffer, text.number_of_commands, filesize);
    text.array_of_commands = array_of_commands;

    return text;
}

//----------------------------------------------------------------------------------------------------------------------

code_struct create_code_array(void* text_of_commands)
{
    text_struct* text = (text_struct*) text_of_commands;

    int* data = (int*)calloc (2 * text->number_of_commands, sizeof(int));

    size_t counter = 0;

    char cmd[100] = "";
    for (size_t i = 0; i < text->number_of_commands; i++)
    {
        sscanf(text->array_of_commands[i], "%s", cmd);

        if (strcmp(cmd, "PUSH") == 0)
        {
            int value = 0;
            sscanf(text->array_of_commands[i] + 5, "%d", &value);
            data[counter] = CMD_PUSH;
            data[counter + 1] = value;
            counter += 2;
        }
        else if (strcmp(cmd, "ADD") == 0)
        {
            data[counter] = CMD_ADD;
            counter++;
        }
        else if (strcmp(cmd, "SUB") == 0)
        {
            data[counter] = CMD_SUB;
            counter++;
        }
        else if (strcmp(cmd, "MUL") == 0)
        {
            data[counter] = CMD_MUL;
            counter++;
        }
        else if (strcmp(cmd, "DIV") == 0)
        {
            data[counter] = CMD_DIV;
            counter++;
        }
        else if (strcmp(cmd, "OUT") == 0)
        {
            data[counter] = CMD_OUT;
            counter++;
        }
        else if (strcmp(cmd, "HLT") == 0)
        {
            data[counter] = CMD_HLT;
            counter++;
        }
        else if (strcmp(cmd, "DUMP") == 0)
        {
            data[counter] = CMD_DUMP;
            counter++;
        }
        else
        {
            printf("\nSyntax error\n");
        }
    }

    code_struct code = {};
    code.number_of_elements = counter;

    int* code_pointer = (int*)realloc (data, (counter - 1) * sizeof(int));
    code.pointer = code_pointer;

    return code;
}

//----------------------------------------------------------------------------------------------------------------------
