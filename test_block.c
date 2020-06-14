// Test code for block.c .
#include <stdio.h>
#include <assert.h>
#include "block.h"

int main(void) {
    Block *b1 = new_block("block1", 1);
    Block *b2 = new_block("block10", 2);
    Block *b3 = new_block("block1", 1);
    assert(!is_equal_block(b1, b2));
    assert(is_equal_block(b1, b3));
}