#include <stdio.h>
#include <stdlib.h>

#include "file_manager.h"

int main(void) {
    // This database has 400 byte blocks.
    new_FileManager("tmp", 400);

    // Write a string and a number to the page and write it to the block.
    Block *blk = new_block("tmp1", 2);
    Page *page1 = new_page();
    int pos1 = 88;
    int size = set_string_to_page(page1, pos1, "abcdefghijklmn");
    printf("%d\n", size);
    int pos2 = pos1 + size;
    set_int_to_page(page1, pos2, 1111);
    write_page_to_blk(blk, page1);

    // Read a string and a number from the block.
    Page *page2 = new_page();
    read_page_from_blk(blk, page2);
    char str[MAX_STRING_SIZE];
    printf("%p main str\n", &str);
    get_string_from_page(page2, pos1, str);
    printf("%s\n", str);
    int num;
    get_int_from_page(page2, pos2, &num);
    printf("%d\n", num);
    return 0;
}