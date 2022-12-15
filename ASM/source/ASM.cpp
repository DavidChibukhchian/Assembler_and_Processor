#include "ASM.h"

//----------------------------------------------------------------------------------------------------------------------

#define here printf("\nfunc: %s | line: %d)\n", __func__, __LINE__);

//----------------------------------------------------------------------------------------------------------------------

static const char  VERSION = 7;
static const int SIGNATURE = 0x123ABCD;

static const size_t NUMBER_OF_REGISTERS = 4;

//----------------------------------------------------------------------------------------------------------------------

static const unsigned char ARG_NUM = 1 << 7;
static const unsigned char ARG_REG = 1 << 6;
static const unsigned char ARG_RAM = 1 << 5;

//----------------------------------------------------------------------------------------------------------------------

static const size_t MULTIPLIER = 2;
static const size_t START_NUMBER_OF_JUMP_ADDRESSES = 3;
static const int next_case = 1000;

static const size_t SIZE_OF_SIGNATURE = sizeof(int);
static const size_t SIZE_OF_VERSION   = sizeof(char);

//----------------------------------------------------------------------------------------------------------------------

#define DEF_CMD(name, arg_type, CPU_instructions) \
CMD_##name,

enum Commands
{
    #include "cmd.h"
};

#undef DEF_CMD

//----------------------------------------------------------------------------------------------------------------------

enum ARG_type
{
    NO_ARGS,
    NUM_REG_RAM,
    LABEL
};

//----------------------------------------------------------------------------------------------------------------------

static void parse_command(code_struct* code, char* command, char** cmd, char** argument, size_t* length_of_cmd, bool* syntax_error)
{
    *cmd = strtok(command, " ");
    *length_of_cmd = strlen(*cmd);

    *argument = strtok(nullptr, " ");

    if (strtok(nullptr, " ") != nullptr)
    {
        code->err = Too_Many_Arguments_To_Such_Command;
        *syntax_error = true;
    }
}

//----------------------------------------------------------------------------------------------------------------------

static void reset_variables(char* reg, int* value, unsigned char* arg_mask)
{
    *reg = 0;
    *value = 0;
    *arg_mask = 0;
}

//----------------------------------------------------------------------------------------------------------------------

void close_files(files_struct* files)
{
    if (files->ASM_in  != nullptr)
        fclose(files->ASM_in);

    if (files->ASM_out != nullptr)
        fclose(files->ASM_out);
    
    fclose(files->logfile);
}

//----------------------------------------------------------------------------------------------------------------------

int open_logfile(files_struct* files)
{
    FILE* logfile = fopen("ASM_logfile.txt",  "w");
    if (logfile == nullptr)
    {
        printf("---\nERROR: Failed to create logfile\n---");
        return Failed_To_Create_Logfile;
    }
    files->logfile = logfile;

    files->ASM_in  = nullptr;
    files->ASM_out = nullptr;

    return Done_Successfully;
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
        return Failed_To_Create_Output_File;
    }
    files->ASM_out = ASM_out;

    return Done_Successfully;
}

//----------------------------------------------------------------------------------------------------------------------

static void code_Shrink_To_Fit(code_struct* code)
{
    char* new_pointer = (char*)recalloc (code->pointer, code->offset);
    if (new_pointer == nullptr)
    {
        code->err = Failed_To_Resize_Array_Of_Code;
        return;
    }
    code->pointer = new_pointer;
}

//----------------------------------------------------------------------------------------------------------------------

static void code_Expand(code_struct* code)
{
    char* new_pointer = (char*)recalloc (code->pointer, MULTIPLIER * code->size);
    if (new_pointer == nullptr)
    {
        code->err = Failed_To_Resize_Array_Of_Code;
        return;
    }
    code->pointer = new_pointer;

    code->size *= MULTIPLIER;
}

//----------------------------------------------------------------------------------------------------------------------

static void code_Push_Char(code_struct* code, char value)
{
    if (code->offset == code->size)
    {
        code_Expand(code);
        if (code->err)
            return;
    }

    *(code->pointer + code->offset) = value;

    code->offset += sizeof(char);
}

//----------------------------------------------------------------------------------------------------------------------

static void code_Push_Int(code_struct* code, int value)
{
    if ((code->size - code->offset) < sizeof(int))
    {
        code_Expand(code);
        if (code->err)
            return;
    }

    int* ptr = (int*)(code->pointer + code->offset);
    *ptr = value;

    code->offset += sizeof(int);
}

//----------------------------------------------------------------------------------------------------------------------

static void check_number_of_arguments(code_struct* code, char* argument, ARG_type arg_type, bool* syntax_error)
{
    if ((arg_type == NO_ARGS) && (argument != nullptr))
    {
        code->err = Too_Many_Arguments_To_Such_Command;
        *syntax_error = true;
        return;
    }

    if (((arg_type == NUM_REG_RAM) || (arg_type == LABEL)) && (argument == nullptr))
    {
        code->err = Too_Few_Arguments_To_Such_Command;
        *syntax_error = true;
    }
}

//----------------------------------------------------------------------------------------------------------------------

static int check_square_brackets(char* argument, size_t length)
{
    size_t  left_square_brackets = 0;
    size_t right_square_brackets = 0;

    for (size_t i = 0; i < length; i++)
    {
        if (argument[i] == '[')
            left_square_brackets++;

        else if (argument[i] == ']')
            right_square_brackets++;
    }

    if ((left_square_brackets != right_square_brackets) || (left_square_brackets > 1))
        return Incorrect_Command;

    return Done_Successfully;
}

//----------------------------------------------------------------------------------------------------------------------

static int case_NUM(char* argument, unsigned char* arg_mask, int* value)
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
            return Incorrect_Argument;
    }

    return next_case;
}

//----------------------------------------------------------------------------------------------------------------------

static int case_REG(char* argument, unsigned char* arg_mask, char* reg, size_t length)
{
    if ((length == 3) && (argument[0] == 'r') && (argument[2] == 'x'))
    {
        if ((argument[1] - 'a') < NUMBER_OF_REGISTERS)
        {
            *arg_mask = (*arg_mask) | ARG_REG;
            *reg = argument[1] - 'a' + 1;
            return Done_Successfully;
        }
        else
            return Nonexistent_Register;
    }

    return next_case;
}

//----------------------------------------------------------------------------------------------------------------------

static int case_RAM_REG_NUM(char* argument, unsigned char* arg_mask, int* value)
{
    size_t length_of_number = 0;

    if ((argument[4] == '+') && (sscanf((argument + 5), "%d%n", value, &length_of_number)))
    {
        if (*(argument + 5 + length_of_number) == ']')
        {
            *arg_mask = (*arg_mask) | ARG_NUM;
            return Done_Successfully;
        }
    }

    return next_case;
}

//----------------------------------------------------------------------------------------------------------------------

static int case_RAM_REG(char* argument, unsigned char* arg_mask, char* reg, int* value)
{
    if ((argument[1] == 'r') && (argument[3] == 'x'))
    {
        if ((argument[2] - 'a') < NUMBER_OF_REGISTERS)
        {
            *arg_mask = (*arg_mask) | ARG_REG;
            *reg = argument[2] - 'a' + 1;

            int res = case_RAM_REG_NUM(argument, arg_mask, value);
            if (res != next_case)
                return res;
        }
        else
            return Nonexistent_Register;

        if (argument[4] == ']')
            return Done_Successfully;
    }

    return next_case;
}

//----------------------------------------------------------------------------------------------------------------------

static int case_RAM_NUM(char* argument, unsigned char* arg_mask, int* value)
{
    size_t length_of_number = 0;

    if (sscanf(argument + 1, "%d%n", value, &length_of_number))
    {
        if (*(argument + 1 + length_of_number) == ']')
        {
            *arg_mask = (*arg_mask) | ARG_NUM;
            return Done_Successfully;
        }
    }

    return next_case;
}

//----------------------------------------------------------------------------------------------------------------------

static int case_RAM(char* argument, unsigned char* arg_mask, char* reg, int* value, size_t length)
{
    if ((argument[0] == '[') && (argument[length - 1] == ']'))
    {
        *arg_mask = (*arg_mask) | ARG_RAM;

        int res = 0;

        res = case_RAM_REG(argument, arg_mask, reg, value);
        if (res != next_case)
            return res;

        res = case_RAM_NUM(argument, arg_mask, value);
        if (res != next_case)
            return res;
    }

    return next_case;
}

//----------------------------------------------------------------------------------------------------------------------

static int identify_arg(char* argument, unsigned char* arg_mask, char* reg, int* value)
{
    size_t length = strlen(argument);

    if (check_square_brackets(argument, length))
        return Incorrect_Argument;

    int res = 0;

    res = case_NUM(argument, arg_mask, value);
    if (res != next_case)
        return res;

    res = case_REG(argument, arg_mask, reg, length);
    if (res != next_case)
        return res;

    res = case_RAM(argument, arg_mask, reg, value, length);
    if (res != next_case)
        return res;

    return Incorrect_Argument;
}

//----------------------------------------------------------------------------------------------------------------------

static void create_arg_mask(code_struct* code, ARG_type arg_type, char* argument, unsigned char* arg_mask, char* reg, int* value, bool* syntax_error)
{
    if (arg_type == NO_ARGS)
        return;

    if (arg_type == LABEL)
    {
        *arg_mask = (*arg_mask) | ARG_NUM;
        return;
    }

    if (arg_type == NUM_REG_RAM)
    {
        code->err = identify_arg(argument, arg_mask, reg, value);
        if (code->err)
            *syntax_error = true;
    }
}

//----------------------------------------------------------------------------------------------------------------------

static void code_Push_Command(code_struct* code, unsigned char cmd_code, int value, char reg)
{
    code_Push_Char(code, cmd_code);
    if (code->err)
        return;

    if (cmd_code & ARG_NUM)
    {
        code_Push_Int(code, value);
        if (code->err)
            return;
    }

    if (cmd_code & ARG_REG)
    {
        code_Push_Char(code, reg);
    }
}

//----------------------------------------------------------------------------------------------------------------------

static void code_Push_Signature(code_struct* code)
{
    code_Push_Int(code, SIGNATURE);
}

//----------------------------------------------------------------------------------------------------------------------

static void code_Push_Version(code_struct* code)
{
    code_Push_Char(code, VERSION);
}

//----------------------------------------------------------------------------------------------------------------------

static void jumps_Ctor(jumps_struct* jumps)
{
    jumps->jump = (jump_struct*)calloc (START_NUMBER_OF_JUMP_ADDRESSES, sizeof(jump_struct));
    if (jumps->jump == nullptr)
    {
        jumps->err = Failed_To_Create_Array_Of_Jump_Addresses;
        return;
    }

    jumps->number_of_addresses = 0;
    jumps->number_of_free_addresses = START_NUMBER_OF_JUMP_ADDRESSES;
}

//----------------------------------------------------------------------------------------------------------------------

static void jumps_Expand(jumps_struct* jumps)
{
    size_t new_size = MULTIPLIER * jumps->number_of_addresses * sizeof(jump_struct);

    jump_struct* new_pointer = (jump_struct*)recalloc (jumps->jump, new_size);
    if (new_pointer == nullptr)
    {
        jumps->err = Failed_To_Resize_Array_Of_Jump_Addresses;
        return;
    }
    jumps->jump = new_pointer;

    jumps->number_of_free_addresses = jumps->number_of_addresses * (MULTIPLIER - 1);
}

//----------------------------------------------------------------------------------------------------------------------

static void jumps_Put_Code_Address(jumps_struct* jumps, int offset, char* label_name, size_t line)
{
    if (jumps->number_of_free_addresses == 0)
    {
        jumps_Expand(jumps);
        if (jumps->err)
            return;
    }

    size_t idx = jumps->number_of_addresses;

    jumps->jump[idx].code_address = ++offset;
    jumps->jump[idx].line         = ++line;
    jumps->jump[idx].label_name   = ++label_name;

    jumps->number_of_addresses++;
    jumps->number_of_free_addresses--;
}

//----------------------------------------------------------------------------------------------------------------------

static void jumps_Dtor(jumps_struct* jumps)
{
    if (jumps->jump != nullptr)
        free(jumps->jump);
}

//----------------------------------------------------------------------------------------------------------------------

void free_buffer(commands_struct* commands)
{
    free(commands->buffer);
    free(commands->array_of_commands);
}

//----------------------------------------------------------------------------------------------------------------------

static void code_Dtor(code_struct* code)
{
    if (code->pointer != nullptr)
        free(code->pointer);
}

//----------------------------------------------------------------------------------------------------------------------

static void code_Ctor(code_struct* code, commands_struct* commands)
{
    code->err    = 0;
    code->offset = 0;
    code->size   = SIZE_OF_SIGNATURE + SIZE_OF_VERSION + commands->number_of_commands;

    code->pointer = (char*)calloc (code->size, sizeof(char));
    if (code->pointer == nullptr)
    {
        code->err = Failed_To_Create_Array_Of_Code;
        return;
    }

    code_Push_Signature(code);
    if (code->err)
        return;

    code_Push_Version(code);
}

//----------------------------------------------------------------------------------------------------------------------

int record_commands_to_buffer(files_struct* files, commands_struct* commands)
{
    size_t filesize = get_filesize(files->ASM_in);
    if (filesize == 0)
        return Input_File_Is_Empty;

    char* buffer = read_file_to_buffer(files->ASM_in, filesize);
    if (buffer == nullptr)
        return Failed_To_Create_Buffer;

    commands->buffer = buffer;
    commands->number_of_commands = count_lines(buffer);

    char** array_of_commands = split_buffer(buffer, commands->number_of_commands, filesize);
    if (array_of_commands == nullptr)
    {
        free(buffer);
        return Failed_To_Create_Array_Of_Commands;
    }

    commands->array_of_commands = array_of_commands;

    return Done_Successfully;
}

//----------------------------------------------------------------------------------------------------------------------

static int record_commands_to_code(commands_struct* commands, code_struct* code, labels_struct* labels, jumps_struct* jumps, size_t* err_line)
{
    unsigned char cmd_code = 0;
    unsigned char arg_mask = 0;

    int value = 0;
    char reg  = 0;

    char* cmd      = nullptr;
    char* argument = nullptr;
    size_t length_of_cmd = 0;

    bool syntax_error = false;

    size_t i = 0;
    while (i < commands->number_of_commands)
    {
        parse_command(code, commands->array_of_commands[i], &cmd, &argument, &length_of_cmd, &syntax_error);
        VERIFY(code->err);

        if (false) {}

        //------------------------------------------------------------------

        #define DEF_CMD(name, arg_type, CPU_instructions)                                                              \
                                                                                                                       \
        else if (!strcmp(cmd, #name))                                                                                  \
        {                                                                                                              \
            cmd_code = CMD_##name;                                                                                     \
            reset_variables(&reg, &value, &arg_mask);                                                                  \
                                                                                                                       \
            check_number_of_arguments(code, argument, arg_type, &syntax_error);                                        \
            VERIFY(code->err);                                                                                         \
                                                                                                                       \
            create_arg_mask(code, arg_type, argument, &arg_mask, &reg, &value, &syntax_error);                         \
            VERIFY(code->err);                                                                                         \
                                                                                                                       \
            if (arg_type == LABEL)                                                                                     \
            {                                                                                                          \
                labels_Check_Jump(labels, argument, &syntax_error);                                                    \
                VERIFY(labels->err);                                                                                   \
                                                                                                                       \
                jumps_Put_Code_Address(jumps, code->offset, argument, i);                                              \
                VERIFY(jumps->err);                                                                                    \
            }                                                                                                          \
                                                                                                                       \
            cmd_code = cmd_code | arg_mask;                                                                            \
            code_Push_Command(code, cmd_code, value, reg);                                                             \
            VERIFY(code->err);                                                                                         \
        }

        #include "cmd.h"

        #undef DEF_CMD

        //------------------------------------------------------------------

        else if ((cmd[length_of_cmd - 1] == ':') && (argument == nullptr))
        {
            labels_Check_Label(labels, cmd, code->offset + 1, length_of_cmd, &syntax_error);
            VERIFY(labels->err);

            labels_Push(labels, cmd, code->offset + 1);
            VERIFY(labels->err);
        }

        else
        {
            code->err = Incorrect_Command;
            syntax_error = true;
            VERIFY(code->err);
        }

        i++;
    }

    code_Shrink_To_Fit(code);
    VERIFY(code->err);

    labels_Set(labels, jumps, code->pointer, err_line);

    VERIFY(jumps->err);

    return Done_Successfully;
}

//----------------------------------------------------------------------------------------------------------------------

int create_code(code_struct* code, commands_struct* commands, size_t* line)
{
    labels_struct labels = {};
    labels.label = nullptr;

    jumps_struct jumps = {};
    jumps.jump = nullptr;

    code_Ctor(code, commands);
    CHECK_Ctor(code->err);

    jumps_Ctor(&jumps);
    CHECK_Ctor(jumps.err);

    labels_Ctor(&labels);
    CHECK_Ctor(labels.err);

    int err = record_commands_to_code(commands, code, &labels, &jumps, line);
    if (err) return err;

    free_buffer(commands);
    labels_Dtor(&labels);
    jumps_Dtor(&jumps);

    printf("---\nSIZE = %d\n(without signature and version)\n", code->offset - SIZE_OF_SIGNATURE - SIZE_OF_VERSION);
    return Done_Successfully;
}

//----------------------------------------------------------------------------------------------------------------------

int write_code_to_file(code_struct* code, files_struct* files)
{
    size_t written_bytes = 0;
    written_bytes = fwrite(code->pointer, sizeof(char), code->offset, files->ASM_out);

    if (written_bytes != code->offset)
    {
        code_Dtor(code);
        return Failed_To_Write_Code_To_File;
    }

//    code_Dtor(code);

    return Done_Successfully;
}

//----------------------------------------------------------------------------------------------------------------------