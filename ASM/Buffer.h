#ifndef ASM_BUFFER_H
#define ASM_BUFFER_H

size_t get_file_size(FILE* filename);

char* read_file_to_buffer(FILE* filename, size_t filesize);

size_t count_lines(char* buffer);

void change_delimiter(char* buffer, char old_delimiter, char new_delimiter);

char** split_buffer(char* buffer, size_t number_of_lines, size_t filesize);

#endif //ASM_BUFFER_H
