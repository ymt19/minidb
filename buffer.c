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