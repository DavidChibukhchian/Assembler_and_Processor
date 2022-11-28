#include "ASM.h"

static const unsigned char ARG_num = 1 << 7;
static const unsigned char ARG_reg = 1 << 6;
static const unsigned char ARG_ram = 1 << 5;

//----------------------------------------------------------------------------------------------------------------------

static const float MULTIPLIER = 1.5;
static const size_t BUFF_SIZE = 35;

//----------------------------------------------------------------------------------------------------------------------

static const int SIGNATURE = 0x123ABCD;
static const char  VERSION = 7;

static const size_t SIZE_OF_SIGNATURE = sizeof(int);
static const size_t SIZE_OF_VERSION   = sizeof(char);

//----------------------------------------------------------------------------------------------------------------------

void close_files(files_struct* files)
{
    fclose(files->ASM_in);
    fclose(files->ASM_out);
    fclose(files->logfile);
}

//----------------------------------------------------------------------------------------------------------------------

void open_logfile(files_struct* files)
{
    FILE* logfile = fopen("ASM_logfile.txt",  "w");
    files->logfile = logfile;
}

//----------------------------------------------------------------------------------------------------------------------

int open_files(files_struct* files, char** argv)
{
    FILE* ASM_in = fopen(argv[1], "r");
    if (ASM_in == nullptr)
    {
        return Failed_To_Open_Input_File;
    }
    files->ASM_in = ASM_in;

    FILE* ASM_out = fopen("ASM_out.bin", "wb");
    if (ASM_out == nullptr)
    {
        fclose(files->ASM_in);
        return Failed_To_Create_Output_File;
    }
    files->ASM_out = ASM_out;

    return Done_Successfully;
}

//----------------------------------------------------------------------------------------------------------------------


#define DEF_CMD(name, arg_type, cpu) \
CMD_##name,

enum Commands
{
    #include "cmd.h"
};

#undef DEF_CMD

//----------------------------------------------------------------------------------------------------------------------

enum ARG_type
{
    NO_ARGS = 0,
    ARGS    = 1
};

//----------------------------------------------------------------------------------------------------------------------

static void codeShrinkToFit(code_struct* code)
{
    char* new_pointer = (char*)realloc (code->pointer, code->offset);
    if (new_pointer == nullptr)
    {
        free(code->pointer);
        code->err = Failed_To_Create_Array_Of_Code;
    }
}

//----------------------------------------------------------------------------------------------------------------------

static void codeExpand(code_struct* code)
{
    char* new_pointer = nullptr;
    if (code->size > 1)
    {
        new_pointer =(char*)realloc (code->pointer, code->size * MULTIPLIER);
        code->size *= MULTIPLIER;
    }
    else
    {
        new_pointer = (char*)realloc (code->pointer, code->size + sizeof(int));
        code->size += sizeof(int);
    }

    if (new_pointer == nullptr)
    {
        code->err = Failed_To_Resize_Array_Of_Code;
        return;
    }

    if (code->pointer != new_pointer)
        free(code->pointer);

    code->pointer = new_pointer;
}

//----------------------------------------------------------------------------------------------------------------------

static void codePush_Char(code_struct* code, char value)
{
    if (code->offset == code->size)
    {
        codeExpand(code);
        if (code->err)
        {
            free(code->pointer);
            return;
        }
    }

    code->pointer[code->offset] = value;

    code->offset += sizeof(char);
}

//----------------------------------------------------------------------------------------------------------------------

static void codePush_Int(code_struct* code, int value)
{
    if ((code->size - code->offset) < sizeof(int))
    {
        codeExpand(code);
        if (code->err)
        {
            free(code->pointer);
            return;
        }
    }

    int* ptr = (int*)(code->pointer + code->offset);
    *ptr = value;

    code->offset += sizeof(int);
}

//----------------------------------------------------------------------------------------------------------------------

static int check_no_args(commands_struct* commands, size_t i, size_t cmd_name)
{
    int  test_value = 0;
    char test_str[BUFF_SIZE] = "";

    size_t test_scanned_symbols_1 = 0;
    size_t test_scanned_symbols_2 = 0;

    sscanf(commands->array_of_commands[i] + cmd_name, "%d%n", &test_value, &test_scanned_symbols_1);
    sscanf(commands->array_of_commands[i] + cmd_name, "%s%n", test_str,    &test_scanned_symbols_2);

    if (test_scanned_symbols_1 | test_scanned_symbols_2)
    {
        return Invalid_Syntax;
    }

    return Done_Successfully;
}

//----------------------------------------------------------------------------------------------------------------------

static int identify_args(unsigned char* arg_mask, char* cmd, commands_struct* commands, size_t i, size_t cmd_name, int* reg, int* value)
{
    size_t counter = 0;

    if (!check_no_args(commands, i, cmd_name)) // todo naming
    {
        return Invalid_Syntax;
    }
    cmd_name++;

    if (sscanf(commands->array_of_commands[i] + cmd_name, "%d%n", value, &counter))
    {
        if (check_no_args(commands, i, cmd_name + counter))
        {
            return Invalid_Syntax;
        }

        *arg_mask = (*arg_mask) | ARG_num;
        return Done_Successfully;
    }

    if (sscanf(commands->array_of_commands[i] + cmd_name, "%s%n", cmd, &counter))
    {
        if (check_no_args(commands, i, cmd_name + counter))
        {
            return Invalid_Syntax;
        }

        if ((counter == 3) && (cmd[0] == 'r') && (cmd[counter - 1] == 'x'))
        {
            if ((cmd[1] - 'a') > ('d' - 'a'))
            {
                return Nonexistent_Register;
            }

            *arg_mask = (*arg_mask) | ARG_reg;
            *reg = cmd[1] - 'a' + 1;
            return Done_Successfully;
        }

        else if ((cmd[0] == '[') && (cmd[counter - 1] == ']'))
        {
            if ((cmd[1] == 'r') && (cmd[3] == 'x'))
            {
                if ((cmd[2] - 'a') > ('d' - 'a'))
                {
                    return Nonexistent_Register;
                }
                *arg_mask = (*arg_mask) | ARG_reg;
                *reg = cmd[1] - 'a' + 1;

                *arg_mask = (*arg_mask) | ARG_ram;

                if ((cmd[4] == '+') && (sscanf(commands->array_of_commands[i] + cmd_name + 1 + 3 + 1, "%d", value)))
                {
                    *arg_mask = (*arg_mask) | ARG_num;
                    return Done_Successfully;
                }
                if ((cmd[4] == '+') && (!sscanf(commands->array_of_commands[i] + cmd_name + 1 + 3 + 1, "%d", value)))
                {
                    return Invalid_Syntax;
                }
                if ((cmd[4] != '+') && (sscanf(commands->array_of_commands[i] + cmd_name + 1 + 3, "%d", value)))
                {
                    return Invalid_Syntax;
                }
                if (!sscanf(commands->array_of_commands[i] + cmd_name + 1 + 3 + 1, "%d", value))
                {
                    return Invalid_Syntax;
                }

                *arg_mask | ARG_num;
                return Done_Successfully;
            }

            if (sscanf(commands->array_of_commands[i] + cmd_name + 1, "%d", value))
            {
                *arg_mask = (*arg_mask) | ARG_ram;
                *arg_mask = (*arg_mask) | ARG_num;
                return Done_Successfully;
            }
            else
            {
                return Invalid_Syntax;
            }
        }
        else
        {
            return Invalid_Syntax;
        }
    }
    else
    {
        return Invalid_Syntax;
    }
}

//----------------------------------------------------------------------------------------------------------------------

static int make_arg_mask(unsigned char* arg_mask, char* cmd, commands_struct* commands, size_t i, size_t cmd_name, ARG_type arg_type, int* number_of_register, int* value)
{
    int err = 0;

    if (arg_type == NO_ARGS)
    {
        err = check_no_args(commands, i, cmd_name);
    }
    else
    {
        err = identify_args(arg_mask, cmd, commands, i, cmd_name, number_of_register, value);
    }

    if (err)
    {
        return err;
    }

    return Done_Successfully;
}


//----------------------------------------------------------------------------------------------------------------------

static void codePush_Command(code_struct* code, unsigned char cmd_code, int reg, int value)
{
    codePush_Char(code, cmd_code);

    if (cmd_code & ARG_reg)
    {
        codePush_Char(code, reg);
    }

    if (cmd_code & ARG_num)
    {
        codePush_Int(code, value);
    }
}

//----------------------------------------------------------------------------------------------------------------------

static void codePush_Signature(code_struct* code)
{
    codePush_Int(code, SIGNATURE);
}

//----------------------------------------------------------------------------------------------------------------------

static void codePush_Version(code_struct* code)
{
    codePush_Char(code, VERSION);
}

//----------------------------------------------------------------------------------------------------------------------

static void codeCtor(code_struct* code, size_t number_of_commands)
{
    code->err    = 0;
    code->offset = 0;
    code->size   = SIZE_OF_SIGNATURE + SIZE_OF_VERSION + number_of_commands;

    code->pointer = (char*)calloc (code->size, sizeof(char));
    if (code->pointer == nullptr)
    {
        code->err = Failed_To_Create_Array_Of_Code;
        return;
    }

    codePush_Signature(code);
    codePush_Version(code);
}

//----------------------------------------------------------------------------------------------------------------------

void free_buffer(commands_struct* commands)
{
    free(commands->buffer);
    free(commands->array_of_commands);
}

//----------------------------------------------------------------------------------------------------------------------

int record_commands_to_buffer(FILE* ASM_in, commands_struct* commands)
{
    size_t filesize = get_filesize(ASM_in);
    if (filesize == 0)
    {
        fclose(ASM_in);
        return Input_File_Is_Empty;
    }

    char* buffer = read_file_to_buffer(ASM_in, filesize);
    if (buffer == nullptr)
    {
        fclose(ASM_in);
        return Failed_To_Create_Buffer;
    }
    commands->buffer = buffer;

    commands->number_of_commands = count_lines(buffer);
   
    char** array_of_commands = split_buffer(buffer, commands->number_of_commands, filesize);
    if (array_of_commands == nullptr)
    {
        free(buffer);
        fclose(ASM_in);
        return Failed_To_Create_Array_Of_Commands;
    }

    commands->array_of_commands = array_of_commands;

    fclose(ASM_in);
    return Done_Successfully;
}

//----------------------------------------------------------------------------------------------------------------------

int create_code_array(code_struct* code, commands_struct* commands)
{
    unsigned char arg_mask = 0;
    unsigned char cmd_code = 0;

    bool HLT_was_set = false;

    codeCtor(code, commands->number_of_commands);
    VERIFY_CODE_ERR;

    char cmd[BUFF_SIZE] = "";
    size_t scanned_name = 0;

    int number_of_register = 0;
    int value = 0;



    for (size_t i = 0; i < commands->number_of_commands; i++)
    {
        sscanf(commands->array_of_commands[i], "%s%n", cmd, &scanned_name);

        if (false) {}

        //------------------------------------------------------------------
        #define DEF_CMD(name, arg_type, cpu)                                                                            \
        else if (!strcmp(cmd, #name))                                                                                   \
        {                                                                                                               \
            if (!strcmp("hlt", #name))                                                                                  \
                HLT_was_set = true;                                                                                     \
                                                                                                                        \
            arg_mask = 0;                                                                                               \
            code->err = make_arg_mask(&arg_mask, cmd, commands, i, scanned_name, arg_type, &number_of_register, &value);\
            VERIFY_CODE_ERR;                                                                                            \
                                                                                                                        \
            cmd_code = 0;                                                                                               \
            cmd_code = (CMD_##name) | arg_mask;                                                                         \
                                                                                                                        \
            codePush_Command(code, cmd_code, number_of_register, value);                                                \
            VERIFY_CODE_ERR;                                                                                            \
        }

        #include "cmd.h"
        #undef DEF_CMD
        //------------------------------------------------------------------

        else
        {
            free_buffer(commands);
            if (code->pointer != nullptr)
                free(code->pointer);
            return Invalid_Syntax;
        }
    }

    if (!(HLT_was_set))
    {
        return Missed_HLT_Command;
    }

    codeShrinkToFit(code);
    VERIFY_CODE_ERR;

    printf("---\nSIZE = %zu\n(without signature and version)\n", code->offset - SIZE_OF_SIGNATURE - SIZE_OF_VERSION);

    free_buffer(commands);

    return Done_Successfully;
}

//----------------------------------------------------------------------------------------------------------------------

int write_code_to_file(code_struct* code, FILE* ASM_out)
{
    size_t written_bytes = 0;
    written_bytes = fwrite(code->pointer, sizeof(char), code->offset, ASM_out);

    if (written_bytes != code->offset)
    {
        fclose(ASM_out);
        free(code->pointer);
        return Failed_To_Write_Code_To_File;
    }

    free(code->pointer);

    return Done_Successfully;
}

//----------------------------------------------------------------------------------------------------------------------