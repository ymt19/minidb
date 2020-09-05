#include <stdlib.h>
#include "buffer.h"

Buffer *new_Buffer(FileManager *fm, LogManager *lm) {
    Buffer *buffer;
    buffer = malloc(sizeof(Buffer));
    if (buffer == NULL){
        return NULL;
    }

    buffer->fm = fm;
    buffer->lm = lm;
    buffer->page = new_page(fm->blksize);
    buffer->blk = NULL;
    buffer->pins = 0;
    buffer->txnum = -1;
    buffer->lsn = -1;
    return Buffer;
}

void buffer_modified(Buffer *buffer, int txnum, int lsn) {
    buffer->txnum = txnum;
    if (lsn >= 0) buffer->lsn = lsn;
}

int buffer_is_pinned(Buffer *buffer) {
    if (buffer->pins > 0) {
        return 1;
    }

    return 0;
}

void buffer_flush(Buffer *buffer) {
    if (buffer->txnum >= 0) {
        log_flush_to_lsn(buffer->lm, buffer->lsn);
        fm_write(buffer->fm, buffer->blk, buffer->page);
        buffer->txnum = -1;
    }
}

void buffer_pin(Buffer *buffer) {
    buffer->pins++;
}

void buffer_unpin(Buffer *buffer) {
    buffer->pins--;
}