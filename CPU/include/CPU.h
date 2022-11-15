#ifndef CPU_CPU_H
#define CPU_CPU_H

#include "Logger.h"
#include "Stack.h"

#define ASSERT(condition, err, code) {  if (!(condition))           \
                                        {                           \
                                            code;                   \
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

char* read_code_to_buffer(FILE* ASM_out, int* err);

int run_code(char* code, Stack* stk, int* registers);

#endif //CPU_CPU_H