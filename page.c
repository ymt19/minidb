#include <stdlib.h>
#include <string.h>

#include "file_manager.h"
#include "page.h"

// 参照 https://github.com/hawstan/ByteBuffer/blob/master/ByteBuffer.c

Page* new_page_blksize(void) {
    Page* page = malloc(sizeof(Page));
    if (page == NULL) {
        return NULL;
    }

    page->size = g_blksize;
    page->data = malloc(sizeof(unsigned char) * g_blksize);
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

int page_set_int(Page *page, int offset, int value) {
    if (page == NULL) {
        return 0;
    }
    
    page->data[offset++] = value & 255;
    page->data[offset++] = (value << 8) & 255;
    page->data[offset++] = (value << 16) & 255;
    page->data[offset++] = (value << 24) & 255;
    return 4;
}

// get bytes from page.
int page_get_bytes(Page *page, int offset, unsigned char **value) {
    if (page == NULL) {
        return 0;
    }

    int length;
    page_get_int(page, offset, &length);
    offset += 4;
    memmove(value, page->data + offset, length);
    return length;
}

// Set number of bytes and the bytes themselves to page.
int page_set_bytes(Page *page, int offset, unsigned char *value) {
    if (page == NULL) {
        return 0;
    }

    int length = strlen(value);
    page_set_int(page, offset, length);
    offset += 4;
    memmove(page->data + offset, value, length);
    return length;
}

// Get string from page.
int page_get_string(Page *page, int offset, char **str) {
    unsigned char* bytes;
    int length;

    if (page == NULL) {
        return 0;
    }

    length = page_get_bytes(page, offset, &bytes);
    if (length == 0) {
        return 0;
    }

    memmove(*str, bytes, length);
    return length;
}

// Set string from page.
int page_set_string(Page *page, int offset, char *str) {
    int length;

    if (page == NULL) {
        return 0;
    }
    length = page_set_bytes(page, offset, str);
    if (length == 0) {
        return 0;
    }

    return length;
}