#pragma once

#include "buffer.h"

/**
 * @struct  BufferManager
 * @brief   BufferPoolを管理する構造体
 * @note
 */
typedef struct {
    // データベースが扱うBufferの集合
    Buffer *buffer_pool;

    // BufferPoolのBufferの個数
    int num_buffs;

    // BufferPoolのunpin状態のBufferの個数
    int available_buffs;

    // 指定のBlockとしてBufferをpin状態にする時の最大待ち時間
    // ?
    unsigned int max_wating_time;
} BufferManager;

BufferManager *new_BufferManager(FileManager*, LogManager*, int, unsigned int);
void bm_flush_all(BufferManager*, int);
void bm_unpin(BufferManager*, Buffer*);
Buffer* bm_pin(BufferManager*, Block*);