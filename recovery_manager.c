#include <stdlib.h>
#include <stdio.h>
#include "recovery_manager.h"
#include "log.h"
#include "transaction.h"
#include "int_list.h"

static void rollback(Transaction*, FileManager*, LogManager*);
static void recover(Transaction*, FileManager*, LogManager*);

/**
 * @brief   START log recordを発行する
 * @param   tx recordを発行するトランザクション
 * @return
 * @note    
 * 
 * @attention
 */
void rm_start(Transaction *tx) {
    write_start_log(tx->lm, tx->txnum);
}

/**
 * @brief   COMMIT log recordを発行する
 * @param   tx recordを発行するトランザクション
 * @return
 * @note
 * record発行前に、トランザクションの変更した値をディスクに書き込む。
 * record発行後に、そのrecordまでのlogをディスクに書き込む。
 * @attention
 */
void rm_commit(Transaction *tx) {
    bm_flush_all(tx->bm, tx->txnum); // tx->bufflistを使う
    int lsn = write_commit_log(tx->lm, tx->txnum);
    lm_flush_log_to_lsn(tx->lm, lsn);
}

/**
 * @brief   トランザクションの変更をundoし、ROLLBACK log recordを発行する
 * @param   tx recordを発行するトランザクション
 * @return
 * @note
 * bufferの内容を変更する
 * @attention
 */
void rm_rollback(Transaction *tx) {
    rollback(tx, tx->fm, tx->lm);
    bm_flush_all(tx->bm, tx->txnum);  // tx->bufflistを使う
    int lsn = write_rollback_log(tx->lm, tx->txnum);
    lm_flush_log_to_lsn(tx->lm, lsn);
}

/**
 * @brief   未完了のトランザクションの変更をundoし、CHECKOINT log recordを発行する
 * @param   tx recovery transaction
 * @return
 * @note    Quiescent Checkpointingを利用。
 * @attention
 */
void rm_recover(Transaction *tx) {
    recover(tx, tx->fm, tx->lm);
    bm_flush_all(tx->bm, tx->txnum);
    int lsn = write_checkpoint_log(tx->lm);
    lm_flush_log_to_lsn(tx->lm, lsn);
}

/**
 * @brief   SETINT log recordを発行する
 * @param   tx recordを発行するトランザクション
 * @param   buff txが変更する数値があるbuffer
 * @param   offset txが変更する数値があるbufferのoffset
 * @return  発行したSETINT log recordのlsn
 * @note
 * recordを発行するのみ。
 * @attention
 */
int rm_set_int(Transaction *tx, Buffer *buff, int offset) {
    int oldval = get_int_from_page(buff->page, offset);
    Block *blk = buff->blk;
    return write_set_int_log(tx->lm, tx->txnum, blk, offset, oldval);
}

/**
 * @brief   SETSTRING log recordを発行する
 * @param   tx recordを発行するトランザクション
 * @param   buff txが変更する数値があるbuffer
 * @param   offset txが変更する数値があるbufferのoffset 
 * @return
 * @note
 * recordを発行するのみ。
 * @attention 
 * newvalのサイズ管理
 * set前のサイズより大きなnewvalの場合の処理
 */
int rm_set_string(Transaction *tx, Buffer *buff, int offset) {
    char oldval[MAX_STRING_SIZE + 1];
    int val_size = get_string_from_page(buff->page, offset, oldval);
    Block *blk = buff->blk;
    return write_set_string_log(tx->lm, tx->txnum, blk, offset, oldval, val_size);
}

/**
 * @brief   指定されたトランザクションをrollbackする
 * @param   tx rollbackするトランザクション
 * @param   fm file manager
 * @param   lm log manager
 * @return
 * @note
 * log recordの調査に失敗した場合、プログラムを終了する。
 * @attention
 */
static void rollback(Transaction *tx, FileManager *fm, LogManager *lm) {
    Block *blk = new_block(lm->log_filename, lm->current_blk->blk_number);
    Page *page = new_page_bytes(lm->log_page->data, lm->log_page->size);
    int offset;     // 着目しているlog recordのoffset

    char record[MAX_RECORD_SIZE+1]; // 着目しているrecord
    int record_size;                // 着目しているrecordのサイズ
    LogType record_type;            // 着目しているrecordの種類
    int record_txnum;               // 着目しているrecordのtxnum

    blk->blk_number--;  // 次に読み込むblockの情報に変更
    offset = get_int_from_page(page, 0);

    // 作成された順にlog file内の全てのlog recordをスキャンする
    while (!(offset == fm->data_size && blk->blk_number < 0)) {
        if (offset >= fm->data_size) { // 1ブロック分を読み終わった場合
            if (fm_read(fm, blk, page) == 0) {
                fprintf(stderr, "error: fm_read()\n");
                exit(1);
            }
            blk->blk_number--;  // 次に読み込むblockの情報に変更
            offset = get_int_from_page(page, 0);
        }

        record_size = get_string_from_page(page, offset, record);
        record_type = search_log_type(record, record_size);
        if (record_type == ERROR_LOG) {     // log typeの調査に失敗した場合
            fprintf(stderr, "Log investigation failed.\n");
            exit(1);
        }
        record_txnum = search_log_txnum(record, record_size);
        if (record_txnum == ERROR_LOG) {     // logのtxnumの調査に失敗した場合
            fprintf(stderr, "Log investigation failed.\n");
            exit(1);
        }

        // 指定のトランザクションのstart log recordが見つかるまでスキャンする
        // それまで、そのトランザクションのlog recordをundoする
        if (tx->txnum == record_txnum) {
            if (record_type == START_LOG) {
                free(blk);
                free(page);
                return;
            }
            undo(tx, record, record_size);
        }

        // offsetの更新
        // record本体のサイズとそのサイズを表す数値がpage情で1つのlog recordを表す
        offset += record_size + sizeof(int);
    }
    
}

/**
 * buffer poolが足りなくなる場合は想定しない
 */
static void recover(Transaction *tx, FileManager *fm, LogManager *lm) {
    Block *blk = new_block(lm->log_filename, lm->current_blk->blk_number);
    Page *page = new_page_bytes(lm->log_page->data, lm->log_page->size);
    int offset;     // 着目しているlog recordのoffset

    char record[MAX_RECORD_SIZE+1]; // 着目しているrecord
    int record_size;                // 着目しているrecordのサイズ
    LogType record_type;            // 着目しているrecordの種類
    int record_txnum;               // 着目しているrecordのtxnum

    // 終了した(commit or rollback)トランザクションのtxnumのlistの末尾
    IntList *finished_txs = NULL;

    blk->blk_number--;  // 次に読み込むblockの情報に変更
    offset = get_int_from_page(page, 0);

    // 作成された順にlog file内の全てのlog recordをスキャンする
    while (!(offset == fm->data_size && blk->blk_number < 0)) {
        if (offset >= fm->data_size) { // 1ブロック分を読み終わった場合
            if (fm_read(fm, blk, page) == 0) {
                fprintf(stderr, "error: fm_read()\n");
                exit(1);
            }
            blk->blk_number--;  // 次に読み込むblockの情報に変更
            offset = get_int_from_page(page, 0);
        }

        record_size = get_string_from_page(page, offset, record);
        record_type = search_log_type(record, record_size);
        if (record_type == ERROR_LOG) {     // log typeの調査に失敗した場合
            fprintf(stderr, "Log investigation failed.\n");
            exit(1);
        }
        record_txnum = search_log_txnum(record, record_size);
        if (record_txnum == ERROR_LOG) {     // logのtxnumの調査に失敗した場合
            fprintf(stderr, "Log investigation failed.\n");
            exit(1);
        }


        if (record_type == CHECKPOINT_LOG) {
            // checkpoint recordを発見したらrecoverは終了
            free(blk);
            free(page);
            return;
        } else if (record_type == COMMIT_LOG || record_type == ROLLBACK_LOG) {
            // commit record または rollback recordを発見したら、
            // そのトランザクションは終了している
            finished_txs = add_IntList(finished_txs, record_txnum);
        } else if (!search_IntList(finished_txs, record_txnum)) {
            // 未だ終了していないトランザクションをみつけたら発見したら、
            // そのlog recordをundoする
            // start/ set int/ set string
            undo(tx, record, record_size);
        }

        // offsetの更新
        // record本体のサイズとそのサイズを表す数値がpage情で1つのlog recordを表す
        offset += record_size + sizeof(int);
    }

    // checkpoint recordが発行されていない場合、全てのlogを調査する
    free(blk);
    free(page);
    free_all_IntList(&finished_txs);
}
