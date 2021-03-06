#pragma once

/**
 * @def     MAX_STRING_SIZE
 * @brief   扱う文字列の最大長
 * @note
 */
#define MAX_STRING_SIZE (128)

/**
 * @struct  Page
 * @brief   Page構造体
 * @note
 */
typedef struct {
    // Pageのデータサイズ
    unsigned int size;

    // Pageのデータ本体である
    // 大きさは、FilaManager構造体のdata_sizeメンバ
    char* data;
} Page;

Page* new_page(int);
Page *new_page_bytes(char*, int);
void clear_page(Page*);
int get_int_from_page(Page*, int);
void set_int_to_page(Page*, int, int);
int get_string_from_page(Page*, int, char*);
int set_string_to_page(Page*, int, char*, int);
