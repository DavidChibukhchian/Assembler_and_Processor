#include "CPU.h"

//----------------------------------------------------------------------------------------------------------------------

static const char  VERSION = 7;

//--------------------------------------------------------------------------------------------------------------------//

static const int SIGNATURE = 0x123ABCD;

static const size_t SIZE_OF_SIGNATURE = sizeof(int);
static const size_t SIZE_OF_VERSION   = sizeof(char);

static const unsigned char ARG_NUM = 1 << 7;
static const unsigned char ARG_REG = 1 << 6;
static const unsigned char ARG_RAM = 1 << 5;

//----------------------------------------------------------------------------------------------------------------------

#define DEF_CMD(name, arg_type, code)                                                                                  \
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

//--------------------------------------------------------------------------------------------------------------------//

static const size_t MAX_NUMBER_OF_OPERATIONS = 50000;

//----------------------------------------------------------------------------------------------------------------------

int open_files(files_struct* files, char** argv)
{
    FILE* ASM_out = fopen(argv[1], "rb");
    if (ASM_out == nullptr)
    {
        return Failed_To_Open_Input_File;
    }
    files->ASM_out = ASM_out;

    return Done_Successfully;
}

//----------------------------------------------------------------------------------------------------------------------

void close_files(files_struct* files)
{
    if (files->ASM_out != nullptr)
        fclose(files->ASM_out);

    fclose(files->logfile);
}

//----------------------------------------------------------------------------------------------------------------------

int open_logfile(files_struct* files)
{
    FILE* logfile = fopen("CPU_logfile.txt",  "w");
    if (logfile == nullptr)
    {
        printf("---\nERROR: Failed to create logfile\n---");
        return Failed_To_Create_Logfile;
    }
    files->logfile = logfile;

    files->ASM_out = nullptr;

    return Done_Successfully;
}

//----------------------------------------------------------------------------------------------------------------------

static int check_signature_and_version(FILE* ASM_out)
{
    int file_signature = 0;
    char file_version  = 0;

    fread(&file_signature, SIZE_OF_SIGNATURE, 1, ASM_out);
    fread(&file_version,   SIZE_OF_VERSION,   1, ASM_out);

    if (file_signature != SIGNATURE)
        return Invalid_Type_Of_Input_File;

    if (file_version < VERSION)
        return Outdated_Version_Of_Code;

    if (file_version > VERSION)
        return Incorrect_Version_Of_Code;

    return Done_Successfully;
}

//----------------------------------------------------------------------------------------------------------------------

static int check_file(FILE* ASM_out, size_t filesize)
{
    if (filesize <= SIZE_OF_SIGNATURE + SIZE_OF_VERSION)
        return Invalid_Type_Of_Input_File;

    int err = check_signature_and_version(ASM_out);
    if (err)
        return err;

    if (filesize == SIZE_OF_SIGNATURE + SIZE_OF_VERSION)
        return Input_File_Is_Empty;

    return Done_Successfully;
}
//----------------------------------------------------------------------------------------------------------------------

static int check_RAM_address(int address)
{
    if (address > SIZE_OF_RAM)
        return Address_Is_Bigger_Than_Size_Of_RAM;

    return Done_Successfully;
}

//----------------------------------------------------------------------------------------------------------------------

static int check_stack(const Stack* stack, Commands command)
{
    size_t min_number_of_elements = 0;

    if (command == CMD_pop)
    {
        min_number_of_elements = 1;
        if (stack->size < min_number_of_elements)
            return Stack_Is_Empty;
    }
    else
    {
        min_number_of_elements = 2;
        if (stack->size < min_number_of_elements)
            return Too_Few_Elements_In_Stack;
    }

    if ((command == CMD_div) && (*stack->top_elem == 0))
        return Division_By_Zero;

    return Done_Successfully;
}

//----------------------------------------------------------------------------------------------------------------------

static void get_numbers_from_stack(Stack* stack, int* number_1, int* number_2, ARG_type arg_type)
{
    if (arg_type == LABEL)
    {
        stack_Pop(stack, number_1);
        stack_Top(stack, number_2);
        stack_Push(stack, *number_1);
    }
    else
    {
        stack_Pop(stack, number_1);
        stack_Pop(stack, number_2);
    }
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

void fprintf_commands(FILE* dump_file, unsigned char* code, int size)
{
    unsigned char cmd = 0;
    for (size_t i = 0; i < size; i++)
    {
        cmd = code[i] & ~(ARG_NUM + ARG_REG + ARG_RAM);
        (cmd / 10) ? (fprintf(dump_file, "%d ", cmd)) : (fprintf(dump_file, "0%d ", cmd));
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

void CPU_Dump(Code code, FILE* dump_file, size_t ip)
{
    int size = _msize(code);

    fprintf_bytes(dump_file, size);

    fprintf_commands(dump_file, code, size);

    fprintf_line(dump_file, ip);
}

//----------------------------------------------------------------------------------------------------------------------

static int Dump(Code code, const Stack* stack, size_t ip)
{
    FILE* dump_file = fopen("CPU_dump.txt", "w");
    if (dump_file == nullptr)
    {
        return Failed_To_Create_Dump_File;
    }

    CPU_Dump(code, dump_file, ip);
    stack_Dump(stack, dump_file);

    fclose(dump_file);

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

static int create_code_array(Code* code, FILE* ASM_out, size_t filesize)
{
    size_t code_size = filesize - SIZE_OF_SIGNATURE - SIZE_OF_VERSION;

    *code = (Code)calloc (code_size, sizeof(char));
    if (*code == nullptr)
        return Failed_To_Create_Array_Of_Code;

    fread(*code, sizeof(char), code_size, ASM_out);

    return Done_Successfully;
}

//----------------------------------------------------------------------------------------------------------------------

int read_code(Code* code, files_struct* files)
{
    size_t filesize = get_filesize(files->ASM_out);

    int err = 0;

    err = check_file(files->ASM_out, filesize);
    if (err)
        return err;

    err = create_code_array(code, files->ASM_out, filesize);
    if (err)
        return err;

    return Done_Successfully;
}

//----------------------------------------------------------------------------------------------------------------------

static void identify_arg(char cmd_byte, unsigned char* arg_mask, unsigned char* cmd)
{
    *arg_mask = cmd_byte  &  (ARG_NUM + ARG_REG + ARG_RAM);
    *cmd      = cmd_byte  & ~(ARG_NUM + ARG_REG + ARG_RAM);
}

//----------------------------------------------------------------------------------------------------------------------

int run_code(Code code, Stack* stack, int* REG, int* RAM)
{
    int err = 0;

    unsigned char arg_mask = 0;
    unsigned char cmd = 0;

    int new_ip   = 0;
    int number_1 = 0;
    int number_2 = 0;
    char number_of_register = 0;

    size_t number_of_operations = 0;

    Stack call_stack = {};
    stack_Ctor(&call_stack);

    int ip = 0;

    while (code[ip] != CMD_hlt)
    {
        identify_arg(code[ip], &arg_mask, &cmd);

        switch (cmd)
        {

            //-------------------------------Code-Generation-------------------------------

            #define DEF_CMD(name, arg_type, execution_code)                                                            \
                                                                                                                       \
            case CMD_##name:                                                                                           \
                execution_code;                                                                                        \
                break;

            #include "cmd.h"

            #undef DEF_CMD

            //-----------------------------------------------------------------------------
        }

        number_of_operations++;
        if (number_of_operations > MAX_NUMBER_OF_OPERATIONS)
        {
            free(code);
            return Infinite_Loop_Was_Detected;
        }
    }

    stack_Dtor(&call_stack);
    free(code);

    return Done_Successfully;
}

//----------------------------------------------------------------------------------------------------------------------