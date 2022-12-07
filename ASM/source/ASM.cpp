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
static const int next_case = 1000;
static const size_t START_NUMBER_OF_JUMP_ADDRESSES = 3;

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
    NO_ARGS = 0,
    ONE_ARG = 1,
    LABEL   = 2
};

//----------------------------------------------------------------------------------------------------------------------

static void reset_variables(unsigned char* arg_mask, int* value, char* number_of_register)
{
    *arg_mask = 0;
    *value    = 0;
    *number_of_register = 0;
}

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

static void code_Shrink_To_Fit(code_struct* code)
{
    char* new_pointer = (char*)realloc (code->pointer, code->offset);
    if (new_pointer == nullptr)
    {
        code->err = Failed_To_Resize_Array_Of_Code;
        return;
    }

    code->pointer = new_pointer;

    // Возвращается  указатель на переразмещенный  блок памяти.
    // Если нет подходящей памяти, возвращается NULL (но память по ptr не освобождается).
}

//----------------------------------------------------------------------------------------------------------------------

static void code_Expand(code_struct* code)
{
    char* new_pointer = (char*)realloc (code->pointer, code->size * MULTIPLIER);
    code->size *= MULTIPLIER;

    if (new_pointer == nullptr)
    {
        code->err = Failed_To_Resize_Array_Of_Code;
        return;
    }

    code->pointer = new_pointer;
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

static int check_number_of_arguments(char* argument, ARG_type arg_type)
{
    if ((arg_type == NO_ARGS) && (argument != nullptr))
    {
        here
        return Too_Many_Arguments_To_Such_Command;
    }

    if (((arg_type == ONE_ARG) || (arg_type == LABEL)) && (argument == nullptr))
        return Too_Few_Arguments_To_Such_Command;

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
        return Incorrect_Command;
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
            return Incorrect_Argument;
    }

    return next_case;
}

//----------------------------------------------------------------------------------------------------------------------

static int case_Register(char* argument, unsigned char* arg_mask, char* number_of_register, size_t length)
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

    return next_case;
}

//----------------------------------------------------------------------------------------------------------------------

static int case_RAM(char* argument, unsigned char* arg_mask, char* number_of_register, int* value, size_t length)
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
                *number_of_register = argument[2] - 'a' + 1;

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

    return next_case;
}

//----------------------------------------------------------------------------------------------------------------------

static int create_arg_mask(char* argument, unsigned char* arg_mask, char* number_of_register, int* value)
{
    size_t length = strlen(argument);

    if (check_square_brackets(argument, length))
        return Incorrect_Command;

    int res = 0;

    res = case_Number(argument, arg_mask, value);
    if (res != next_case)
        return res;

   res = case_Register(argument, arg_mask, number_of_register, length);
   if (res != next_case)
       return res;

   res = case_RAM(argument, arg_mask, number_of_register, value, length);
   if (res != next_case)
       return res;

   return Incorrect_Command;
}

//----------------------------------------------------------------------------------------------------------------------

static void code_Push_Command(code_struct* code, unsigned char cmd_code, int value, char reg)
{
    code_Push_Char(code, cmd_code);
    if (code->err)
        return;

    if (cmd_code & ARG_REG)
    {
        code_Push_Char(code, reg);
        if (code->err)
            return;
    }

    if (cmd_code & ARG_NUM)
    {
        code_Push_Int(code, value);
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

static void jumps_Ctor(code_struct* code)
{
    code->jumps.addresses = (int*)calloc (START_NUMBER_OF_JUMP_ADDRESSES, sizeof(int));
    if (code->jumps.addresses == nullptr)
    {
        code->err = Failed_To_Create_Array_Of_Jump_Addresses;
        return;
    }

    code->jumps.number_of_addresses = 0;
    code->jumps.number_of_free_addresses = START_NUMBER_OF_JUMP_ADDRESSES;
}

//----------------------------------------------------------------------------------------------------------------------

static void jumps_Expand(code_struct* code)
{
    size_t new_size = code->jumps.number_of_addresses * MULTIPLIER;

    int* new_pointer = (int*)realloc (code->jumps.addresses, new_size * sizeof(int));
    if (new_pointer == nullptr)
    {
        code->err = Failed_To_Resize_Array_Of_Jump_Addresses;
        return;
    }
    code->jumps.addresses = new_pointer;

    code->jumps.number_of_free_addresses = code->jumps.number_of_addresses * (MULTIPLIER - 1);
}

//----------------------------------------------------------------------------------------------------------------------

static void jumps_Put_Address(code_struct* code, int value)
{
    if (code->jumps.number_of_free_addresses == 0)
    {
        jumps_Expand(code);
        if (code->err)
            return;
    }

    size_t idx = code->jumps.number_of_addresses;
    code->jumps.addresses[idx] = value;

    code->jumps.number_of_addresses++;
    code->jumps.number_of_free_addresses--;
}

//----------------------------------------------------------------------------------------------------------------------

static void jumps_Dtor(code_struct* code)
{
    if (code->jumps.addresses != nullptr)
    {
        free(code->jumps.addresses);
        code->jumps.addresses = nullptr;
    }
}

//----------------------------------------------------------------------------------------------------------------------

static void code_Ctor(code_struct* code, size_t number_of_commands)
{
    code->err    = 0;
    code->offset = 0;
    code->HLT_was_set = false;
    code->size = SIZE_OF_SIGNATURE + SIZE_OF_VERSION + number_of_commands;
    code->jumps.addresses = nullptr;

    code->pointer = (char*)calloc (code->size, sizeof(char));
    if (code->pointer == nullptr)
    {
        code->err = Failed_To_Create_Array_Of_Code;
        return;
    }

    jumps_Ctor(code);
    if (code->err)
        return;

    code_Push_Signature(code);
    if (code->err)
        return;

    code_Push_Version(code);
}

//----------------------------------------------------------------------------------------------------------------------

static void code_Dtor(code_struct* code)
{
    if (code->pointer != nullptr)
    {
        free(code->pointer);
        code->pointer = nullptr;
    }
    jumps_Dtor(code);
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
    code_Ctor(code, commands->number_of_commands);
    if (code->err)
    {
        code_Dtor(code);
        free_buffer(commands);
        return code->err;
    }

    labels_struct labels = {};
    labels_Ctor(&labels);
    VERIFY_LABELS_ERR;

    unsigned char cmd_code = 0;
    unsigned char arg_mask = 0;

    int value = 0;
    char number_of_register = 0;

    char* cmd      = nullptr;
    char* argument = nullptr;
    size_t length_of_cmd = 0;

    for (size_t i = 0; i < commands->number_of_commands; i++)
    {
        cmd = strtok(commands->array_of_commands[i], " ");    // function
        length_of_cmd = strlen(cmd);
        argument = strtok(nullptr, " ");

        if (strtok(nullptr, " ") != nullptr)
        {
            code->err = Too_Many_Arguments_To_Such_Command;
            VERIFY_CODE_ERR;
        }

        if (false) {}

        //------------------------------------------------------------------

        #define DEF_CMD(name, arg_type, CPU_instructions)                                                              \
        else if (!strcmp(cmd, #name))                                                                                  \
        {                                                                                                              \
            reset_variables(&arg_mask, &value, &number_of_register);                                                   \
            cmd_code = CMD_##name;                                                                                     \
                                                                                                                       \
            code->err = check_number_of_arguments(argument, arg_type);                                                 \
            VERIFY_CODE_ERR;                                                                                           \
                                                                                                                       \
            if (arg_type == ONE_ARG)                                                                                   \
            {                                                                                                          \
                code->err = create_arg_mask(argument, &arg_mask, &number_of_register, &value);                         \
            }                                                                                                          \
            else if (arg_type == LABEL)                                                                                \
            {                                                                                                          \
                code->err = labels_Check_Jump(&labels, argument);                                                      \
                VERIFY_CODE_ERR;                                                                                       \
                                                                                                                       \
                arg_mask = ARG_NUM;                                                                                    \
                jumps_Put_Address(code, code->offset + 1);                                                             \
            }                                                                                                          \
            VERIFY_CODE_ERR;                                                                                           \
                                                                                                                       \
            cmd_code = cmd_code | arg_mask;                                                                            \
                                                                                                                       \
            code_Push_Command(code, cmd_code, value, number_of_register);                                              \
            VERIFY_CODE_ERR;                                                                                           \
                                                                                                                       \
            if (!strcmp("hlt", #name))                                                                                 \
                code->HLT_was_set = true;                                                                              \
        }

        #include "cmd.h"
        #undef DEF_CMD

        //------------------------------------------------------------------

        else if ((cmd[length_of_cmd - 1] == ':') && (argument == nullptr))
        {
            labels_Check_Label(&labels, cmd, code->offset + 1, length_of_cmd);
            VERIFY_LABELS_ERR;

            labels_Push(&labels, cmd, code->offset + 1);
            VERIFY_LABELS_ERR;
        }

        else
        {
            code->err = Incorrect_Command;
            VERIFY_CODE_ERR;
        }
    }
    if (!(code->HLT_was_set))
    {
        code->err = Missed_HLT_Command;
        VERIFY_CODE_ERR;
    }

    code_Shrink_To_Fit(code);
    VERIFY_CODE_ERR;

    labels_Put_Values(&labels, code->pointer);

    printf("---\nSIZE = %d\n(without signature and version)\n", code->offset - SIZE_OF_SIGNATURE - SIZE_OF_VERSION);

    free_buffer(commands);
    labels_Dtor(&labels);

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
        code_Dtor(code);
        return Failed_To_Write_Code_To_File;
    }

    code_Dtor(code);

    return Done_Successfully;
}

//----------------------------------------------------------------------------------------------------------------------