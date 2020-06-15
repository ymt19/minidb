#pragma once

#include "buffer.h"

typedef struct {
    Buffer *buffer_pool;
    int available_buffs;
    int max_wating_time;       // second
} BufferManager;

BufferManager *new_BufferManager(FileManager*, LogManager*, int);
void bm_flush_all(int);
void bm_unpin(BufferManager*, Buffer*);