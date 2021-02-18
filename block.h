#pragma once

/** @def
 * このプログラムで扱う全てのファイル名の
 * 大きさの最大値
 */
#define MAX_FILENAME (128)

typedef struct {
    char filename[MAX_FILENAME+1];
    int blk_number;
} Block;

Block* new_block(char*, int);
void free_block(Block*);
int block_is_equal(Block*, Block*);
