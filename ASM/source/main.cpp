#include "ASM.h"

int main(int argc, char* argv[])
{
    int err = 0;
    size_t err_line = 0;

    files_struct files = {};
    err = open_logfile(&files);
    if (err) return err;

    ASSERT(argc == 2, Incorrect_Number_Of_CMD_Arguments);

    err = open_files(&files, argv);
    VERIFY_err;

    commands_struct commands = {};
    err = record_commands_to_buffer(&files, &commands);
    VERIFY_err;

    code_struct code = {};
    err = create_code(&code, &commands, &err_line);
    VERIFY_err;

    err = write_code_to_file(&code, &files);
    VERIFY_err;

    close_files(&files);
    
    return 0;
}
