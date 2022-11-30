#include "ASM.h"

static const unsigned char ARG_NUM = 1 << 7;
static const unsigned char ARG_REG = 1 << 6;
static const unsigned char ARG_RAM = 1 << 5;

//----------------------------------------------------------------------------------------------------------------------

static const float MULTIPLIER = 1.5;
static const size_t BUFF_SIZE = 35;
static const int CONTINUE = 100;

//----------------------------------------------------------------------------------------------------------------------

static const int SIGNATURE = 0x123ABCD;
static const char  VERSION = 7;
static const size_t NUMBER_OF_REGISTERS = 4;

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
    ONE_ARG = 1
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

static int check_number_of_arguments(char* argument, ARG_type arg_type)
{
    if ((arg_type == NO_ARGS) && (argument != nullptr))
    {
        return Invalid_Syntax;
    }

    if ((arg_type == ONE_ARG) && (argument == nullptr))
    {
        return Invalid_Syntax;
    }

    return Done_Successfully;
}

//----------------------------------------------------------------------------------------------------------------------

static int check_square_brackets(char* argument, size_t length)
{
    size_t left__square_bracket = 0;
    size_t right_square_bracket = 0;

    for (size_t i = 0; i < length; i++)
    {
        if (argument[i] == '[')
        {
            left__square_bracket++;
        }
        else if (argument[i] == ']')
        {
            right_square_bracket++;
        }
    }

    if ((left__square_bracket != right_square_bracket) || (left__square_bracket > 1))
    {
        return Invalid_Syntax;
    }

    return Done_Successfully;
}

//----------------------------------------------------------------------------------------------------------------------

static int case_Number(char* argument, unsigned char* arg_mask, int* value)
{
    size_t length_of_number = 0;

    if (sscanf(argument, "%d%n", value, &length_of_number))
    {
        if (*(argument + length_of_number) == '\0')
        {
            *arg_mask = (*arg_mask) | ARG_NUM;
            return Done_Successfully;
        }
        else
            return Invalid_Syntax;
    }

    return CONTINUE;
}

//----------------------------------------------------------------------------------------------------------------------

static int case_Register(char* argument, unsigned char* arg_mask, size_t* number_of_register, size_t length)
{
    if ((length == 3) && (argument[0] == 'r') && (argument[2] == 'x'))
    {
        if ((argument[1] - 'a') < NUMBER_OF_REGISTERS)
        {
            *arg_mask = (*arg_mask) | ARG_REG;
            *number_of_register = argument[1] - 'a' + 1;
            return Done_Successfully;
        }
        else
            return Nonexistent_Register;
    }

    return CONTINUE;
}

//----------------------------------------------------------------------------------------------------------------------

static int case_RAM(char* argument, unsigned char* arg_mask, size_t* number_of_register, int* value, size_t length)
{
    size_t length_of_number = 0;

    if ((argument[0] == '[') && (argument[length - 1] == ']'))
    {
        *arg_mask = (*arg_mask) | ARG_RAM;

        if ((argument[1] == 'r') && (argument[3] == 'x'))
        {
            if ((argument[2] - 'a') < NUMBER_OF_REGISTERS)
            {
                *arg_mask = (*arg_mask) | ARG_REG;
                if ((argument[4] == '+') && (sscanf((argument + 5), "%d%n", value, &length_of_number)))
                {
                    if (*(argument + 5 + length_of_number) == ']')
                    {
                        *arg_mask = (*arg_mask) | ARG_NUM;
                        return Done_Successfully;
                    }
                }
            }
            else
                return Nonexistent_Register;

            if (argument[4] == ']')
                return Done_Successfully;
        }

        if (sscanf(argument + 1, "%d%n", value, &length_of_number))
        {
            if (*(argument + 1 + length_of_number) == ']')
            {
                *arg_mask = (*arg_mask) | ARG_NUM;
                return Done_Successfully;
            }
        }
    }

    return CONTINUE;
}

//----------------------------------------------------------------------------------------------------------------------

static int create_arg_mask(char* argument, unsigned char* arg_mask, size_t* number_of_register, int* value)
{
    size_t length = strlen(argument);

    if (check_square_brackets(argument, length))
        return Invalid_Syntax;

    int res = 0;

    res = case_Number(argument, arg_mask, value);
    if (res != CONTINUE)
        return res;

   res = case_Register(argument, arg_mask, number_of_register, length);
    if (res != CONTINUE)
        return res;

   res = case_RAM(argument, arg_mask, number_of_register, value, length);
    if (res != CONTINUE)
        return res;

    return Invalid_Syntax;
}

//----------------------------------------------------------------------------------------------------------------------

static void codePush_Command(code_struct* code, unsigned char cmd_code, int reg, int value)
{
    codePush_Char(code, cmd_code);

    if (cmd_code & ARG_REG)
    {
        codePush_Char(code, reg);
    }

    if (cmd_code & ARG_NUM)
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
    unsigned char cmd_code = 0;
    unsigned char arg_mask = 0;

    size_t i = 0;
    bool HLT_was_set = false;

    codeCtor(code, commands->number_of_commands);
    VERIFY_CODE_ERR;

    char* cmd = nullptr;
    char* argument = nullptr;

    size_t number_of_register = 0;
    int value = 0;

    for (; i < commands->number_of_commands; i++)
    {
        cmd = strtok(commands->array_of_commands[i], " ");
        argument = strtok(nullptr, " ");
        if (strtok(nullptr, " ") != nullptr)
        {
            code->err = Invalid_Syntax;
            VERIFY_CODE_ERR;
        }

        if (false) {}

        //------------------------------------------------------------------
        #define DEF_CMD(name, arg_type, cpu)                                                                            \
        else if (strcmp(cmd, #name) == 0)                                                                               \
        {                                                                                                               \
            code->err = check_number_of_arguments(argument, arg_type);                                                  \
            VERIFY_CODE_ERR;                                                                                            \
                                                                                                                        \
            cmd_code = 0;                                                                                               \
            arg_mask = 0;                                                                                               \
                                                                                                                        \
            if (arg_type == ONE_ARG)                                                                                    \
            {                                                                                                           \
                code->err = create_arg_mask(argument, &arg_mask, &number_of_register, &value);                          \
                VERIFY_CODE_ERR;                                                                                        \
                cmd_code = (CMD_##name) | arg_mask;                                                                     \
            }                                                                                                           \
                                                                                                                        \
            codePush_Command(code, cmd_code, number_of_register, value);                                                \
            VERIFY_CODE_ERR;                                                                                            \
                                                                                                                        \
            if (!strcmp("hlt", #name))                                                                                  \
                HLT_was_set = true;                                                                                     \
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
        free_buffer(commands);
        free(code->pointer);
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