#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "block.h"

/**
 * 構造体Blockのメモリを確保する。
 * 
 * filename     : ファイル名
 * blk_number   : block number
 * @return 成功したら、確保したメモリ領域
 * @return 失敗したら、NULL
 */
Block* new_block(char *filename, int blk_number) {
    Block* blk = malloc(sizeof(Block));
    if (blk == NULL) {
        return NULL;
    }

    snprintf(blk->filename, MAX_FILENAME, "%s", filename);
    blk->filename[MAX_FILENAME-1] = '\0';
    blk->blk_number = blk_number;
    return blk;
}

void free_block(Block *blk) {
    free(blk);
}

/**
 * 指定した2つのBlockが同じBlockであるかどうか。
 * 
 * b1   : 指定するBlock
 * b2   : 指定するBlock
 * @return 同じBlockなら、1
 * @return 同じでないBlockなら、0
 */
int block_is_equal(Block *b1, Block *b2) {
    if (b1->blk_number == b2->blk_number &&
        strncmp(b1->filename, b2->filename, strlen(b1->filename)) == 0) {
        return 1;
    }
    return 0;
}
