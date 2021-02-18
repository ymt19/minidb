#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "buffer_list.h"

char dir[] = "tmp_test_buffer_list";
char log_filename[] = "logfile";
char filename[] = "testfile";
const int block_size = 400;
const int buff_num = 10;
const int pin_limit = 1000;

int main() {
    FileManager *fm = new_FileManager(dir, block_size);
    LogManager *lm = new_LogManager(fm, log_filename);
    BufferManager *bm = new_BufferManager(fm, lm, buff_num, pin_limit);
    BufferList *list = NULL;
    Block *blk0 = new_block(filename, 0);
    Block *blk1 = new_block(filename, 1);
    Block *blk2 = new_block(filename, 2);
    Buffer *tmp;

    // blk0を追加
    // list = [blk0]
    tmp = bm_pin(bm, blk0);
    add_BufferList(&list, tmp);

    tmp = get_buffer_from_BufferList(list, blk0);
    assert(tmp != NULL);
    assert(block_is_equal(tmp->blk, blk0));

    // blk1を追加
    // list = [blk0, blk1]
    tmp = bm_pin(bm, blk1);
    add_BufferList(&list, tmp);

    tmp = get_buffer_from_BufferList(list, blk0);
    assert(tmp != NULL);
    assert(block_is_equal(tmp->blk, blk0));
    tmp = get_buffer_from_BufferList(list, blk1);
    assert(tmp != NULL);
    assert(block_is_equal(tmp->blk, blk1));

    // blk2を追加
    // list = [blk0, blk1, blk2]
    tmp = bm_pin(bm, blk2);
    add_BufferList(&list, tmp);

    tmp = get_buffer_from_BufferList(list, blk0);
    assert(tmp != NULL);
    assert(block_is_equal(tmp->blk, blk0));
    tmp = get_buffer_from_BufferList(list, blk1);
    assert(tmp != NULL);
    assert(block_is_equal(tmp->blk, blk1));
    tmp = get_buffer_from_BufferList(list, blk2);
    assert(tmp != NULL);
    assert(block_is_equal(tmp->blk, blk2));

    // blk1を除去
    // list = [blk0, blk2]
    tmp = get_buffer_from_BufferList(list, blk1);
    bm_unpin(bm, tmp);
    remove_BufferList(&list, tmp);

    tmp = get_buffer_from_BufferList(list, blk0);
    assert(tmp != NULL);
    assert(block_is_equal(tmp->blk, blk0));
    tmp = get_buffer_from_BufferList(list, blk1);
    assert(tmp == NULL);
    tmp = get_buffer_from_BufferList(list, blk2);
    assert(tmp != NULL);
    assert(block_is_equal(tmp->blk, blk2));

    // blk2を除去
    // list = [blk0]
    tmp = get_buffer_from_BufferList(list, blk2);
    bm_unpin(bm, tmp);
    remove_BufferList(&list, tmp);

    tmp = get_buffer_from_BufferList(list, blk0);
    assert(tmp != NULL);
    assert(block_is_equal(tmp->blk, blk0));
    tmp = get_buffer_from_BufferList(list, blk1);
    assert(tmp == NULL);
    tmp = get_buffer_from_BufferList(list, blk2);
    assert(tmp == NULL);

    // blk1,blk2を追加
    // list = [blk0, blk1, blk2]
    tmp = bm_pin(bm, blk1);
    add_BufferList(&list, tmp);
    tmp = bm_pin(bm, blk2);
    add_BufferList(&list, tmp);

    tmp = get_buffer_from_BufferList(list, blk0);
    assert(tmp != NULL);
    assert(block_is_equal(tmp->blk, blk0));
    tmp = get_buffer_from_BufferList(list, blk1);
    assert(tmp != NULL);
    assert(block_is_equal(tmp->blk, blk1));
    tmp = get_buffer_from_BufferList(list, blk2);
    assert(tmp != NULL);
    assert(block_is_equal(tmp->blk, blk2));

    
    // listからすべて除去
    // list = []
    remove_all_BufferList(&list, bm);

    assert(list == NULL);
    tmp = get_buffer_from_BufferList(list, blk0);
    assert(tmp == NULL);
    tmp = get_buffer_from_BufferList(list, blk1);
    assert(tmp == NULL);
    tmp = get_buffer_from_BufferList(list, blk2);
    assert(tmp == NULL);
}