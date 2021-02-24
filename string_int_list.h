#pragma once

#include "page.h"

/**
 * @struct
 * @brief   文字列、数値を持つlistのノード
 */
struct StringIntList {
    char str[MAX_STRING_SIZE];
    int num;
    struct StringIntList *next;
};
typedef struct StringIntList StringIntList;


void add_StringIntList(StringIntList **head, char *str, int num);
StringIntList *search_from_string_StringIntList(StringIntList *head, char *str);