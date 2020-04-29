// Test code for block.c .
#include <stdio.h>
#include "block.h"

int main(void) {
    Block *b1 = new_block("block1", 1);
    Block *b2 = new_block("block10", 2);
    Block *b3 = new_block("block1", 1);
    if (is_equal_block(b1, b2)) {
        printf("b1 is same with b2.\n");
    } else {
        printf("b1 is not same with b2.\n");
    }
    if (is_equal_block(b1, b3)) {
        printf("b1 is same with b3.\n");
    } else {
        printf("b1 is not same with b3.\n");
    }
}