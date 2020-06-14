#include <stdlib.h>
#include "buffer_manager.h"

static int wating_past_limit

BufferManager *new_BufferManager(FileManager *fm, LogManager *lm, int num_buffs) {
    BufferManager *bm;
    bm = malloc(sizeof(BufferManager));
    if (bm == NULL) {
        return NULL;
    }

    bm->buffer_pool = malloc(sizeof(Buffer) * num_buffs);
    if (bm->buffer_pool == NULL) {
        free(bm);
        return NULL;
    }

    bm->available_buffs = num_buffs;
    bm->MAX_TIME = 10000000;   // 10sec
    return bm;
}

void bm_unpin(BufferManager *bm, Buffer *buff) {
    buffer_unpin(buff);
    if (buffer_is_pinned(buff) == 0) {
        bm->available_buffs++;
    }
}
