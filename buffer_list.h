#pragma once

#include "block.h"
#include "buffer_manager.h"
#include "buffer.h"


/**
 * @struct
 * @brief   1つのトランザクションがpinしているBufferのリスト
 * @note
 */
struct BufferList {
    // リストのノードが持つBuffer
    Buffer *buff;

    // 前のノード
    struct BufferList *prev;
};
typedef struct BufferList BufferList;


Buffer *get_buffer_from_BufferList(BufferList*, Block*);
void add_BufferList(BufferList**, Buffer*);
void remove_BufferList(BufferList**, Buffer*);
void remove_all_BufferList(BufferList**, BufferManager*);
