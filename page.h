#ifndef PAGE_H
#define PAGE_H

typedef struct Page Page;
struct Page {
    unsigned int size;
    unsigned char* data;
};

#endif