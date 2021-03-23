#pragma once

#include "buffer_manager.h"
#include "log_manager.h"
#include "buffer_list.h"

typedef struct {
    int txnum;
    FileManager *fm;
    BufferManager *bm;
    LogManager *lm;
    BufferList *bufflist;
} Transaction;

Transaction *new_Transaction(FileManager*, LogManager*, BufferManager*);
void tx_commit(Transaction*);
void tx_rollback(Transaction*);
void tx_recover(Transaction*);
void tx_pin(Transaction*, Block*);
void tx_unpin(Transaction*, Block*);
int tx_get_int(Transaction*, Block*, int);
int tx_get_string(Transaction*, Block*, int, char*);
void tx_set_int(Transaction*, Block*, int, int, int);
void tx_set_string(Transaction*, Block*, int, char*, int, int);
int tx_filesize(Transaction*, char*);
Block *tx_append_blk(Transaction*, char*);