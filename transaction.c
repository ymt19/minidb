#include <stdlib.h>
#include <stdio.h>
#include "transaction.h"
#include "recovery_manager.h"

static int get_next_txnum();

static int next_txnum = 0;  // 発行した中で最新のtxnum

/**
 * @brief   構造体Transactionのメモリを動的確保し、そのstart logを発行する
 * @param   fm file manager
 * @param   lm log manager
 * @param   bm buffer manager
 * @return  確保した構造体Transactionを指すポインタ
 * @note
 */
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

/**
 * @brief   トランザクションをcommitする
 * @param   tx commitするトランザクション
 * @return
 * @note
 */
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

/**
 * @brief   トランザクションをrollbackする
 * @param   tx rollbackするトランザクション
 * @return
 * @note
 */
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
 * @brief   recoverする
 * @param   tx recover時に使用されるトランザクション
 * @return
 * @note
 * @todo
 * どのような状況でこの関数が呼ばれるのか
 */
void tx_recover(Transaction *tx) {
    bm_flush_all(tx->bm, tx->txnum); // どの状況で呼ばれるか
    rm_recover(tx);
}

/**
 * @brief   指定したトランザクションでblockをpinする
 * @param   tx pinするトランザクション
 * @param   blk pinされるブロック
 * @return
 * @note
 */
void tx_pin(Transaction *tx, Block *blk) {
    Buffer *buff = bm_pin(tx->bm, blk);
    add_BufferList(&(tx->bufflist), buff);
}

/**
 * @brief   指定したトランザクションでblockをunpinする
 * @param   tx unpinするトランザクション
 * @param   blk unpinされるブロック
 * @return
 * @note
 */
void tx_unpin(Transaction *tx, Block *blk) {
    Buffer *buff = get_buffer_from_BufferList(tx->bufflist, blk);
    bm_unpin(tx->bm, buff);
    remove_BufferList(&(tx->bufflist), buff);
}

/**
 * @brief   トランザクションによって指定の位置の数値を取得する
 * @param   tx 数値を取得するトランザクション
 * @param   blk 数値があるブロック
 * @param   offset 数値があるブロックのオフセット
 * @return  取得した数値
 * @note
 */
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

/**
 * @brief   指定のトランザクションによって指定の位置の文字列を取得する
 * @param   tx 文字列を取得するトランザクション
 * @param   blk 文字列があるブロック
 * @param   offset 文字列があるブロックのオフセット
 * @param   val 取得した文字列を格納するメモリを指すポインタ
 * @return  取得した文字列のサイズ
 * @note
 */
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
 * @brief   指定のトランザクションによって指定の位置に数値を書き込む
 * @param   tx 数値を書き込むトランザクション
 * @param   blk 数値を書き込むブロック
 * @param   offset 数値を書き込むブロックのオフセット
 * @param   val 書き込む数値
 * @param   is_rollback_recover この関数を呼び出すトランザクションがrollback/recoverによるものなら真
 * @return
 * @note
 * ファイルへの書き込みは保証しない。
 * is_rollback_recoverが真の場合、このset intによるlog recordは発行されない。
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

/**
 * @brief   指定のトランザクションによって指定の位置に文字列を書き込む
 * @param   tx 文字列を書き込むトランザクション
 * @param   blk 文字列を書き込むブロック
 * @param   offset 文字列を書き込むブロックのオフセット
 * @param   val 書き込む文字列
 * @param   val_size 書き込む文字列のサイズ
 * @param   is_rollback_recover この関数を呼び出すトランザクションがrollback/recoverによるものなら真
 * @return
 * @note
 * ファイルへの書き込みは保証しない。
 * is_rollback_recoverが真の場合、このset stringによるlog recordは発行されない。
 */
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

/**
 * @brief   
 * @param
 * @return
 * @note
 */
int tx_filesize(Transaction *tx, char *filename, int filename_size) {
    /**
     * Block *dummy = new_block(filename, -1);
     * concurrency manager slock(dummy)
     */
    int size = fm_file_size(tx->fm, filename);
    return size;
}

/**
 * @brief
 * @param
 * @return
 * @note
 */
Block *tx_append_blk(Transaction *tx, char *filename, int filename_size) {
    /**
     * Block *dummy = new_block(filename, -1);
     * concurrency manager xlock(dummy)
     */
    Block *newblk = fm_append_newblk(tx->fm, filename);
    return newblk;
}

/**
 * @brief
 * @param
 * @return
 * @note
 */
static int get_next_txnum() {
    next_txnum++;
    return next_txnum;
}