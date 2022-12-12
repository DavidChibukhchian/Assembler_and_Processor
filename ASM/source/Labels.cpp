#include "Labels.h"

//----------------------------------------------------------------------------------------------------------------------

static const size_t MULTIPLIER = 2;
static const size_t START_NUMBER_OF_LABELS = 3;

//----------------------------------------------------------------------------------------------------------------------

void labels_Ctor(labels_struct* labels)
{
    labels->number_of_labels = 0;
    labels->number_of_free_labels = START_NUMBER_OF_LABELS;
    labels->err = 0;

    labels->label = (label_struct*)calloc (START_NUMBER_OF_LABELS, sizeof(label_struct));
    if (labels->label == nullptr)
    {
        labels->err = Failed_To_Create_Array_Of_Labels;
        return;
    }
}

//----------------------------------------------------------------------------------------------------------------------

static int incorrect_label_name(labels_struct* labels, char* label_name, size_t length_of_label_name)
{
    for (size_t i = 0; i < length_of_label_name - 1; i++)
    {
        if ((!isalpha(label_name[i])) && (!isdigit(label_name[i])))
        {
            labels->err = Invalid_Label_Name;
            return labels->err ;
        }
    }

    return Done_Successfully;
}

//----------------------------------------------------------------------------------------------------------------------

static int many_labels_in_a_row(labels_struct* labels, size_t value)
{
    if (labels->number_of_labels != 0)
    {
        if (labels->label[labels->number_of_labels - 1].value == value)
        {
            labels->err = Too_Many_Labels_In_A_Row;
            return labels->err;
        }
    }

    return Done_Successfully;
}

//----------------------------------------------------------------------------------------------------------------------

static int label_already_exists(labels_struct* labels, char* label_name)
{
    for (size_t i = 0; i < labels->number_of_labels; i++)
    {
        if (strcmp(labels->label[i].name, label_name) == 0)
        {
            labels->err = This_Label_Already_Exists;
            return labels->err;
        }
    }

    return Done_Successfully;
}

//----------------------------------------------------------------------------------------------------------------------

void labels_Check_Label(labels_struct* labels, char* label_name, size_t value, size_t length_of_label_name)
{
    if (incorrect_label_name(labels, label_name, length_of_label_name))
        return;

    if (many_labels_in_a_row(labels, value))
        return;

    if (label_already_exists(labels, label_name))
        return;
}


//----------------------------------------------------------------------------------------------------------------------

void labels_Check_Jump(labels_struct* labels, char* label_name)
{
    if ((label_name[0] != ':') || (label_name[1] == '\0'))
    {
        labels->err = Incorrect_Jump_Command;
        return;
    }

    label_name++;
    size_t length_of_label_name = strlen(label_name);

    if (incorrect_label_name(labels, label_name, length_of_label_name + 1))
        return;
}

//----------------------------------------------------------------------------------------------------------------------

static void labels_Expand(labels_struct* labels)
{
    size_t new_size = MULTIPLIER * labels->number_of_labels * sizeof(label_struct);

    label_struct* new_pointer = (label_struct*)realloc (labels->label, new_size);
    if (new_pointer == nullptr)
    {
        labels->err = Failed_To_Resize_Array_Of_Labels;  //todo "resize"
        return;
    }
    labels->label = new_pointer;

    labels->number_of_free_labels = labels->number_of_labels * (MULTIPLIER - 1);
}

//----------------------------------------------------------------------------------------------------------------------

void labels_Push(labels_struct* labels, char* label_name, size_t value)
{
    if (labels->number_of_free_labels == 0)
    {
        labels_Expand(labels);
        if (labels->err)
            return;
    }

    size_t idx = labels->number_of_labels;

    labels->label[idx].name  = label_name;
    labels->label[idx].value = value;

    labels->number_of_labels++;
    labels->number_of_free_labels--;
}

//----------------------------------------------------------------------------------------------------------------------

void labels_Set(labels_struct* labels, void* jumps_ptr, char* code_pointer)
{
    jumps_struct* jumps = (jumps_struct*) jumps_ptr;
    bool label_was_set = false;

    for (size_t i = 0; i < jumps->number_of_addresses; i++)
    {
        for (size_t j = 0; j < labels->number_of_labels; j++)
        {
            if ((strstr(labels->label[j].name, jumps->jump[i].label_name) != nullptr))
            {
                size_t len = strlen(jumps->jump[i].label_name);
                if (labels->label[j].name[len] == ':')
                {
                    *(code_pointer + jumps->jump[i].address) = labels->label[j].value;
                    label_was_set = true;
                    break;
                }
            }
        }

        if (label_was_set)
        {
            label_was_set = false;
            continue;
        }

        jumps->err = Jump_To_Nonexistent_Label;
        return;
    }
}

//----------------------------------------------------------------------------------------------------------------------

void labels_Dtor(labels_struct* labels)
{
    if (labels->label != nullptr)
        free(labels->label);
}

//----------------------------------------------------------------------------------------------------------------------