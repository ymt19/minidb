#ifndef BLOCK_H
#define BLODK_H

#define MAX_FILENAME 256

typedef struct Block Block;
struct Block {
    char filename[MAX_FILENAME];
    int blk_number;
};

Block* new_block(char[], int);
int is_equal_block(Block*, Block*);

#endif
