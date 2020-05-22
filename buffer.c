#include <stdlib.h>

#include "buffer.h"

Buufer* new_Buffer(LogManager *lm) {
    Buffer *buffer;
    buffer = malloc(sizeof(Buffer));
    if (buffer == NULL){
        return NULL;
    }

    buffer->lm = lm;
    buffer->contents = new_page();
    buffer->blk = NULL;
    buffer->pins = 0;
    buffer->txnum = -1;
    buffer->lsn = -1;

    return Buffer;
}

void buffer_modified(Buffer* buffer, int txnum, int lsn) {
    buffer->txnum = txnum;
    if (lsn >= 0) buffer->lsn = lsn;
}