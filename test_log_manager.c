/**
 * @file    test_log_managerr.c
 * @brief
 * @note
 * 決められた個数のlog recordを追加し、その個数からlog file内に
 * いくつのlog recordが書き込まれているのかを予想する。
 * @attention
 * 実行する前に、tmp_test_log_managerというディレクトリを
 * 削除する。
 */


#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "log_manager.h"

#define LOG_FILENAME    "logfile"

const int BLOCK_SIZE = 400;

void check_logfile(LogManager*, int);
void create_records(LogManager*,int);

int main(void) {
    FileManager *fm;
    LogManager *lm;
    int num1 = 45;
    int num2 = 90;
    int num3 = 65;
    // log fileに書き込まれているlogの数
    // (作成したlogの数の合計)
    //      / ((1つのBlockのサイズ) / (1つのBlockのサイズ))
    //      * ((1つのBlockのサイズ) / (1つのBlockのサイズ))
    int num_in_logfile;

    fm = new_FileManager("tmp_test_log_manager", BLOCK_SIZE);
    lm = new_LogManager(fm, LOG_FILENAME);

    // 合計1~num1の範囲のlogを作成する
    create_records(lm, num1);

    // 10文字のlogをnum1個作ったとき、log fileに書き込まれているlogの個数
    num_in_logfile = num1
                        / ( BLOCK_SIZE / (10 + sizeof(int)) )
                        * ( BLOCK_SIZE / (10 + sizeof(int)) );
    check_logfile(lm, num_in_logfile);

    // 合計1~num2の範囲のlogが作成される
    create_records(lm, num2);

    // LSNまでのlogをファイルに書き込む
    // ここでのLSNはlog_numに相当する
    lm_flush_log_to_lsn(lm, num3);

    // 10文字のlogをnum2個作ったとき、log fileに書き込まれているlogの個数と
    // flushされたnum3個の最大値
    num_in_logfile = num2
                        / ( BLOCK_SIZE / (10 + sizeof(int)) )
                        * ( BLOCK_SIZE / (10 + sizeof(int)) );
    if (num_in_logfile < num3)
        num_in_logfile = num3;
    check_logfile(lm, num_in_logfile);
}

/**
 * @brief   指定された範囲の数字のrecordがlog fileに書き込まれているのか
 *          をチェックする
 * @param   (lm) LogManager
 * @param   (end) 指定する数字の範囲の終了位置
 * @return
 * @note
 * 最新のlog recordからチェックする。
 * 指定する範囲の開始位置は1とする。
 * もし指定する範囲より少ないrecordしかlog fileに書き込まれていなかったり
 * 範囲より多いrecordが書き込まれていたらエラーとする。
 * @attention
 */
void check_logfile(LogManager *lm, int end) {
    int log_num = end;                  // 着目しているlog番号
    int blk_num;                        // 着目するBlockのblk_num
    int file_size;                      // log fileのBlock数
    int offset;                         // log pageのoffset
    int record_size;                    // recordのサイズ
    char record[200];                   // record
    char expected_record[200];          // 予想されるrecord
    Block *blk;                         // 着目するBlock
    Page *page;                         // Blockの内容を扱うPage

    file_size = fm_file_size(lm->fm, lm->log_filename);
    page = new_page(lm->fm->blk_size);

    // log fileの末尾のBlockから先頭のBlockへそれぞれのBlockの内容を
    // Pageに読み取り、処理を行う
    for (blk_num = file_size - 1; blk_num >= 0; blk_num--) {
        blk = new_block(lm->log_filename, blk_num);     // 毎回mallocするので結構ムダ

        if (fm_read(lm->fm, blk, page) == 0) {
            fprintf(stderr, "checksum不一致");
        }
        
        // Pageの先頭から最後に更新されたlog recordのoffsetを
        // 抽出する
        offset = get_int_from_page(page, 0);

        // 1つのBlockにある全てのlog recordを新しい順に読み取る
        while (offset != lm->fm->data_size) {
            // offsetからlog recordを抽出
            record_size = get_string_from_page(page, offset, record);
            record[record_size] = '\0';

            // 予想されるlog recordの作成
            sprintf(expected_record, "record%04d", log_num);
            
            // 予想されるlog recordとlog fileに保存されていたlog recordの比較
            assert(strcmp(record, expected_record) == 0);

            // 次のoffsetを計算
            offset += record_size + sizeof(int);
            // 次のlog numに変更
            log_num--;
        }

        free_block(blk);
    }
}

/**
 * @brief   指定された範囲の数字でrecordを作成する
 * @param (lm) LogManager
 * @param (end) 指定する範囲の終了位置
 * @return
 * @note
 * 指定する範囲の開始位置はstatic変数として保存され、初期値は1で
 * ある。
 * "record"という文字列の6文字と指定された数で4文字を組み合わせ
 * て、合計10文字のrecordをそれぞれ作成する。
 * ex) 1番のrecord : record0001
 * @attention
 * 9999より大きい数字は指定できない。
 */
void create_records(LogManager *lm, int end) {
    static int i = 1;
    char record[20];    // 作成するrecord

    for (; i <= end; i++) {
        // recordを作成する
        sprintf(record, "record%04d", i);

        // log recordを追加する
        lm_append_log(lm, record, strlen(record));
    }
}