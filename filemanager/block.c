#include <string.h>
#include <stdlib.h>
#include "block.h"

Block* new_block(char filename[MAX_FILENAME], int blk_number) {
    Block* blk = calloc(1, sizeof(Block));
    strncpy(blk->filename, filename, sizeof(blk->filename));
    blk->blk_number = blk_number;
}

int is_equal_block(Block *b1, Block *b2) {
    if (b1->blk_number == b2->blk_number &&
        strncmp(b1->filename, b2->filename, strlen(b1->filename)) == 0) {
        return 1;
    }
    return 0;
}
