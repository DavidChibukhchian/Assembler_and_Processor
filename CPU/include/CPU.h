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

char* read_code_to_buffer(FILE* ASM_out, int* err);

void run_code(char* code, Stack* stk);

#endif //CPU_CPU_H