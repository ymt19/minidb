#include <stdlib.h>
#include <string.h>
#include "page.h"

// 参照 https://github.com/hawstan/ByteBuffer/blob/master/ByteBuffer.c

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

// get 32bit(4byte) int from page.
int page_get_int(Page *page, int offset, int* value) {
    if (page == NULL) {
        return 0;
    }

    // 微妙
    *value = *((int*)(page->data + offset));
    return 4;
}

int page_set_int(Page *page, int offset, int n) {
    if (page == NULL) {
        return 0;
    }

    page->data[offset++] = n & 255;
    page->data[offset++] = (n << 8) & 255;
    page->data[offset++] = (n << 16) & 255;
    page->data[offset++] = (n << 24) & 255;
    return 4;
}