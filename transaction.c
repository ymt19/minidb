#include <stdlib.h>
#include <stdio.h>
#include "transaction.h"
#include "recovery_manager.h"

static int get_next_txnum();

static int next_txnum = 0;  // 発行した中で最新のtxnum

Transaction *new_Transaction(FileManager *fm, LogManager *lm, BufferManager *bm) {
    Transaction *tx;

    tx = malloc(sizeof(Transaction));
    tx->fm = fm;
    tx->lm = lm;
    tx->bm = bm;
    tx->txnum = get_next_txnum();
    tx->bufflist = NULL;
    rm_start(tx);   // lmとtxnumを定義済みのため呼び出し可能
    /**
     * concurrency managerの追加
     */
    return tx;
}

void tx_commit(Transaction *tx) {
    rm_commit(tx);
    /**
     * concurrency manager release lockを解除
     */
    remove_all_BufferList(&(tx->bufflist), tx->bm);
    /**
     * free(tx);
     */
}

void tx_rollback(Transaction *tx) {
    rm_rollback(tx);
    /**
     * concurrency manager release
     */
    remove_all_BufferList(&(tx->bufflist), tx->bm);
    /**
     * free(tx);
     */
}

/**
 * どの状況で呼ばれるか
 */
void tx_recover(Transaction *tx) {
    bm_flush_all(tx->bm, tx->txnum); // どの状況で呼ばれるか
    rm_recover(tx);
}

/**
 * @todo 名称
 */
void tx_pin(Transaction *tx, Block *blk) {
    // bm_pin()
    // tx->bufflist = pin(tx->bufflist, tx->bm, blk); // bufflist add

    Buffer *buff = bm_pin(tx->bm, blk);
    add_BufferList(&(tx->bufflist), buff);
}

void tx_unpin(Transaction *tx, Block *blk) {
    // tx->bufflist = unpin(tx->bufflist, tx->bm, blk);

    Buffer *buff = get_buffer_from_BufferList(tx->bufflist, blk);
    bm_unpin(tx->bm, buff);
    remove_BufferList(&(tx->bufflist), buff);
}

int tx_get_int(Transaction *tx, Block *blk, int offset) {
    /**
     * concurrency manager slock(blk)
     */
    Buffer *buff = get_buffer_from_BufferList(tx->bufflist, blk);
    if (buff == NULL) {
        /**
         * buffer listにbufferが無い場合
         */
    }
    int val = get_int_from_page(buff->page, offset);
    return val;
}

int tx_get_string(Transaction *tx, Block *blk, int offset, char *val) {
    /**
     * concurrency manager slock(blk)
     */
    Buffer *buff = get_buffer_from_BufferList(tx->bufflist, blk);
    if (buff == NULL) {
        /**
         * buffer listにbufferが無い場合
         */
    }
    int size = get_string_from_page(buff->page, offset, val);
    return size;
}

/**
 * is_rollback <= write_to_log
 */
void tx_set_int(Transaction *tx, Block *blk, int offset, int val, int is_rollback_or_recover) {
    /**
     * concurrency manager xlock(blk)
     */
    Buffer *buff = get_buffer_from_BufferList(tx->bufflist, blk);
    if (buff == NULL) {
        /**
         * buffer listにbufferが無い場合
         */
    }
    int lsn = -1;
    if (!is_rollback_or_recover) {
        lsn = rm_set_int(tx, buff, offset);
    }
    set_int_to_page(buff->page, offset, val);
    buffer_modified(buff, tx->txnum, lsn);
}

void tx_set_string(Transaction *tx, Block *blk, int offset, char *val, int val_size, int is_rollback_or_recover) {
    /**
     * concurrency manager xlock(blk)
     */
    Buffer *buff = get_buffer_from_BufferList(tx->bufflist, blk);
    if (buff == NULL) {
        /**
         * buffer listにbufferが無い場合
         */
    }
    int lsn = -1;
    if (!is_rollback_or_recover) {
        lsn = rm_set_string(tx, buff, offset);
    }
    set_string_to_page(buff->page, offset, val, val_size);
    buffer_modified(buff, tx->txnum, lsn);
}

int tx_filesize(Transaction *tx, char *filename, int filename_size) {
    /**
     * Block *dummy = new_block(filename, -1);
     * concurrency manager slock(dummy)
     */
    int size = fm_file_size(tx->fm, filename);
    return size;
}

Block *tx_append_blk(Transaction *tx, char *filename, int filename_size) {
    /**
     * Block *dummy = new_block(filename, -1);
     * concurrency manager xlock(dummy)
     */
    Block *newblk = fm_append_newblk(tx->fm, filename);
    return newblk;
}

static int get_next_txnum() {
    next_txnum++;
    return next_txnum;
}