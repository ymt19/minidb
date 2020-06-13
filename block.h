#pragma once

#define MAX_FILENAME (256)

typedef struct {
    char filename[MAX_FILENAME];
    int blk_number;
} Block;

Block* new_block(char*, int);
int is_equal_block(Block*, Block*);
