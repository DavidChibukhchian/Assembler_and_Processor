#include <stdio.h>
#include <malloc.h>
#include "Buffer.h"
#include "ASM.h"

int main(int argc, char* argv[])
{
    int err = 0;
    FILE* logfile = fopen("ASM_logfile.txt",  "w");
    CHECK_LOGFILE(logfile);

    ASSERT(argc == 2, Incorrect_Number_Of_CMD_Arguments,{});

    FILE* ASM_in = fopen(argv[1], "r");
    ASSERT(ASM_in != nullptr, Failed_To_Open_Input_File,{});

    commands_struct commands = {};
    err = record_commands_to_buffer(ASM_in, &commands);
    VERIFY(err);

    code_struct code = {};
    err = create_code_array(&commands, &code);
    VERIFY(err);

    FILE* ASM_out = fopen("ASM_out.bin", "wb");
    ASSERT(ASM_out != nullptr, Failed_To_Create_Output_File, (fclose(ASM_in), free(code.pointer)));

    err = write_code_to_file(&code, ASM_out);
    VERIFY(err);

    fclose(logfile);

    printf("---\nDone successfully\n---");
    return 0;
}