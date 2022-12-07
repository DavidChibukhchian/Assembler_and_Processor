#ifndef _LABELS_H
#define _LABELS_H

//----------------------------------------------------------------------------------------------------------------------

#include <ctype.h>
#include <string.h>
#include <malloc.h>
#include "Logger.h"
#include "ASM.h"

//----------------------------------------------------------------------------------------------------------------------

struct label_struct
{
    char* name;
    size_t value;
};

struct labels_struct
{
    label_struct* label;
    size_t number_of_labels;
    size_t number_of_free_labels;
    int err;
};

//----------------------------------------------------------------------------------------------------------------------

void labels_Ctor(labels_struct* labels);

void labels_Push(labels_struct* labels, char* label_name, size_t value);

void labels_Check_Label(labels_struct* labels, char* label_name, size_t value, size_t length_of_label_name);

void labels_Check_Jump(labels_struct* labels, char* label_name);

void labels_Set(labels_struct* labels, void* jumps, char* code_pointer);

void labels_Dtor(labels_struct* labels);

//----------------------------------------------------------------------------------------------------------------------

#endif // _LABELS_H