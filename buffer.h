#pragma once

#include "file_manager.h"
#include "log_manager.h"

typedef struct {
    FileManager *fm;
    LogManager *lm;
    Page *page;
    Block *blk;
    int pins;
    int txnum;
    int lsn;
} Buffer;

Buffer* new_Buffer(FileManager*, LogManager*);
void buffer_modified(Buffer*, int, int);
int buffer_is_pinned(Buffer*);
void buffer_flush(Buffer*);
void buffer_pin(Buffer*);
void buffer_unpin(Buffer*);