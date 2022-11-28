#include "ASM.h"
#include <string.h>
#include <cstring>

int main(int argc, char* argv[])
{


    char str[] ="push [rax+10] 4";
    char* pointer[10] = {nullptr};

    pointer[0] = strtok(str, " ");

    size_t i = 0;

    while (pointer[i] != nullptr)
    {
        pointer[i + 1] = strtok(nullptr, " ");
        i++;
    }

    for (size_t j = 0; j < i; j++)
    {
//        printf("%s\n", pointer[j]);
    }



    files_struct files = {};
    open_logfile(&files);
    CHECK_LOGFILE(files);

    ASSERT(argc == 2, Incorrect_Number_Of_CMD_Arguments);

    int err = 0;

    err = open_files(&files, argv);
    VERIFY(err);

    commands_struct commands = {};
    err = record_commands_to_buffer(files.ASM_in, &commands);
    VERIFY(err);

    code_struct code = {};
    err = create_code_array(&code, &commands);
    VERIFY(err);

    err = write_code_to_file(&code, files.ASM_out);
    VERIFY(err);

    close_files(&files);

    printf("---\nDone successfully\n---");
    return 0;
}