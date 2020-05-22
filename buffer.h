#ifndef BUFFER_H
#define BUFFER_H

#include "file_manager.h"
#include "log_manager.h"

typedef struct Buffer Buffer;
struct Buffer {
    LogManager *lm;
    Page *contents;
    Block *blk;
    int pins;
    int txnum;
    int lsn;
}

Buffer* new_Buffer(LogManager*);
void buffer_modified(Buffer*, int, int);

#endif