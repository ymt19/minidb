#include <stdlib.h>
#include <time.h>
#include "buffer_manager.h"

static int waiting_too_long(BufferManager*, struct timespec);
static Buffer* bm_try_to_pin(BufferManager*, Block*);
static Buffer* bm_find_same_buffer(BufferManager*, Block*);
static Buffer* bm_find_unpinned_buffer(BufferManager*);


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
    bm->max_wating_time = 10;           // 10sec
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
 * BufferPoolのBufferを指定のBlockでpinする。
 * 失敗するとNULL
 */
Buffer* bm_pin(BufferManager *bm, Block *blk) {
    Buffer buff;
    struct timespec start_time;

    // 現在時間の測定
    clock_gettime(CLOCK_REALTIME, &start_time);

    buff = bm_try_to_pin(bm, blk);
    while (buff == NULL && waiting_too_long(bm, start_time)) {
        buff = bm_try_to_pin(bm, blk);
    }

    return NULL;
}

/**
 * 時間制限の確認をする。
 */
int waiting_too_long(BufferManager *bm, struct timespec start_time) {
    struct timespec end_time;
    unsigned int waiting_time;

    // 現在時間の測定
    clock_gettime(CLOCK_REALTIME, &end_time);

    // 秒単位による待ち時間の計測
    waiting_time = (unsigned int)(end_time.tv_sec - start_time.tv_sec);

    if (wating_time > bm->max_wating_time) {
        return 1;
    }
    return 0;
}

/**
 * BufferPoolからBlockを割り当てるBufferを探す。
 */
Buffer* bm_try_to_pin(BufferManager *bm, Block *blk) {
    Buffer *buff;

    // 同じblockを持つbufferがあるか確認する
    buff = bm_find_same_buffer(bm, blk);

    // BufferPoolからunpinnedなbufferを探す
    if (buff == NULL) {
        buff = bm_find_unpinned_buffer(bm);
        if (buff == NULL)
            return NULL;
        buffer_assign_to_block(buff, blk);
    }

    if (!buffer_is_pinned(buff))
        bm->available_buffs--;

    buffer_pin(buff);

    return buff;       
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