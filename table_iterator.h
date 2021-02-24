#pragma once

#include "transaction.h"
#include "layout.h"
#include "record_page.h"


/**
 * @struct  Table
 * @note
 * 1つのtableのファイルの情報を扱う。
 * 
 * 固定長record(fixed record)
 * recordは物理的に分割しない(unspaned)
 * 1つのファイルに1つのTableを保存(homogeneous)
 * @todo
 * table iterator
 */
struct TableIterator {
    // このtableを扱うトランザクション
    Transaction *tx;

    // このtableのlayout(物理情報)
    Layout *layout;

    // このtableの現在参照しているブロックの情報
    RecordPage *rp;

    // このtableが保存されているfile名
    char filename[MAX_FILENAME + 1];

    // このtableの現在参照しているslot
    int current_slot;
};
typedef struct TableIterator TableIterator;


TableIterator *new_TableIterator(Transaction *tx, Layout *layout, char *tablename, int tablename_size);
void free_TableIterator(TableIterator **tblitr);
void move_first_block_TableIterator(TableIterator *tblitr);
int is_next_record_TableIterator(TableIterator *tblitr);
int get_int_from_TableIterator(TableIterator *tblitr, char *fieldname);
int get_string_from_TableIterator(TableIterator *tblitr, char *fieldname, char *val);
int has_field_TableIterator(TableIterator *tblitr, char *fieldname);
void set_int_to_record_TableIterator(TableIterator *tblitr, char *fieldname, int val);
void set_string_to_record_TableIterator(TableIterator *tblitr, char *fieldname, char *val, int val_size);
void insert_TableIterator(TableIterator *tblitr);
void delete_slot_TableIterator(TableIterator *tblitr);
void move_to_rid_TableIterator(TableIterator *tblitr, RecordID rid);
RecordID get_rid_TableIterator(TableIterator *tblitr);