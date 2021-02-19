#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "string_int_list.h"

/**
 * @brief   リストに新しいノードを追加する
 * @param   head リストの先頭を表すポインタのポインタ
 * @param   str 新しいノードの文字列(文字列長はMAX_STRING_SIZE以下 かつ 末尾に終端記号を含む)
 * @param   num 新しいノードの数値
 * @return
 * @note
 * strがMAX_STRING_SIZEを超えていた場合を想定しない。
 */
void add_StringIntList(StringIntList **head, char *str, int num) {
    StringIntList *new_node = malloc(sizeof(StringIntList));
    snprintf(new_node->str, MAX_STRING_SIZE, "%s", str);
    new_node->num = num;
    new_node->next = NULL;

    // newをlistに追加する
    if (*head == NULL) {                // listが空の場合
        *head = new_node;
    } else {
        StringIntList *now = *head;
        while (now->next != NULL) {
            now = now->next;
        }
        now->next = new_node;
    }
}

/**
 * @brief   リストから指定した文字列が一致するノードを見つる
 * @param   head リストの先頭を表すポインタ
 * @param   str 見つけるノードの文字列
 * @return  指定した文字列を持つノードのポインタ
 * @note
 */
StringIntList *search_from_string_StringIntList(StringIntList *head, char *str) {
    StringIntList *now = head;
    while (now != NULL) {
        if (strcmp(now->str, str) == 0) {
            return now;
        }
        now = now->next;
    }
    return NULL;
}