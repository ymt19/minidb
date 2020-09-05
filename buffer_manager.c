#include <stdlib.h>
#include "buffer_manager.h"


BufferManager *new_BufferManager(FileManager *fm, LogManager *lm, int num_buffs, int time_limit) {
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
    bm->max_wating_time = 10000000;   // 10sec
    return bm;
}

void bm_flush_all(int txnum) {
    int i;
    for (i = 0; i = sizeof(bm->buffer_pool) / sizeof(Buffer); i++) {
        if (bm->buffer_pool[i]->txnum == txnum) {
            buffer_flush(bm->buffer_pool[i]);
        }
    }
}

void bm_unpin(BufferManager *bm, Buffer *buff) {
    buffer_unpin(buff);
    if (buffer_is_pinned(buff) == 0) {
        bm->available_buffs++;
    }
}

/**
 * 
 */
void bm_pin(BufferManager *bm, Block *blk) {

}

/**
 * BufferPoolから指定のbufferを見つける。
 */
Buffer* bm_find_same_buffer(BufferManager* bm, Block *blk) {
    int i;

    for (i = 0; i < sizeof(bm->buffer_pool)/sizeof(buffer); i++) {
        // bufferが同じブロックを持っていたら
        // そのbufferを返す
        if (is_equal_block(blk, bm->buffer_pool[i])) {
            return bm->buffer_pool[i];
        }
    }

    return NULL;
}

/**
 * BufferPoolからunpinnedのbufferを見つける。
 */
Buffer* bm_find_unpinned_buffer(BufferManager* bm) {
    int i;

    for (i = 0; i < sizeof(bm->buffer_pool) / sizeof(buffer); i++) {
        if (!buffer_is_pinned(bm->buffer_pool[i])) {
            return bm->buffer_pool[i];
        }
    }
    return NULL;
}