#ifndef CPU_CPU_H
#define CPU_CPU_H

#include "Logger.h"
#include "Stack.h"

#define ASSERT(condition, err) {  if (!(condition))           \
                                  {                           \
                                      dump(logfile, err);     \
                                      fclose(logfile);        \
                                      return err;             \
                                  }                           }

#define VERIFY(err) {  if (err)                    \
                       {                           \
                           dump(logfile, err);     \
                           fclose(logfile);        \
                           return err;             \
                       }                           }

struct code_struct
{
    char* code;
    size_t size;
};

int check_file(FILE* ASM_out);

char* read_code_to_buffer(FILE* ASM_out, int* err);

int check_code_version(char* code);

int run_code(char* code, Stack* stk, int* registers);

#endif //CPU_CPU_H