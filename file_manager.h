#pragma once

#include "block.h"
#include "page.h"

extern unsigned int g_blksize;

void new_FileManager(char*, unsigned int);
void fm_read_page_from_blk(Block*, Page*);
void fm_write_page_to_blk(Block*, Page*);
Block* fm_append_newblk(char filename[MAX_FILENAME]);
int file_size(char filename[MAX_FILENAME]);
