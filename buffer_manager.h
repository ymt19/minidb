#pragma once

#include "buffer.h"

typedef struct {
    Buffer *buffer_pool;
    int available_buffs;
    unsigned int max_wating_time;       // 秒単位
} BufferManager;

BufferManager *new_BufferManager(FileManager*, LogManager*, int);
void bm_flush_all(int);
void bm_unpin(BufferManager*, Buffer*);
Buffer* bm_pin(BufferManager*, Block*)