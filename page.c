#include <stdlib.h>
#include <string.h>
#include "page.h"

/**
 * 構造体Pageのメモリを確保する。
 * 
 * data_size    : Pageのデータの大きさ
 * @return 成功したら、確保したメモリ領域
 * @return 失敗したら、NULL
 */
Page* new_page(int data_size) {
    Page* page = malloc(sizeof(Page));
    if (page == NULL) {
        return NULL;
    }

    page->size = data_size;
    page->data = malloc(sizeof(unsigned char) * data_size);
    if (page->data == NULL) {
        return NULL;
    }

    clear_page(page);
    return page;
}

/**
 * 指定するPageのデータの内容を消去する。
 * 
 * page : 指定するPage
 */
void clear_page(Page *page) {
    memset(page->data, 0, page->size);
}

/**
 * Pageからオフセットを指定し、数値を抽出する。
 * 
 * page     : Page
 * offset   : Pageのオフセット
 * @return (value) 抽出した数値
 */
int get_int_from_page(Page *page, int offset) {
    int value;  // 抽出する数値
    memcpy(&value, page->data + offset, sizeof(int));
    return value;
}

/**
 * Pageの指定のオフセットに、数値を代入する。
 * 
 * page     : Page
 * offset   : Pageのオフセット
 * value    : 代入する数値
 */
void set_int_to_page(Page *page, int offset, int value) {
    memcpy(page->data + offset, &value, sizeof(int));
}

/**
 * Get sequential bytes from page.
 * @returns size of the bytes on success 0 on underflow.
 */
int get_bytes_from_page(Page *page, int offset, unsigned char *value) {
    if (page == NULL) {
        return 0;
    }

    int length;
    length = get_int_from_page(page, offset);
    offset += 4;
    
    memcpy(value, page->data + offset, length);
    return length;
}

// Set number of bytes and the bytes themselves to page.
/**
 * Set to page value of int that is size of bytes and the bytes themselves
 *  to page.
 * @returns size written to page on success 0 on undewflow.
 */
int set_bytes_to_page(Page *page, int offset, unsigned char *value, int length) {
    if (page == NULL) {
        return 0;
    }

    // String that is lager than MAX_STRING_SIZE cannot be handled.
    if (length > MAX_STRING_SIZE) {
        length = MAX_STRING_SIZE;
    }

    // sizeについて、ページサイズ

    set_int_to_page(page, offset, length);
    offset += 4;

    memcpy(page->data + offset, value, length);
    length += 4;
    return length;
}

// Get string from page.
/**
 * Get string from page.
 * @returns size of string on success 0 on underflow.
 */
int get_string_from_page(Page *page, int offset, char *str) {
    unsigned char *bytes = calloc(1, sizeof(unsigned char) * MAX_STRING_SIZE);
    int length;

    if (page == NULL) {
        return 0;
    }
    length = get_bytes_from_page(page, offset, bytes);
    if (length == 0) {
        return 0;
    }

    memcpy(str, bytes, length);
    free(bytes);
    return length;
}

/**
 * Set string to page.
 * @returns size written to page on success 0 on underflow.
 */
int set_string_to_page(Page *page, int offset, char *str) {
    int bytes_len, string_len;

    if (page == NULL) {
        return 0;
    }
    string_len = strlen(str) + 1;   // '\0'
    bytes_len = set_bytes_to_page(page, offset, (unsigned char*)str, string_len);
    return bytes_len;
}