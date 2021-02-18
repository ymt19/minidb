#include <stdio.h>
#include <stdlib.h>
#include "buffer_list.h"
#include "buffer_manager.h"

/**
 * @brief   Buffer Listから指定したBlockの情報を持つBufferを得る
 * @param   tail Buffer Listの末尾のポインタ
 * @param   blk 指定するBlockのポインタ
 * @return  Blockの情報を持つBufferのポインタ、見つからない場合はNULL
 * @note
 * 
 * @attention
 */
Buffer *get_buffer_from_BufferList(BufferList *tail, Block *blk) {
    BufferList *now = tail;
    while (now != NULL) {
        if (block_is_equal(now->buff->blk, blk)) {
            return now->buff;
        }
        now = now->prev;
    }
    return NULL;
}

/**
 * @brief   BufferListに新しいノードを加える
 * @param   tail Buffer Listの末尾のポインタのポインタ
 * @param   buff 追加するBuffer
 * @return
 * @note
 * 
 * @attention
 */
void add_BufferList(BufferList **tail, Buffer *buff) {
    BufferList *new = malloc(sizeof(BufferList));
    new->buff = buff;
    new->prev = *tail;
    *tail = new;
}

/**
 * @brief   BufferListの指定のBufferを持つノードを除去する
 * @param   tail Buffer Listの末尾のポインタのポインタ
 * @param   buff 除去するBuffer
 * @return 
 * @note
 * 除去するノードが存在すればリスト内のうち1つのみ除去し、
 * 存在しないならば何もしない。
 * @attention
 */
void remove_BufferList(BufferList **tail, Buffer *buff) {
    // リストが空の場合
    if (*tail == NULL) {
        return;
    }

    BufferList *next = NULL;
    BufferList *now = *tail;

    // 最後尾のノードが除去の対象の場合
    if (now->buff == buff) {
        *tail = now->prev;
        free(now);
        return;
    }

    // 最後尾以外のノードが除去の対象の場合
    while (now != NULL) {
        if (now->buff == buff) {
            next->prev = now->prev;
            free(now);
            return;
        }
        next = now;
        now = now->prev;
    }
}

/**
 * @brief   BufferListのすべてのノードを除去し、それらのBufferをunpinする
 * @param   tail Buffer Listの末尾のポインタのポインタ
 * @param   bm Buffer Managerのポインタ
 * @return
 * @note
 * BufferListの持つノードのメモリをすべて開放し、
 * tailの指すポインタの値をNULLとする。
 * @attention
 */
void remove_all_BufferList(BufferList **tail, BufferManager *bm) {
    BufferList *next;
    BufferList *now = *tail;
    while (now != NULL) {
        bm_unpin(bm, now->buff);
        next = now;
        now = now->prev;
        free(next);
    }
    *tail = NULL;
}