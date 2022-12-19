#ifndef _STACK_H
#define _STACK_H

//----------------------------------------------------------------------------------------------------------------------

#include <stdio.h>
#include <malloc.h>

//----------------------------------------------------------------------------------------------------------------------

typedef int elem_t;
typedef long long int canary_t;

//----------------------------------------------------------------------------------------------------------------------

enum stack_status
{
    ACTIVE,
    INACTIVE
};

//----------------------------------------------------------------------------------------------------------------------

struct Stack
{
    canary_t left_struct_canary;

    stack_status status;
    size_t size;
    size_t capacity;
    elem_t* first_elem;
    elem_t* top_elem;
    FILE* logfile;

    canary_t* left_buff_canary;
    canary_t* right_buff_canary;

    int errors;
    unsigned int buffer_hash;
    unsigned int struct_hash;

    canary_t right_struct_canary;
};

//----------------------------------------------------------------------------------------------------------------------

int stack_Ctor(Stack* stk, size_t init_capacity = 1);

int stack_Push(Stack* stk, elem_t value);

int stack_Pop(Stack* stk, elem_t* top = nullptr);

int stack_Top(Stack* stk, elem_t* top);

int stack_Display(Stack* stk);

void stack_Dump(const Stack* stk, FILE* dump_file);

int stack_Dtor(Stack* stk);

//----------------------------------------------------------------------------------------------------------------------

#endif // _STACK_H