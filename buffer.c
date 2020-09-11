#include <stdlib.h>
#include "buffer.h"

/**
 * @brief   構造体Bufferを指定した数のメモリ領域を確保する。
 * @param   (fm) FileManager
 * @param   (lm) LogManager
 * @param   (num_buffs) 確保するBufferの個数
 * @return  成功したら、確保したメモリ領域
 * @return  失敗したら、NULL
 * @note
 * 指定した個数のBufferを連続するメモリ領域に確保される。
 * @attention
 * 呼び出した関数は、指定した個数を記録しておく必要がある。
 */
Buffer *new_Buffers(FileManager *fm, LogManager *lm, int num_buffs) {
    int i;
    Buffer *buffs;

    buffs = malloc(sizeof(Buffer) * num_buffs);
    if (buffs == NULL){
        return NULL;
    }

    for (i = 0; i < num_buffs; i++) {
        buffs[i]->fm = fm;
        buffs[i]->lm = lm;
        buffs[i]->page = new_page(fm->blksize);
        buffs[i]->blk = NULL;
        buffs[i]->pins = 0;
        buffs[i]->txnum = -1;
        buffs[i]->lsn = -1;
    }
    return buffs;
}

/**
 * @brief   BufferのPageの内容を変更したときの、Bufferの情報の
 *          更新をする
 * @param   (buff) Buffer
 * @param   (txnum) BufferのPageを変更したtxnum
 * @param   (lsn) BufferのPageを変更したときに作成されるLSN
 * @return
 * @note
 * BufferのPageの内容を変更したとき、log recordを作成し、Bufferの
 * 情報を更新する必要がある。この関数は、そのBufferの情報を更新
 * するものである。
 * @attention
 */
void buffer_modified(Buffer *buff, int txnum, int lsn) {
    buff->txnum = txnum;
    buff->lsn = lsn;
    // if (lsn >= 0) buff->lsn = lsn;
}

/**
 * @brief   Bufferがpin状態であるかを確認する
 * @param   (buff) Buffer
 * @return  pin状態であれば、1
 * @return  pin状態でないならば、0
 * @note
 * 
 * @attention
 */
int buffer_is_pinned(Buffer *buff) {
    if (buff->pins > 0) {
        return 1;
    }

    return 0;
}

/**
 * @brief   BufferにBlockを割り当てる
 * @param   (buff) 割り当てられるBuffer
 * @param   (blk) 割り当てるBlock
 * @return
 * @note
 * BufferにBlockが割り当てられる前に、そのBufferをflushすることで
 * 以前のBufferの情報はfileに保存される。
 * @attention
 */
void buffer_assign_to_block(Buffer *buff, Block *blk) {
    // 以前のBufferの情報をfileに保存される
    buffer_flush(buff);

    // Blockを割り当てる
    buff->blk = blk;

    // BufferとなるPageに割り当てるBlockを読み込む
    fm_read(buff->fm, buff->blk, buff->page);

    buffer->pins = 0;
}

/**
 * @brief   Bufferの内容をfileにflushする
 * @param   (buff) flushされるBuffer
 * @return
 * @note
 * txnumを確認し、bufferの内容がtxによって変更されている場合のみ、
 * bufferをflushする、つまり、bufferが管理するPageをファイルに書き
 * 込む。
 * @attention
 * bufferのlsnが更新されている必要があるため、buffer_modified()を
 * この関数の前に呼び出す必要がある。
 */
void buffer_flush(Buffer *buff) {
    // bufferの内容がtxによって変更されている場合
    if (buff->txnum >= 0) {
        // lsnまでのlogをlog fileに書き込む
        lm_flush_log_to_lsn(buff->lm, buff->lsn);

        // bufferに相当するPageをfileに書き込む
        fm_write(buff->fm, buff->blk, buff->page);

        // txnumを初期化する
        buff->txnum = -1;
    }
}

/**
 * @brief   Bufferをpin状態にする
 * @param   (buffer) pin状態にするBuffer
 * @return
 * @note
 * 
 * @attention
 */
void buffer_pin(Buffer *buffer) {
    buffer->pins++;
}

/**
 * @brief   Bufferをunpin状態にする
 * @param   (buffer) unpin状態にするBuffer
 * @return
 * @note
 * 
 * @attention
 */
void buffer_unpin(Buffer *buffer) {
    buffer->pins--;
}