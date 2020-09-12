#pragma once

/** @def
 * このプログラムで扱う全てのファイル名の
 * 大きさの最大値
 */
#define MAX_FILENAME (256)

typedef struct {
    char filename[MAX_FILENAME];
    int blk_number;
} Block;

Block* new_block(char*, int);
void free_block(Block*);
int block_is_equal(Block*, Block*);
