#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "recovery_manager.h"
#include "file_manager.h"
#include "log_manager.h"
#include "transaction.h"

void test_init(FileManager*, LogManager*, BufferManager*, Block*, Block*);
void test_modify(FileManager*, LogManager*, BufferManager*, Block*, Block*);
void test_recover(FileManager*, LogManager*, BufferManager*, Block*, Block*);
void print_values(FileManager*, Block*, Block*);

char dir[] = "tmp_test_recovery_manager";
char log_filename[] = "logfile";
char filename[] = "testfile";
const int block_size = 400;
const int buff_num = 10;
const int pin_limit = 1000;
char str1[] = "abc";
char str2[] = "def";
char str3[] = "ghi";
char str4[] = "jkl";

int main() {
    FileManager *fm = new_FileManager(dir, block_size);
    LogManager *lm = new_LogManager(fm, log_filename);
    BufferManager *bm = new_BufferManager(fm, lm, buff_num, pin_limit);
    Block *blk0 = new_block(filename, 0);
    Block *blk1 = new_block(filename, 1);

    test_init(fm, lm, bm, blk0, blk1);
    test_modify(fm, lm, bm, blk0, blk1);
    test_recover(fm, lm, bm, blk0, blk1);
}

/**
 * @brief   logを残さず、ブロックを編集する
 * @param
 * @return
 * @note
 * @attention
 */
void test_init(FileManager *fm, LogManager *lm, BufferManager *bm, Block *blk0, Block *blk1) {
    int i;
    Transaction *tx1, *tx2;
    tx1 = new_Transaction(fm, lm, bm);
    tx2 = new_Transaction(fm, lm, bm);
    tx_pin(tx1, blk0);
    tx_pin(tx2, blk1);
    int pos = 0;
    for (i = 0; i < 6; i++) {
        tx_set_int(tx1, blk0, pos, pos, 1);     // logに書き込まない
        tx_set_int(tx2, blk1, pos, pos, 1);     // logに書き込まない
        pos += sizeof(int);
    }
    tx_set_string(tx1, blk0, 30, str1, strlen(str1), 1);    // logに書き込まない
    tx_set_string(tx2, blk1, 30, str2, strlen(str2), 1);    // logに書き込まない
    tx_commit(tx1);
    tx_commit(tx2);

    /*** test開始 ****/
    Page *page0 = new_page(fm->blk_size);
    Page *page1 = new_page(fm->blk_size);
    fm_read(fm, blk0, page0);
    fm_read(fm, blk1, page1);
    pos = 0;
    for (i = 0; i < 6; i++) {
        assert(pos == get_int_from_page(page0, pos));
        assert(pos == get_int_from_page(page1, pos));
        pos += sizeof(int);
    }
    char string[MAX_STRING_SIZE + 1];
    get_string_from_page(page0, 30, string);
    assert(strcmp(string, str1) == 0);
    get_string_from_page(page1, 30, string);
    assert(strcmp(string, str2) == 0);
    free(page0);
    free(page1);
    /*******************/
}

/**
 * @brief   logを残して、ブロックを編集する
 * @param
 * @return
 * @note
 * 
 * @attention
 */
void test_modify(FileManager *fm, LogManager *lm, BufferManager *bm, Block *blk0, Block *blk1) {
    int i;
    Transaction *tx3, *tx4;
    tx3 = new_Transaction(fm, lm, bm);
    tx4 = new_Transaction(fm, lm, bm);
    tx_pin(tx3, blk0);
    tx_pin(tx4, blk1);
    int pos = 0;
    for (i = 0; i < 6; i++) {
        tx_set_int(tx3, blk0, pos, pos+100, 0);
        tx_set_int(tx4, blk1, pos, pos+100, 0);
        pos += sizeof(int);
    }
    tx_set_string(tx3, blk0, 30, str3, strlen(str3), 0);
    tx_set_string(tx4, blk1, 30, str4, strlen(str4), 0);
    bm_flush_all(bm, 3);    // tx3の変更を永続化
    bm_flush_all(bm, 4);    // tx4の変更を永続化

    /****test開始******/
    // block0は、tx3の変更が永続化されたためtx3の値が残る
    // block1は、tx4の変更が永続化されたためtx4の値が残る
    Page *page0 = new_page(fm->blk_size);
    Page *page1 = new_page(fm->blk_size);
    fm_read(fm, blk0, page0);
    fm_read(fm, blk1, page1);
    pos = 0;
    for (i = 0; i < 6; i++) {
        assert(pos+100 == get_int_from_page(page0, pos));
        assert(pos+100 == get_int_from_page(page1, pos));
        pos += sizeof(int);
    }
    char string[MAX_STRING_SIZE + 1];
    get_string_from_page(page0, 30, string);
    assert(strcmp(string, str3) == 0);
    get_string_from_page(page1, 30, string);
    assert(strcmp(string, str4) == 0);
    free(page0);
    free(page1);
    /******************/

    tx_rollback(tx3);
    // tx4はcommit/rollbackしないため、recover時に変更が戻される

    /****test開始******/
    // block0は、tx3の変更はrollbackされたため、tx1による値が残る
    // block1は、未完了のtx4の変更した値が残る
    page0 = new_page(fm->blk_size);
    page1 = new_page(fm->blk_size);
    fm_read(fm, blk0, page0);
    fm_read(fm, blk1, page1);
    pos = 0;
    for (i = 0; i < 6; i++) {
        assert(pos == get_int_from_page(page0, pos));
        assert(pos+100 == get_int_from_page(page1, pos));
        pos += sizeof(int);
    }
    get_string_from_page(page0, 30, string);
    assert(strcmp(string, str1) == 0);
    get_string_from_page(page1, 30, string);
    assert(strcmp(string, str4) == 0);
    free(page0);
    free(page1);
    /******************/
}

/**
 * @brief   recoverを実行する
 * @param
 * @return
 * @note
 * 
 * @attention
 */
void test_recover(FileManager *fm, LogManager *lm, BufferManager *bm, Block *blk0, Block *blk1) {
    Transaction *tx = new_Transaction(fm, lm, bm);
    tx_recover(tx);

    /****test開始******/
    // blcok0は、そのままtx1の値が残っている
    // block1は、recoverのため未完了のtx4の値をundoし、tx2の値が残る
    Page *page0 = new_page(fm->blk_size);
    Page *page1 = new_page(fm->blk_size);
    fm_read(fm, blk0, page0);
    fm_read(fm, blk1, page1);
    int i, pos = 0;
    for (i = 0; i < 6; i++) {
        assert(pos == get_int_from_page(page0, pos));
        assert(pos == get_int_from_page(page1, pos));
        pos += sizeof(int);
    }
    char string[MAX_STRING_SIZE + 1];
    get_string_from_page(page0, 30, string);
    assert(strcmp(string, str1) == 0);
    get_string_from_page(page1, 30, string);
    assert(strcmp(string, str2) == 0);
    free(page0);
    free(page1);
    /******************/
}

void print_values(FileManager *fm, Block *blk0, Block *blk1) {
    printf("---------print values----------\n");
    int i;
    Page *page0 = new_page(fm->blk_size);
    Page *page1 = new_page(fm->blk_size);
    fm_read(fm, blk0, page0);
    fm_read(fm, blk1, page1);
    int pos = 0;
    for (i = 0; i < 6; i++) {
        printf("block0 offset-%d %d\n", pos, get_int_from_page(page0, pos));
        printf("block1 offset-%d %d\n", pos, get_int_from_page(page1, pos));
        pos += sizeof(int);
    }
    char string[MAX_STRING_SIZE + 1];
    get_string_from_page(page0, 30, string);
    printf("block0 offset-%d %s\n", 30, string);
    get_string_from_page(page1, 30, string);
    printf("block1 offset-%d %s\n", 30, string);

    free(page0);
    free(page1);
}