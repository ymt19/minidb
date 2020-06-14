#pragma once

#define MAX_STRING_SIZE (1024)

typedef struct {
    unsigned int size;
    unsigned char* data;
} Page;

Page* new_page(int);
void clear_page(Page*);
int get_int_from_page(Page*, int, int*);
int set_int_to_page(Page*, int, int);
int get_bytes_from_page(Page*, int, unsigned char*);
int set_bytes_to_page(Page*, int, unsigned char*, int);
int get_string_from_page(Page*, int, char*);
int set_string_to_page(Page*, int, char*);
