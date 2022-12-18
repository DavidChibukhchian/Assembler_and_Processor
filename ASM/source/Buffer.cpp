#include "Buffer.h"

//----------------------------------------------------------------------------------------------------------------------

size_t get_filesize(FILE* filename)
{
    struct stat file_data = {};
    fstat(fileno(filename), &file_data);

    return file_data.st_size;
}

//----------------------------------------------------------------------------------------------------------------------

char* read_file_to_buffer(FILE* filename, size_t filesize)
{
    char* buffer = (char*)calloc (1 + filesize + 1, sizeof(char));
    if (buffer == nullptr)
    {
        return nullptr;
    }

    *buffer = '\n';
    *(buffer + filesize + sizeof(char)) = '\0';

    fread(buffer + sizeof(char), sizeof(char), filesize, filename);

    return buffer;
}

//----------------------------------------------------------------------------------------------------------------------

size_t count_lines(char* buffer)
{
    size_t number_of_lines = 0;

    while (*buffer != '\0')
    {
        if (*buffer == '\n')
        {
            number_of_lines++;
        }
        buffer++;
    }

    return number_of_lines;
}

//----------------------------------------------------------------------------------------------------------------------

void change_delimiter(char* buffer, char old_delimiter, char new_delimiter)
{
    while (*buffer != '\0')
    {
        if (*buffer == old_delimiter)
        {
            *buffer = new_delimiter;
        }

        buffer++;
    }
}

//----------------------------------------------------------------------------------------------------------------------

char** split_buffer(char* buffer, size_t number_of_lines, size_t filesize)
{
    change_delimiter(buffer, '\n', '\0');

    char** array_of_commands = (char**)calloc (number_of_lines, sizeof(char*));
    if (array_of_commands == nullptr)
    {
        return nullptr;
    }

    buffer++;
    size_t j = 0;
    for (size_t i = 0; i <= filesize; i++)
    {
        if (*(buffer + i - 1) == '\0')
        {
            array_of_commands[j] = buffer + i;
            j++;
        }
    }

    return array_of_commands;
}

//----------------------------------------------------------------------------------------------------------------------

void* recalloc(void* mem_pointer, size_t new_size)
{
    if (new_size == 0)
    {
        free(mem_pointer);
        return nullptr;
    }

    void* new_pointer = nullptr;

    if (mem_pointer == nullptr)
    {
        new_pointer = (void*)calloc (new_size, sizeof(char));
        if (new_pointer == nullptr)
            return nullptr;

        return new_pointer;
    }

    size_t old_size = _msize(mem_pointer);

    new_pointer = (void*)realloc (mem_pointer, new_size);
    if (new_pointer == nullptr)
        return nullptr;

    if (new_size > old_size)
    {
        for (size_t i = old_size; i < new_size; i++)
            ((char*)new_pointer)[i] = 0;
    }

    return new_pointer;

    // Возвращается  указатель на переразмещенный  блок памяти.
    // Если нет подходящей памяти, возвращается NULL (но память по ptr не освобождается).
}

//----------------------------------------------------------------------------------------------------------------------