#pragma once

#include "int_list.h"

/**
 * @struct
 * @brief   intを持つ線形listを構成するノード
 * @note
 */
struct IntList {
    // データとなる値
    int val;

    // リストの以前の要素
    struct IntList *prev;
};
typedef struct IntList IntList;


IntList *add_IntList(IntList*, int);
int search_IntList(IntList*, int);
void free_all_IntList(IntList**);
