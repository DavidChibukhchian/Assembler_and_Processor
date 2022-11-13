#include <stdio.h>
#include <sys\stat.h>
#include <malloc.h>
#include "Buffer.h"

//----------------------------------------------------------------------------------------------------------------------

size_t get_filesize(FILE* filename)
{
    struct stat file_data{};
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

    buffer++;
    while (*buffer != '\0')
    {
        if ((*(buffer - sizeof(char)) == '\n') && (*buffer != '\n'))
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
    
    char** text = (char**)calloc (number_of_lines, sizeof(char*));
    if (text == nullptr)
    {
        return nullptr;
    }

    char** text_i = text;

    for (char* ptr = buffer; ptr <= (buffer + filesize); ptr++)
    {
        if ( (*(ptr - 1) == '\0') && (*ptr != '\0') )
        {
            *text_i = ptr;
            text_i++;
        }
    }

    return text;
}

//----------------------------------------------------------------------------------------------------------------------