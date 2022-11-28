#include "CPU.h"

//----------------------------------------------------------------------------------------------------------------------

static const int SIGNATURE = 0x123ABCD;
static const char  VERSION = 7;

#define SIZEOF_SIGNATURE sizeof(int)
#define SIZEOF_VERSION   sizeof(char)


//----------------------------------------------------------------------------------------------------------------------

#define DEF_CMD(name, arg_type, code) \
CMD_##name,

enum Commands
{
#include "cmd.h"
};

#undef DEF_CMD


enum ARG_CG
{
    NO_ARGS  = 0,
    ARG_NUM  = 1,
    ARG_REGG = 2,
    ARG_RAM  = 3
};

//enum Commands
//{
//    CMD_HLT  = 0,
//    CMD_PUSH = 1,
//    CMD_POP  = 2,
//    CMD_ADD  = 3,
//    CMD_SUB  = 4,
//    CMD_MUL  = 5,
//    CMD_DIV  = 6,
//    CMD_OUT  = 7,
//    CMD_IN   = 8,
//    CMD_DUMP = 9
//};

enum ARG_type
{
    ARG_IMMED = 0,
    ARG_REG   = 1,
    ARG_MEM   = 2
};

enum REG
{
    REG_RAX = 0,
    REG_RBX = 1,
    REG_RCX = 2,
    REG_RDX = 3
};

//----------------------------------------------------------------------------------------------------------------------

static int check_signature(FILE* ASM_out)
{
    int signature_value = 0;

    fread(&signature_value, SIZEOF_SIGNATURE, 1, ASM_out);

    if (signature_value != SIGNATURE)
    {
        fclose(ASM_out);
        return Invalid_Type_Of_Input_File;
    }

    return Done_Successfully;
}

//----------------------------------------------------------------------------------------------------------------------

static int check_version(FILE* ASM_out)
{
    char version_value = 0;

    fread(&version_value, SIZEOF_VERSION, 1, ASM_out);

    if (version_value < VERSION)
    {
        fclose(ASM_out);
        return Outdated_Version_Of_Code;
    }
    if (version_value > VERSION)
    {
        fclose (ASM_out);
        return Incorrect_Version_Of_Code;
    }

    return Done_Successfully;
}

//----------------------------------------------------------------------------------------------------------------------


int check_file(FILE* ASM_out)
{
    int err = 0;

    err = check_signature(ASM_out);
    if (err)
        return err;

    err = check_version(ASM_out);
    if (err)
        return err;

    return Done_Successfully;
}

//----------------------------------------------------------------------------------------------------------------------

void fprintf_bytes(FILE* dump_file, int size)
{
    for (size_t i = 0; i < size; i++)
    {
        (i / 10) ? (fprintf(dump_file, "%zu ", i)) : (fprintf(dump_file, "0%zu ", i));
    }

    fprintf(dump_file, "\n");
}

//----------------------------------------------------------------------------------------------------------------------

void fprintf_commands(FILE* dump_file, char* code, int size)
{
    for (size_t i = 0; i < size; i++)
    {
        (code[i] / 10) ? (fprintf(dump_file, "%d ", code[i])) : (fprintf(dump_file, "0%d ", code[i]));
    }
    fprintf(dump_file, "\n");
}

//----------------------------------------------------------------------------------------------------------------------

void fprintf_line(FILE* dump_file, size_t ip)
{
    for (size_t i = 0; i < ip; i++)
    {
        fprintf(dump_file, "~~~");
    }
    fprintf(dump_file, "^ ip = %zu\n", ip);
}
//----------------------------------------------------------------------------------------------------------------------

void CPU_Dump(char* code, FILE* dump_file, size_t ip)
{
    int size = _msize(code) - SIZEOF_SIGNATURE - SIZEOF_VERSION;

    fprintf_bytes(dump_file, size);

    fprintf_commands(dump_file, code, size);

    fprintf_line(dump_file, ip);
}

//----------------------------------------------------------------------------------------------------------------------

int Dump(char* code, const Stack* stk, size_t ip)
{
    FILE* dump_file = fopen("CPU_dump.txt", "w");
    if (dump_file == nullptr)
    {
        return Failed_To_Create_Dump_File;
    }

    CPU_Dump(code, dump_file, ip);
    stackDump(stk, dump_file);

    fclose(dump_file);
    return Done_Successfully;
}

//----------------------------------------------------------------------------------------------------------------------

static int check_stack(const Stack* stk, Commands command)
{
    size_t min_number_of_elements = 0;

    if (command == CMD_POP)
        min_number_of_elements = 1;
    else
        min_number_of_elements = 2;

    if (stk->size < min_number_of_elements)
        return Too_Few_Elements_In_Stack;

    if ((command == CMD_DIV) && (*stk->top_elem == 0))
        return Division_By_Zero;

    return Done_Successfully;
}

//----------------------------------------------------------------------------------------------------------------------

size_t get_filesize(FILE* filename)
{
    struct stat file_data{};
    fstat(fileno(filename), &file_data);

    return file_data.st_size;
}

//----------------------------------------------------------------------------------------------------------------------

char* read_code_to_buffer(FILE* ASM_out, int* err)
{
    size_t filesize = get_filesize(ASM_out);
    if (filesize == 0)
    {
        fclose(ASM_out);
        *err = Input_File_Is_Empty;
        return nullptr;
    }

    char* code = (char*)calloc (filesize, sizeof(char));
    if (code == nullptr)
    {
        fclose(ASM_out);
        *err = Failed_To_Create_Buffer;
        return code;
    }

    fread(code, sizeof(char), filesize, ASM_out);

    fclose(ASM_out);
    return code;
}


void func(char* stroka)
{
    stroka[7] = 'a';
}

//----------------------------------------------------------------------------------------------------------------------

int run_code(char* code, Stack* stk, int* REG)
{
    int err = 0;
    size_t ip = 0;

    int num1 = 0;
    int num2 = 0;

    while (code[ip] != CMD_HLT)
    {
        switch(code[ip])
        {


#define DEF_CMD(name, arg_type, code)  \
case CMD_##name:                       \
    code;                              \
                                       \
    break;
#include "cmd.h"


            case CMD_PUSH:
                ip++;
                switch(code[ip])
                {
                    case ARG_IMMED:
                        ip++;
                        stackPush(stk, *(int*)(code + ip));
                        ip += sizeof(int);
                        break;

                    case ARG_REG:
                        ip++;
                        stackPush(stk, *(REG + code[ip]));
                        ip++;
                        break;

                    case ARG_MEM:
                        ip++;
                        break;
                }

                break;

            case CMD_POP:
                CHECK_STACK(CMD_POP);
                ip += 2;
                stackPop(stk, REG + code[ip]);
                ip++;

                break;

            case CMD_ADD:
                CHECK_STACK(CMD_ADD);
                stackPop(stk, &num1);
                stackPop(stk, &num2);
                stackPush(stk, num1 + num2);
                ip++;

                break;

            case CMD_SUB:
                CHECK_STACK(CMD_SUB);
                stackPop(stk, &num1);
                stackPop(stk, &num2);
                stackPush(stk, num2 - num1);
                ip++;

                break;

            case CMD_MUL:
                CHECK_STACK(CMD_MUL);
                stackPop(stk, &num1);
                stackPop(stk, &num2);
                stackPush(stk, num1 * num2);
                ip++;

                break;

            case CMD_DIV:
                CHECK_STACK(CMD_DIV);
                stackPop(stk, &num1);
                stackPop(stk, &num2);
                stackPush(stk, num2 / num1);
                ip++;

                break;

            case CMD_OUT:
                printf("\nStack top element: [%d]\n\n", *stk->top_elem);
                ip++;

                break;

            case CMD_IN:
                err = scanf("%d", &num1);
                if (!err)
                {
                    free(code);
                    return Entered_Invalid_Value;
                }
                stackPush(stk, num1);
                ip++;

                break;

            case CMD_HLT:
                break;

            case CMD_DUMP:
                err = Dump(code, stk, ip);
                if (err)
                {
                    free(code);
                    return err;
                }
                ip++;

                break;

            default:
                return Invalid_Syntax;
                break;
        }
    }

    free(code);
    return Done_Successfully;
}

//----------------------------------------------------------------------------------------------------------------------