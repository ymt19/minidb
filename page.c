#include <stdlib.h>
#include <string.h>

#include "file_manager.h"
#include "page.h"

// 参照 https://github.com/hawstan/ByteBuffer/blob/master/ByteBuffer.c

/**
 * Allocates a new page.
 */
Page* new_page(void) {
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

/**
 * Clear the contents of specified page.
 */
void page_clear(Page *page) {
    memset(page->data, 0, page->size);
}

/**
 * Get value of int from page.
 * @returns 4 on success 0 on underflow
 */
int page_get_int(Page *page, int offset, int* value) {
    if (page == NULL) {
        return 0;
    }

    // 微妙
    *value = *((int*)(page->data + offset));
    return 4;
}

/**
 * Set value of int to page.
 * @returns 4 on success 0 on underflow.
 */
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

/**
 * Get sequential bytes from page.
 * @returns size of the bytes on success 0 on underflow.
 */
int page_get_bytes(Page *page, int offset, void *value) {
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
/**
 * Set to page value of int that is size of bytes and the bytes themselves
 *  to page.
 * @returns size written to page on success 0 on undewflow.
 */
int page_set_bytes(Page *page, int offset, unsigned char *value) {
    if (page == NULL) {
        return 0;
    }

    int length = strlen(value);
    // String that is lager than MAX_STRING_SIZE cannot be handled.
    if (length > MAX_STRING_SIZE) {
        length = MAX_STRING_SIZE;
    }
    page_set_int(page, offset, length);
    offset += 4;
    memmove(page->data + offset, value, min(length, MAX_STRING_SIZE));
    length += 4;
    return length;
}

// Get string from page.
/**
 * Get string from page.
 * @returns size of string on success 0 on underflow.
 */
int page_get_string(Page *page, int offset, char *str) {
    printf("%p page_get_string str\n", str);
    unsigned char *bytes = calloc(1, sizeof(unsigned char) * MAX_STRING_SIZE);
    int length;

    if (page == NULL) {
        return 0;
    }
    length = page_get_bytes(page, offset, bytes);
    if (length == 0) {
        return 0;
    }
    memmove(str, bytes, length);
    free(bytes);
    return length;
}

/**
 * Set string to page.
 * @returns size written to page on success 0 on underflow.
 */
int page_set_string(Page *page, int offset, char *str) {
    int length;

    if (page == NULL) {
        return 0;
    }

    length = page_set_bytes(page, offset, str);

    return length;
}