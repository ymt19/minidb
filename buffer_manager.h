#pragma once

#include "buffer.h"

typedef struct {
    Buffer *buffer_pool;
    int available_buffs;
    int MAX_TIME;       // second
} BufferManager;

BufferManager *new_BufferManager(FileManager*, LogManager*, int);
