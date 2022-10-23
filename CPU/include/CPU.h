#ifndef CPU_CPU_H
#define CPU_CPU_H

int* read_input_file(FILE* ASM_out);

void run_code(int* code, Stack* stk);

#endif //CPU_CPU_H
