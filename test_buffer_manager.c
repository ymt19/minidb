#include <assert.h>
#include <string.h>
#include <stdio.h>
#include "buffer_manager.h"

int main(void) {
    FileManager *fm;
    LogManager *lm;
    BufferManager *bm;

    Block *blk[6] = {NULL};
    Buffer *buffs[7] = {NULL};

    fm = new_FileManager("tmp_test_buffer_manager", 400);
    lm = new_LogManager(fm, "logfile");
    // BufferPoolとしてBuffer3つ確保
    // pin要求の時間制限を1秒とする
    bm = new_BufferManager(fm, lm, 3, 1);

    // 3つのbufferをpin状態にする
    blk[0] = new_block("test", 0);
    buffs[0] = bm_pin(bm, blk[0]);
    assert(buffs[0] != NULL);
    assert(bm->available_buffs == 2);

    blk[1] = new_block("test", 1);
    buffs[1] = bm_pin(bm, blk[1]);
    assert(buffs[1] != NULL);
    assert(bm->available_buffs == 1);

    blk[2] = new_block("test", 2);
    buffs[2] = bm_pin(bm, blk[2]);
    assert(buffs[2] != NULL);
    assert(bm->available_buffs == 0);
    /**
     * BufferPool[0] : blk[0] (buffs[0])
     * BufferPool[1] : blk[1] (buffs[1])
     * BufferPool[2] : blk[2] (buffs[2])
     */

    // 3つのBufferPoolに4つのpinを要求しても、Bufferをpinできない
    blk[3] = new_block("test", 3);
    buffs[3] = bm_pin(bm, blk[3]);
    assert(buffs[3] == NULL);
    /**
     * 現在
     * BufferPool[0] : blk[0] (buffs[0])
     * BufferPool[1] : blk[1] (buffs[1])
     * BufferPool[2] : blk[2] (buffs[2])
     */

    // BufferPoolにunpin状態のBufferが無い場合、pin状態のBufferをunpin
    // すれば、他のBlockとしてpinできる。
    // blk[1]（buffs[1]）のBufferをunpinし、blk[3]をpinする
    bm_unpin(bm, buffs[1]);
    assert(bm->available_buffs == 1);
    buffs[3] = bm_pin(bm, blk[3]);
    assert(buffs[3] != NULL);
    assert(bm->available_buffs == 0);
    /**
     * 現在
     * BufferPool[0] : blk[0] (buffs[0])
     * BufferPool[1] : blk[3] (buffs[3])
     * BufferPool[2] : blk[2] (buffs[2])
     */

    // BufferPoolにunpin状態のBufferが無い場合、同じBlockを扱うpin状態の
    // Bufferがあればpinすることができる。
    // buffs[2]と同じBlock（blk[2]）をpinする
    buffs[4] = bm_pin(bm, blk[2]);
    assert(buffs[4] == buffs[2]);
    /**
     * 現在
     * BufferPool[0] : blk[0] (buffs[0])
     * BufferPool[1] : blk[3] (buffs[3])
     * BufferPool[2] : blk[2] (buffs[2], buffs[4])
     */

    // 書き込み
    {
        /**
         * 現在
         * BufferPool[0] : blk[0] (buffs[0])
         * BufferPool[1] : blk[3] (buffs[3])
         * BufferPool[2] : blk[2] (buffs[2], buffs[4])
         */

        // 書き込み
        // buffs[0]において、blk[0]のpos1にnum1を書き込む
        int pos0 = 80;
        int set_num = 10, get_num;
        int txnum0 = 1;
        int lsn0;
        char log_record0[20];

        set_int_to_page(buffs[0]->page, pos0, set_num);                     // page変更(buffer変更)
        sprintf(log_record0, "record%05d", txnum0);                         // log作成
        lsn0 = lm_append_log(lm, log_record0, (int)strlen(log_record0));    // log追加
        buffer_modified(buffs[0], txnum0, lsn0);                            // buffer情報を加える
        bm_flush_all(bm, txnum0);                                           // この変更をファイルに書き込む

        // BufferPool[0]にblk[0]ではないBlockを読み込ませる
        bm_unpin(bm, buffs[0]);
        buffs[1] = bm_pin(bm, blk[1]);
        bm_unpin(bm, buffs[1]);
        /**
         * 現在
         * BufferPool[0] unpin  : blk[1] (buffs[1])
         * BufferPool[1] pin    : blk[3] (buffs[3])
         * BufferPool[2] pin    : blk[2] (buffs[2], buffs[4])
         */

        // blk[0]を読み込み
        buffs[5] = bm_pin(bm, blk[0]);
        get_num = get_int_from_page(buffs[5]->page, pos0);
        assert(get_num == set_num);

        /**
         * 現在
         * BufferPool[0] pin    : blk[0] (buffs[5]) 書き込みあり
         * BufferPool[1] pin    : blk[3] (buffs[3])
         * BufferPool[2] pin    : blk[2] (buffs[2], buffs[4])
         */
    }

    // 書き込み(Bufferをflushしない)
    {
        /**
         * 現在
         * BufferPool[0] pin    : blk[0] (buffs[5]) 書き込みあり
         * BufferPool[1] pin    : blk[3] (buffs[3])
         * BufferPool[2] pin    : blk[2] (buffs[2], buffs[4])
         */

        // buffs[3]において、blk[3]のpos3にnum3を書き込む、flushしない
        int pos3 = 100;
        int set_num = 20, get_num;
        int txnum3 = 2;
        int lsn3;
        char log_record3[20];
        set_int_to_page(buffs[3]->page, pos3, set_num);                        // page変更(buffer変更)
        sprintf(log_record3, "record%05d", txnum3);                         // log作成
        lsn3 = lm_append_log(lm, log_record3, (int)strlen(log_record3));    // log追加
        buffer_modified(buffs[3], txnum3, lsn3);                            // buffer情報を加える
        // bm_flush_all(bm, txnum3)を実行しないが、Bufferが他のBlockによってpinされたら、その時に
        // flushされる。

        // BufferPool[1]にblk[3]ではないBlockを読み込ませる
        bm_unpin(bm, buffs[3]);
        buffs[1] = bm_pin(bm, blk[1]);
        bm_unpin(bm, buffs[1]);

        /**
         * 現在
         * BufferPool[0] pin    : blk[0] (buffs[5]) 書き込みあり
         * BufferPool[1] unpin  : blk[1] (buffs[1])
         * BufferPool[2] pin    : blk[2] (buffs[2], buffs[4])
         */
        
        buffs[6] = bm_pin(bm, blk[3]);
        get_num = get_int_from_page(buffs[6]->page, pos3);
        assert(get_num == set_num);

        /**
         * 現在
         * BufferPool[0] pin    : blk[0] (buffs[5]) 書き込みあり
         * BufferPool[1] pin    : blk[3] (buffs[6])
         * BufferPool[2] pin    : blk[2] (buffs[2], buffs[4])
         */
    }
}