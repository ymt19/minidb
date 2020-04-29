#ifndef PAGE_H
#define PAGE_H

typedef struct Page Page;
struct Page {
    unsigned int size;
    unsigned char* data;
};

Page* new_page_blksize(void);
void page_clear(Page*);
int page_get_int(Page*, int, int*);
int page_set_int(Page*, int, int);
int page_get_bytes(Page*, int, void*);
int page_set_bytes(Page*, int, void*);
int page_get_string(Page*, int, char**);
int page_set_string(Page*, int, char*);

#endif