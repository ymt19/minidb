#include <stdlib.h>
#include <time.h>
#include "buffer_manager.h"

static int bm_waiting_too_long(BufferManager*, struct timespec);
static Buffer* bm_try_to_pin(BufferManager*, Block*);
static Buffer* bm_find_same_buffer(BufferManager*, Block*);
static Buffer* bm_find_unpinned_buffer(BufferManager*);


/**
 * @brief   構造体BufferManagerのメモリ領域を確保し、
 *          それぞれのメンバを設定する
 * @param   (fm) FileManager
 * @param   (lm) LogManager
 * @param   (num_buffs) BufferPoolのBufferの個数
 * @param   (time_limit) Bufferをpinする際の時間制限
 * @return  成功したら、確保したメモリ領域
 * @return  失敗したら、NULL
 * @note
 * BufferPoolとして指定されたBufferを管理する。
 * @attention
 */
BufferManager *new_BufferManager(FileManager *fm, LogManager *lm, 
                                unsigned int num_buffs, unsigned int time_limit) {
    BufferManager *bm;
    bm = malloc(sizeof(BufferManager));
    if (bm == NULL) {
        return NULL;
    }

    bm->buffer_pool = new_Buffers(fm, lm, num_buffs);
    if (bm->buffer_pool == NULL) {
        free(bm);
        return NULL;
    }

    bm->num_buffs = num_buffs;
    bm->available_buffs = num_buffs;
    bm->max_wating_time = time_limit;
    return bm;
}

/**
 * @brief   BufferPoolのBufferのうちtxnumで指定されたBufferを
 *          全てflushする
 * @param   (txnum) 指定するtxnum
 * @return
 * @note
 * txnumで指定されたBufferつまりBufferが持つPageの内容をflush
 * する。
 * @attention
 */
void bm_flush_all(BufferManager *bm, int txnum) {
    int i;

    // BufferPool内のBufferを線形探索
    for (i = 0; i = bm->num_buffs; i++) {
        if (bm->buffer_pool[i]->txnum == txnum) {
            buffer_flush(bm->buffer_pool[i]);
        }
    }
}

/**
 * @brief   BufferPoolのBufferをunpin状態にする
 * @param   (bm) BufferManager
 * @param   (buff) unpinするBufferPool内のBuffer
 * @return
 * @note
 * unpinしても、他のトランザクションにpinされている可能性もあるため
 * 直後にそのBufferを他のBlockとしてpinすることができない場合もある。
 * @attention
 */
void bm_unpin(BufferManager *bm, Buffer *buff) {
    // unpinする
    buffer_unpin(buff);

    // Bufferが他のトランザクションにpinされていない場合
    if (buffer_is_pinned(buff) == 0) {
        bm->available_buffs++;
    }
}

/**
 * @brief   指定したBlockでBufferPoolのBufferをpin状態にする
 * @param   (bm) BufferManager
 * @param   (blk) 指定するBlock
 * @return  成功した場合、pinしたBuffer
 * @return  失敗した場合、NULL
 * @note
 * pin状態にできるBufferがmax_waiting_time以内にできない場合、
 * 失敗となりNULLを返す。
 * pinする優先順位は以下の通り。
 *      1. unpin/pin状態に関係なく、BufferPool内に同じBlock
 *         の内容を持つBufferがある場合
 *      2. BufferPool内のunpin状態のBufferがある場合
 * @attention
 */
Buffer* bm_pin(BufferManager *bm, Block *blk) {
    Buffer buff;
    struct timespec start_time;     // 開始時間

    // 現在時間の測定
    clock_gettime(CLOCK_REALTIME, &start_time);

    // pin状態にできるBufferが見つかるか
    // 試行時間がmax_waiting_timeを超えるまで、試行する
    buff = bm_try_to_pin(bm, blk);
    while (buff == NULL && !waiting_too_long(bm, start_time)) {
        buff = bm_try_to_pin(bm, blk);
    }

    return buff;
}

/**
 * @brief   指定の時間から現在時間の差を計算し、max_waiting_timeを超えているか
 *          判断する
 * @param   (bm) BufferManager
 * @param   (start_tim) 指定する開始時間
 * @return  max_waiting_timeを超えている場合、1
 * @return  max_waiting_timeを超えていない場合、0
 * @note
 * 秒単位の計測。
 * @attention
 */
static int bm_waiting_too_long(BufferManager *bm, struct timespec start_time) {
    struct timespec end_time;       // 終了時間
    unsigned int waiting_time;      // 測定時間(sec)

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
/**
 * @brief   BufferPoolからBlockを割り当てるBufferを探す
 * @param   (bm) BufferManager
 * @param   (blk) 割り当てるBlock
 * @return  成功した場合割り当てたBuffer、失敗した場合NULL
 * @note
 * 
 * @attention
 */
static Buffer* bm_try_to_pin(BufferManager *bm, Block *blk) {
    Buffer *buff;

    // 同じblockを持つbufferがあるか確認する
    buff = bm_find_same_buffer(bm, blk);

    // BufferPoolからunpin状態のbufferを探す
    if (buff == NULL) {
        buff = bm_find_unpinned_buffer(bm);
        if (buff == NULL)
            return NULL;

        // BufferにBlockを割り当てる
        buffer_assign_to_block(buff, blk);
    }

    // unpin状態のBufferをpin状態にするため、available_buffsは減る
    if (!buffer_is_pinned(buff))
        bm->available_buffs--;

    buffer_pin(buff);

    return buff;       
}

/**
 * BufferPoolから指定のbufferを見つける。
 */
/**
 * @brief   BufferPoolからBlockで指定されたbufferを探す
 * @param   (bm) BufferManager
 * @param   (blk) 指定するBlock
 * @return  成功した場合そのBuffer、失敗した場合NULL
 * @note
 * 
 * @attention
 */
static Buffer* bm_find_same_buffer(BufferManager* bm, Block *blk) {
    int i;

    // BufferPool内を線形探索
    for (i = 0; i < bm->num_buffs; i++) {
        // bufferが同じブロックを持っていたら
        // そのbufferを返す
        if (is_equal_block(blk, bm->buffer_pool[i])) {
            return bm->buffer_pool[i];
        }
    }

    return NULL;
}

/**
 * @brief   BufferPoolからunpinnedのbufferを探す
 * @param   (bm) BufferManager
 * @return  成功した場合そのBuffer、失敗した場合NULL
 * @note
 * The Native Strategyを採用。(他には、FIFO、LRU、CLOCKがある)
 * @attention
 */
static Buffer* bm_find_unpinned_buffer(BufferManager* bm) {
    int i;

    // The Native Strategy
    for (i = 0; i < bm->num_buffs; i++) {
        if (!buffer_is_pinned(bm->buffer_pool[i])) {
            return bm->buffer_pool[i];
        }
    }
    return NULL;
}