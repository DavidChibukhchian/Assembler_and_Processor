#include "ASM.h"

int main(int argc, char* argv[])
{
    files_struct files = {};
    open_logfile(&files);
    CHECK_LOGFILE(files);

    ASSERT(argc == 2, Incorrect_Number_Of_CMD_Arguments);

    int err = open_files(&files, argv);
    VERIFY(err);

    commands_struct commands = {};
    err = record_commands_to_buffer(files.ASM_in, &commands);
    VERIFY(err);

    code_struct code = {};
    err = create_code_array(&code, &commands);
    VERIFY(err)
    printf("%d", code.jumps.addresses[0]);
    err = write_code_to_file(&code, files.ASM_out);
    VERIFY(err);

    close_files(&files);

    printf("---\nDone successfully\n---");
    return 0;
}