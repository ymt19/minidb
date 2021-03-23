#pragma once

#include "transaction.h"
#include "layout.h"
#include "record_page.h"
#include "record_id.h"


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


TableIterator *new_TableIterator(Transaction *tx, Layout *layout, char *tablename);
void free_TableIterator(TableIterator **tblitr);
void tblitr_move_first_block(TableIterator *tblitr);
int tblitr_go_to_next_record(TableIterator *tblitr);
int tblitr_get_int_from_current_slot(TableIterator *tblitr, char *fieldname);
int tblitr_get_string_from_current_slot(TableIterator *tblitr, char *fieldname, char *val);
// int tblitr_has_field(TableIterator *tblitr, char *fieldname);
void tblitr_set_int_to_current_slot(TableIterator *tblitr, char *fieldname, int val);
void tblitr_set_string_to_current_slot(TableIterator *tblitr, char *fieldname, char *val, int val_size);
void tblitr_insert(TableIterator *tblitr);
void tblitr_delete_slot(TableIterator *tblitr);
void tblitr_move_to_rid(TableIterator *tblitr, RecordID rid);
RecordID tblitr_get_rid(TableIterator *tblitr);
