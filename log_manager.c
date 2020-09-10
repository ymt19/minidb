#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "log_manager.h"

static void lm_flush_log(LogManager*);
static Block* lm_append_newblk(LogManager*);

/**
 * @brief   構造体LogManagerのメモリ領域の確保し、
 *          それぞれのメンバを設定する。
 * @param   (fm) FileManager
 * @param   (log_filename) log fileの名前
 * @return 成功したら、確保したメモリ領域
 * @return 失敗したら、NULL
 * @note
 * 
 * @attention
 */
LogManager* new_LogManager(FileManager *fm, char *log_filename) {
    LogManager* lm;
    int log_file_size;      // logfile全体のデータサイズ
    
    // メモリ領域の確保
    if ((lm = malloc(sizeof(LogManager))) == NULL) {
        return NULL;
    }

    // FileManagerを記録
    lm->fm = fm;

    // logfile名を記録
    snprintf(lm->log_filename, MAX_FILENAME, "%s", log_filename);

    // log pageを確保する
    if ((lm->log_page = new_page(lm->fm->data_size)) == NULL) {
        return NULL;
    }

    log_file_size = fm_file_size(lm->fm, lm->log_filename);
    if (log_file_size == 0) {
        // log fileが存在しない場合
        // 新しいBlockをlogfileに追加し、それをcurrent_blkに設定する
        lm->current_blk = lm_append_newblk(lm);
    } else {
        // log fileが存在した場合
        // log fileの最後のBlockをcurrent_blkに設定する
        lm->current_blk = new_block(lm->log_filename, log_file_size - 1);
        // log fileからcurrent_blkの情報をlog_pageに読み込む
        fm_read(lm->fm, lm->current_blk, lm->log_page);
    }

    // Log Manager起動時に、LSN = 0とする
    lm->latest_LSN = 0;
    lm->last_written_LSN = 0;

    return lm;
}

/**
 * @brief   指定されたLSNまでのlogをlogfileに書き込む
 * @param (lm) LogManager
 * @param (lsn) 指定するLSN
 * @return
 * @note
 * 指定されたlogまでがファイルに書き込まれていることが保証される。
 * @attention
 * そのlogがあるBlockまでをファイルに書き込むため、指定したLSN以上
 * のlogも書き込まれる可能性もある。
 */
void lm_flush_log_to_lsn(LogManager *lm, int lsn) {
    if (lsn >= lm->last_written_LSN) {
        lm_flush_log(lm);
    }
}

/**
 * @brief   recordをlog pageに追加する
 * @param   (lm) LogManager
 * @param   (log_record) 追加するlog record
 * @param   (record_size) 追加するlog recordのデータサイズ('\0を含めない)
 * @return  追加したrecordのLSN
 * @note
 * log recordは、log pageの後方から書き込まれる。
 * 
 * log pageは、その先頭にboundaryとして、logが書き込まれている部分
 * と書き込まれていない部分の境界を表す数値を保存している。
 * log page : <--boundary--><----log record---->
 * 
 * log pageに書き込まれるlog recordは、recordのサイズとrecord本体
 * から構成される。これは、pageの文字列書き込みの仕様。
 * log record : <--record size--><----log record---->
 * @attention
 */
int lm_append_log(LogManager *lm, char* log_record, int record_size) {
    int boundary;       // log pageのlogがある部分とない部分の境界
    int needed_size;    // logを書き込むために必要なデータサイズ

    // log pageの先頭から、次にlog recordを書き込むべき
    // logpageのoffsetを抽出する
    boundary = get_int_from_page(lm->log_page, 0);

    // log recordは、recordのサイズとそのrecord本体から構成される
    needed_size = sizeof(int) + record_size;

    // 書き込むlog recordが、書き込めるlog pageのサイズに
    // 収まらなかった場合
    if (boundary - needed_size < (int)sizeof(int)) {
        lm_flush_log(lm);
        lm->current_blk = lm_append_newblk(lm);
        boundary = get_int_from_page(lm->log_page, 0);
    }

    // 新しいboundaryを設定する
    boundary -= needed_size;
    
    // log pageのoffsetに、recordのサイズとそのrecord本体を書き込む
    set_string_to_page(lm->log_page, boundary, log_record, record_size);

    // log pageのlog recordのoffsetを変更する
    set_int_to_page(lm->log_page, 0, boundary);

    // log managerの最新のLSNを更新する
    lm->latest_LSN++;
    return lm->latest_LSN;
}

/**
 * @brief   static関数
 *          log pageをlog fileに書き込む。
 * @param   (lm) LogManager
 * @return
 * @note
 * 
 * @attention
 */
static void lm_flush_log(LogManager *lm) {
    // Blockにlog pageを書き込む
    fm_write(lm->fm, lm->current_blk, lm->log_page);
    
    // last written LSNを更新する
    lm->last_written_LSN = lm->latest_LSN;
}

/**
 * @brief   static関数
 *          logfileに新しいBlockを追加する
 * @param   (lm) LogManager
 * @return  追加したBlock
 * @note
 * log fileにBlockを追加し、そのBlockに新しいlog pageの情報を
 * 書き込む。保持していたlog pageの情報は上書きされる。
 * @attention
 */
static Block* lm_append_newblk(LogManager *lm) {
    Block *block;
    
    // log fileにBlockを追加
    block = fm_append_newblk(lm->fm, lm->log_filename);

    // log pageの内容を消去
    clear_page(lm->log_page);
    
    // log pageに新しいoffsetを書き込む。
    // log recordは、右詰めで追加されるため、
    // log fileの大きさがoffsetとして格納される。
    set_int_to_page(lm->log_page, 0, lm->fm->data_size);

    return block;
}