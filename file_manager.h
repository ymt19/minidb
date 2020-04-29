#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

extern unsigned int g_blksize;

void new_FileManager(char*, unsigned int);
void read_page_from_blk(Block*, Page*);
void write_page_to_blk(Block*, Page*);

#endif