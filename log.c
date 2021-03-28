#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "log.h"
#include "log_manager.h"
#include "transaction.h"

static void undo_set_int_log(Transaction*, char*, int);
static void undo_set_string_log(Transaction*, char*, int);

int MAX_RECORD_SIZE =   sizeof(int)                     // SETSTRING_LOG(数値)
                        + sizeof(int)                   // txnum(数値)
                        + sizeof(int) + MAX_FILENAME    // ファイル名(文字列)
                        + sizeof(int)                   // blknum(数値)
                        + sizeof(int)                   // offset(数値)
                        + sizeof(int) + MAX_STRING_SIZE;// val(文字列)

/**
 * @brief   bufferにstart recordを書き込む
 * @param   lm LogManger
 * @param   txnum transaction number
 * @return  作成したrecordのlsn
 * @note
 * 形式: START_LOG(数値) txnum(数値)
 * @attention
 */
int write_start_log(LogManager *lm, int txnum) {
    int logtype = START_LOG;
    int txnum_pos = sizeof(int);
    int record_size = txnum_pos + sizeof(int);

    Page *record = new_page(record_size);
    set_int_to_page(record, 0, logtype);
    set_int_to_page(record, txnum_pos, txnum);

    int lsn = lm_append_log(lm, record->data, record_size);
    // printf("%-12s lsn:%d txnum:%d\n", "start", lsn, txnum);
    free(record);
    return lsn;
}

/**
 * @brief   bufferにcommit recordを書き込む
 * @param   lm LogManger
 * @param   txnum transaction number
 * @return  作成したrecordのlsn
 * @note
 * 形式: COMMIT_LOG(数値) txnum(数値)
 * @attention
 */
int write_commit_log(LogManager *lm, int txnum) {
    int txnum_pos = sizeof(int);
    int record_size = txnum_pos + sizeof(int);

    Page *record = new_page(record_size);
    set_int_to_page(record, 0, COMMIT_LOG);
    set_int_to_page(record, txnum_pos, txnum);

    int lsn = lm_append_log(lm, record->data, record_size);
    // printf("%-12s lsn:%d txnum:%d\n", "commit", lsn, txnum);
    free(record);
    return lsn;
}

/**
 * @brief   bufferにrollback recordを書き込む
 * @param   lm LogManger
 * @param   txnum transaction number
 * @return  作成したrecordのlsn
 * @note
 * 形式: ROLLBACK_LOG(数値) txnum(数値)
 * @attention
 */
int write_rollback_log(LogManager *lm, int txnum) {
    int txnum_pos = sizeof(int);
    int record_size = txnum_pos + sizeof(int);

    Page *record = new_page(record_size);
    set_int_to_page(record, 0, ROLLBACK_LOG);
    set_int_to_page(record, txnum_pos, txnum);

    int lsn = lm_append_log(lm, record->data, record_size);
    // printf("%-12s lsn:%d txnum:%d\n", "rollback", lsn, txnum);
    free(record);
    return lsn;
}

/**
 * @brief   bufferにset int recordを書き込む
 * @param   lm LogManger
 * @param   txnum transaction number
 * @param   blk トランザクションが変更する値が格納されているblock
 * @param   offset blockのoffset
 * @param   val 変更前の値
 * @return  作成したrecordのlsn
 * @note
 * 形式: SETINT_LOG(数値) txnum(数値) ファイル名(文字列) blknum(数値) offset(数値) val(数値) 
 * @attention
 */
int write_set_int_log(LogManager *lm, int txnum, Block *blk, int offset, int val) {
    int txnum_pos = sizeof(int);
    int filename_pos = txnum_pos + sizeof(int);
    int blknum_pos = filename_pos + sizeof(int) + strlen(blk->filename);
    int offset_pos = blknum_pos + sizeof(int);
    int val_pos = offset_pos + sizeof(int);
    int record_size = val_pos + sizeof(int);

    Page *record = new_page(record_size);
    set_int_to_page(record, 0, SETINT_LOG);
    set_int_to_page(record, txnum_pos, txnum);
    set_string_to_page(record, filename_pos, blk->filename, strlen(blk->filename));
    set_int_to_page(record, blknum_pos, blk->blk_number);
    set_int_to_page(record, offset_pos, offset);
    set_int_to_page(record, val_pos, val);

    int lsn = lm_append_log(lm, record->data, record_size);
    // printf("%-12s lsn:%d txnum:%d filename:%s blocknumber:%d offset:%d oldvalue:%d\n",
    //         "setint", lsn, txnum, blk->filename, blk->blk_number, offset, val);
    free(record);
    return lsn;
}

/**
 * @brief   bufferにset string recordを書き込む
 * @param   lm LogManger
 * @param   txnum transaction number
 * @param   blk トランザクションが変更する値が格納されているblock
 * @param   offset blockのoffset
 * @param   val 変更前の値
 * @param   val_size valのサイズ
 * @return  作成したrecordのlsn
 * @note
 * 形式: SETSTRING_LOG(数値) txnum(数値) ファイル名(文字列) blknum(数値) offset(数値) val(文字列)
 * @attention
 */
int write_set_string_log(LogManager *lm, int txnum, Block *blk, int offset, char *val, int val_size) {
    int txnum_pos = sizeof(int);
    int filename_pos = txnum_pos + sizeof(int);
    int blknum_pos = filename_pos + sizeof(int) + strlen(blk->filename);
    int offset_pos = blknum_pos + sizeof(int);
    int val_pos = offset_pos + sizeof(int);
    int record_size = val_pos + sizeof(int) + val_size;

    Page *record = new_page(record_size);
    set_int_to_page(record, 0, SETSTRING_LOG);
    set_int_to_page(record, txnum_pos, txnum);
    set_string_to_page(record, filename_pos, blk->filename, strlen(blk->filename));
    set_int_to_page(record, blknum_pos, blk->blk_number);
    set_int_to_page(record, offset_pos, offset);
    set_string_to_page(record, val_pos, val, val_size);

    int lsn = lm_append_log(lm, record->data, record_size);
    // printf("%-12s lsn:%d txnum:%d filename:%s blocknumber:%d offset:%d oldvalue:%s\n",
    //         "setstring", lsn, txnum, blk->filename, blk->blk_number, offset, val);
    free(record);
    return lsn;
}

/**
 * @brief   bufferにcheckpoint recordを書き込む
 * @param   lm LogManger
 * @return  作成したrecordのlsn
 * @note
 * Quiescent Checkpointを利用。
 * 形式: CHECKPOINT_LOG(数値)
 * @attention
 */
int write_checkpoint_log(LogManager *lm) {
    int record_size = sizeof(int);

    Page *record = new_page(record_size);
    set_int_to_page(record, 0, CHECKPOINT_LOG);

    int lsn = lm_append_log(lm, record->data, record_size);
    // printf("checkpoint lsn:%d\n", lsn);
    free(record);
    return lsn;
}

/**
 * @brief   log recordを指定されたトランザクションでundoする
 * @param   tx undoするトランザクション
 * @param   record undoするlog record
 * @param   record_size undoするlog recordのサイズ
 * @return
 * @note
 * この関数が実際にデータベースの値を書き換える
 * @attention
 */
void undo(Transaction *tx, char *record, int record_size) {
    switch (search_log_type(record, record_size)) {
        case START_LOG:
            // undoする情報がないため、何もしない
            return;
        case COMMIT_LOG:
            // undoする情報がないため、何もしない
            return;
        case ROLLBACK_LOG:
            // undoする情報がないため、何もしない
            return;
        case SETINT_LOG:
            undo_set_int_log(tx, record, record_size);
            return;
        case SETSTRING_LOG:
            undo_set_string_log(tx, record, record_size);
            return;
        case CHECKPOINT_LOG:
            // undoする情報がないため、何もしない
            return;
        case ERROR_LOG:
            return;
    }
}

/**
 * @brief   recordのlog typeを調べる
 * @param   record 調べるlog record
 * @return  log recordのLogType、それ以外の場合NULL
 * @note
 * 
 * @attention
 */
LogType search_log_type(char *record, int record_size) {
    if (record_size < sizeof(int)) {
        return ERROR_LOG;
    }
    LogType type;
    memcpy(&type, record, sizeof(int));
    return type;
}

/**
 * @brief   log recordに記録されているtxnumを調べる
 * @param   record 調べるlog record
 * @param   record_size 調べるlog recordのサイズ
 * @return  log reocrdに記録されたtxnum、txnumが記録されていない場合ERROR_LOG
 * @note
 * 
 * @attention
 */
int search_log_txnum(char *record, int record_size) {
    if (record_size < sizeof(int)*2) {
        return ERROR_LOG;
    }
    int txnum;
    memcpy(&txnum, record + sizeof(int), sizeof(int));
    return txnum;
}

/**
 * @brief   set int log recordをundo(値の変更を行う)する
 * @param   tx undoを実行するトランザクション
 * @param   record undoをするlog record
 * @param   record_size undoするlog recordのサイズ
 * @return
 * @note
 * 
 * @attention
 * log recordからset int log recordの情報が読み取れなかった場合、プログラム終了。
 */
static void undo_set_int_log(Transaction *tx, char *record, int record_size) {
    Page *page = new_page_bytes(record, record_size);

    // ファイル名(文字列) blknum(数値) offset(数値) val(数値) の情報のみ使用するため、
    // SETINT_LOG(数値) txnum(数値)は読み飛ばす
    int record_offset = sizeof(int) * 2;  // record内のoffset

    char filename[MAX_FILENAME + 1];
    int filename_size;
    int blk_num;
    int offset;
    int val;

    // ファイル名の取得
    filename_size = get_string_from_page(page, record_offset, filename);
    record_offset += filename_size + sizeof(int);
    // block numberの取得
    blk_num = get_int_from_page(page, record_offset);
    record_offset += sizeof(int);
    // offsetの取得
    offset = get_int_from_page(page, record_offset);
    record_offset += sizeof(int);
    // valの取得
    val = get_int_from_page(page, record_offset);
    record_offset += sizeof(int);

    if (record_offset > record_size) {  // recordの大きさが合わない場合
        free(page);
        fprintf(stderr, "error: undo set int log record.\n");
        exit(1);
    }

    Block *blk = new_block(filename, blk_num);
    tx_pin(tx, blk);
    tx_set_int(tx, blk, offset, val, 1); // logに記録しない
    tx_unpin(tx, blk);
    free(page);
}

/**
 * @brief   set string log recordをundo(値の変更を行う)する
 * @param   tx undoを実行するトランザクション
 * @param   record undoをするlog record
 * @param   record_size undoするlog recordのサイズ
 * @return
 * @note
 * 
 * @attention
 * log recordからset string log recordの情報が読み取れなかった場合、プログラム終了。
 */
static void undo_set_string_log(Transaction *tx, char *record, int record_size) {
    Page *page = new_page_bytes(record, record_size);

    // ファイル名(文字列) blknum(数値) offset(数値) val(文字列) の情報のみ使用するため、
    // SETINT_LOG(数値) txnum(数値)は読み飛ばす
    int record_offset = sizeof(int) * 2;  // record内のoffset

    char filename[MAX_FILENAME + 1];
    int filename_size;
    int blk_num;
    int offset;
    char val[MAX_STRING_SIZE + 1];
    int val_size;

    // ファイル名の取得
    filename_size = get_string_from_page(page, record_offset, filename);
    record_offset += filename_size + sizeof(int);
    // block numberの取得
    blk_num = get_int_from_page(page, record_offset);
    record_offset += sizeof(int);
    // offsetの取得
    offset = get_int_from_page(page, record_offset);
    record_offset += sizeof(int);
    // valの取得
    val_size = get_string_from_page(page, record_offset, val);
    record_offset += val_size + sizeof(int);

    if (record_offset > record_size) {  // recordの大きさが合わない場合
        free(page);
        fprintf(stderr, "error: undo set int log record.\n");
        exit(1);
    }

    Block *blk = new_block(filename, blk_num);
    tx_pin(tx, blk);
    tx_set_string(tx, blk, offset, val, val_size, 1); // logに記録しない
    tx_unpin(tx, blk);
    free(page);
}