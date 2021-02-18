#include <stdlib.h>
#include "int_list.h"

/**
 * @brief   IntListの新しいノードを作成する
 * @param   tail リストの以前のノード
 * @param   val 追加する値
 * @return  作成したIntListの末尾を示すポインタ
 * @note
 * 
 * @attention
 */
IntList *add_IntList(IntList *tail, int val) {
    IntList *node = malloc(sizeof(IntList));
    node->val = val;
    node->prev = tail;
    return node;
}

/**
 * @brief   IntListのlist内に指定された値が含まれているかを識別する
 * @param   val 指定する値
 * @param   tail 調べるlistの末尾のノード
 * @return  存在する場合1、存在しない場合0
 * @note
 * 
 * @attention
 */
int search_IntList(IntList *tail, int val) {
    IntList *now = tail;
    while (now != NULL) {
        if (now->val == val) {
            return 1;
        }
        now = now->prev;
    }
    return 0;
}

/**
 * @brief   IntListのすべてのノードを解放する
 * @param   tail 解放するlistの末尾を示すポインタのポインタ
 * @return
 * @note
 * 
 * @attention
 */
void free_all_IntList(IntList **tail) {
    IntList *next;
    IntList *now = *tail;
    while (now != NULL) {
        next = now;
        now = now->prev;
        free(next);
    }
    *tail = NULL;
}