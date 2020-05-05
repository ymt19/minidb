#include <string.h>
#include <stdlib.h>
#include "block.h"

/**
 * Allocate a new block.
 */
Block* new_block(char filename[MAX_FILENAME], int blk_number) {
    Block* blk = malloc(sizeof(Block));
    if (blk == NULL) {
        return NULL;
    }
    
    strncpy(blk->filename, filename, sizeof(blk->filename));
    blk->blk_number = blk_number;
}

/**
 * Compare the two blocks.
 * @returns 1 on two blocks are the same block 0 no that is not so.
 */
int is_equal_block(Block *b1, Block *b2) {
    if (b1->blk_number == b2->blk_number &&
        strncmp(b1->filename, b2->filename, strlen(b1->filename)) == 0) {
        return 1;
    }
    return 0;
}
