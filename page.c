#include <stdlib.h>
#include <string.h>
#include "page.h"

Page* new_page_blksize(unsigned int blksize) {
    Page* page = malloc(sizeof(Page));
    if (page == NULL) {
        return NULL;
    }

    page->size = blksize;
    page->data = malloc(sizeof(unsigned char) * blksize);
    if (page->data == NULL) {
        return NULL;
    }

    page_clear(page);
    return page;
}

void page_clear(Page *page) {
    memset(page->data, 0, page->size);
}