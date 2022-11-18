#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "Buffer.h"

#include "ASM.h"

unsigned char ARG_num = 128;
unsigned char ARG_reg = 64;
unsigned char ARG_ram = 32;

//----------------------------------------------------------------------------------------------------------------------

#define MULTIPLIER 1.5
#define BUFF_SIZE 35
#define SPACE 1

//----------------------------------------------------------------------------------------------------------------------

static const int SIGNATURE = 0x123ABCD;
static const char  VERSION = 7;

#define SIZEOF_SIGNATURE sizeof(int)
#define SIZEOF_VERSION   sizeof(char)

//----------------------------------------------------------------------------------------------------------------------


#define DEF_CMD(name, arg_type, cpu) \
CMD_##name,

enum Commands
{
    #include "cmd.h"
};

#undef DEF_CMD

//----------------------------------------------------------------------------------------------------------------------

//#define DEF_CMD(name, arg_type, code) \
//arg_type,
//
//enum ARG_type
//{
//#include "cmd.h"
//};
//
//#undef DEF_CMD


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

static int check_no_args(commands_struct* commands, size_t i, size_t scanned_symbols)
{
    int  test_value = 0;
    char test_str[BUFF_SIZE] = "";

    size_t scanned_symbols_test_1 = 0;
    size_t scanned_symbols_test_2 = 0;

    sscanf(commands->array_of_commands[i] + scanned_symbols, "%d%n", &test_value, &scanned_symbols_test_1);
    sscanf(commands->array_of_commands[i] + scanned_symbols, "%s%n",  test_str,   &scanned_symbols_test_2);

    if (scanned_symbols_test_1 | scanned_symbols_test_2)
    {
        return Invalid_Syntax;
    }

    return Done_Successfully;
}

//----------------------------------------------------------------------------------------------------------------------

static int identify_args(unsigned char* args_mask, char* cmd, commands_struct* commands, size_t i, size_t scanned_symbols, int* number_of_register, int* value)
{
    size_t counter = 0;

    if(check_no_args(commands, i, scanned_symbols))
    {
//        printf("here");
        return Invalid_Syntax;
    }

    if (sscanf(commands->array_of_commands[i] + scanned_symbols + SPACE, "%d%n", value, &counter))
    {
        if (check_no_args(commands, i, scanned_symbols + SPACE + counter))
        {
//            printf("here");
            return Invalid_Syntax;
        }

        *args_mask | ARG_num;
        return Done_Successfully;
    }

    else if (sscanf(commands->array_of_commands[i] + scanned_symbols + SPACE, "%s%n", cmd, &counter))
    {
        if (check_no_args(commands, i, scanned_symbols + SPACE + counter))
        {
//            printf("here");
            return Invalid_Syntax;
        }

        else if ((counter == 3) && (cmd[0] == 'r') && (cmd[2] == 'x'))
        {
            *args_mask | ARG_reg;
            *number_of_register = cmd[1] - 'a' + 1;
            return Done_Successfully;
        }

        else if ((cmd[0] == '[') && (cmd[counter - 1] == ']'))
        {
            if ((cmd[1] == 'r') && (cmd[3] == 'x'))
            {
                *args_mask | ARG_reg;
                *number_of_register = cmd[1] - 'a' + 1;

                *args_mask | ARG_ram;

                if (cmd[4] != '+')
                {
//                    printf("here");
                    return Invalid_Syntax;
                }

                if (!sscanf(commands->array_of_commands[i] + scanned_symbols + SPACE + 1 + 3 + 1, "%d", value))
                {
//                    printf("here");
                    return Invalid_Syntax;
                }

                *args_mask | ARG_num;
                return Done_Successfully;
            }
            else
            {
//                printf("here");
                return Invalid_Syntax;
            }
        }
        else
        {
//            printf("here");
            return Invalid_Syntax;
        }
    }
    else
    {
//        printf("here");
        return Invalid_Syntax;
    }
}

//----------------------------------------------------------------------------------------------------------------------

static int make_arg_mask(unsigned char* args_mask, char* cmd, commands_struct* commands, size_t i, size_t scanned_symbols, ARG_type arg_type, int* number_of_register, int* value)
{
    int err = 0;

    if (arg_type == NO_ARGS)
    {
        err = check_no_args(commands, i, scanned_symbols);
    }

    else
    {
        err = identify_args(args_mask, cmd, commands, i, scanned_symbols, number_of_register, value);
    }

    if (err)
    {
        return err;
    }

    return Done_Successfully;
}


//----------------------------------------------------------------------------------------------------------------------

static void codePush_Command(code_struct* code, unsigned char byte, int number_of_register, int value)
{
    codePush_Char(code, byte);

    if (byte & ARG_reg)
    {
        codePush_Char(code, number_of_register);
    }

    if (byte & ARG_num)
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
    code->size   = SIZEOF_SIGNATURE + SIZEOF_VERSION + number_of_commands;

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

int create_code_array(commands_struct* commands, code_struct* code)
{
    unsigned char arg_mask = 0;
    unsigned char byte = 0;

    int HLT_was_set = false;

    codeCtor(code, commands->number_of_commands);
    VERIFY_CODE_ERR;

    char cmd[BUFF_SIZE] = "";
    size_t scanned_symbols = 0;

    int number_of_register = 0;
    int value = 0;

    // hlt was set - исключение дял кодогенератора

    for (size_t i = 0; i < commands->number_of_commands; i++)
    {
        printf("1");
        sscanf(commands->array_of_commands[i], "%s%n", cmd, &scanned_symbols);

        if (false) {}

#define DEF_CMD(name, arg_type, cpu)                                                                                   \
else if (strcmp(cmd, #name) == 0)                                                                                      \
{                                                                                                                      \
    printf("1");                                                                                                       \
    arg_mask = 0;                                                                                                      \
    code->err = make_arg_mask(&arg_mask, cmd, commands, i, scanned_symbols, arg_type, &number_of_register, &value);    \
    VERIFY_CODE_ERR;                                                                                                   \
    byte = 0;                                                                                                          \
    byte = CMD_##name | arg_mask;                                                                                      \
                                                                                                                       \
    codePush_Command(code, byte, number_of_register, value);                                                           \
    VERIFY_CODE_ERR;                                                                                                   \
}

#include "cmd.h"

#undef DEF_CMD

        else
        {
            free_buffer(commands);
            if (code->pointer != nullptr)
                free(code->pointer);

            printf("here");
            return Invalid_Syntax;
        }
    }

    codeShrinkToFit(code);
    VERIFY_CODE_ERR;

    printf("%zu", code->offset);

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

    fclose(ASM_out);
    free(code->pointer);

    return Done_Successfully;
}

//----------------------------------------------------------------------------------------------------------------------



/*


        if (strcmp(cmd, "push") == 0)
        {
            codePush_Char(code, CMD_PUSH);

//----------------------------------------------------------------------------------------------------------------------
            if (sscanf(commands->array_of_commands[i] + scanned_symbols + SPACE, "%d", &value))
            {
                codePush_Char(code, ARG_IMMED);
                codePush_Int(code, value);
            }
            else
            {
                sscanf(commands->array_of_commands[i] + scanned_symbols + SPACE, "%s%n", cmd, &scanned_symbols);

                if ((scanned_symbols == 3) && (cmd[0] == 'r') && (cmd[2] == 'x'))
                {
                    codePush_Char(code, ARG_REG);
                    codePush_Char(code, cmd[1] - 'a' + 1);
                }
                else if (strcmp(cmd, "mem") == 0)
                {
                    codePush_Char(code, ARG_MEM);
                }
                else
                {
                    free_buffer(commands);
                    free(code->pointer);
                    return Invalid_Syntax;
                }
            }
//----------------------------------------------------------------------------------------------------------------------

            VERIFY_CODE_ERR;
        }

        else if (strcmp(cmd, "pop") == 0)
        {
            codePush_Char(code, CMD_POP);

//----------------------------------------------------------------------------------------------------------------------

            sscanf(commands->array_of_commands[i] + scanned_symbols + SPACE, "%s%n", cmd, &scanned_symbols);
            if ((scanned_symbols == 3) && (cmd[0] == 'r') && (cmd[2] == 'x'))
            {
                codePush_Char(code, ARG_REG);
                codePush_Char(code, cmd[1] - 'a' + 1);
            }
            else
            {
                free_buffer(commands);
                free(code->pointer);
                return Invalid_Syntax;
            }
//----------------------------------------------------------------------------------------------------------------------

            VERIFY_CODE_ERR;
        }

        else if (strcmp(cmd, "add") == 0)
        {
            codePush_Command(code, CMD_ADD);

            VERIFY_CODE_ERR;
        }

        else if (strcmp(cmd, "sub") == 0)
        {
            codePush_Command(code, CMD_SUB);

            VERIFY_CODE_ERR;
        }

        else if (strcmp(cmd, "mul") == 0)
        {
            codePush_Command(code, CMD_MUL);

            VERIFY_CODE_ERR;
        }

        else if (strcmp(cmd, "div") == 0)
        {
            codePush_Command(code, CMD_DIV);

            VERIFY_CODE_ERR;
        }

        else if (strcmp(cmd, "out") == 0)
        {
            codePush_Command(code, CMD_OUT);

            VERIFY_CODE_ERR;
        }

        else if (strcmp(cmd, "in") == 0)
        {
            codePush_Command(code, CMD_IN);

            VERIFY_CODE_ERR;
        }

        else if (strcmp(cmd, "dump") == 0)
        {
            codePush_Command(code, CMD_DUMP);

            VERIFY_CODE_ERR;
        }

        else if (strcmp(cmd, "hlt") == 0)
        {
            codePush_Command(code, CMD_HLT);

            VERIFY_CODE_ERR;
            HLT_was_set = true;
        }

        else
        {
            free_buffer(commands);
            free(code->pointer);
            return Invalid_Syntax;
        }


    }
}



    if (!HLT_was_set)
    {
        free_buffer(commands);
        free(code->pointer);
        return Missed_HLT_Command;
    }


*/

//----------------------------------------------------------------------------------------------------------------------