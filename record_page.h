#pragma once

#include "transaction.h"
#include "block.h"
#include "layout.h"

enum RECORD_STATE_FLAG {
    EMPTY,
    USED,
};
typedef enum RECORD_STATE_FLAG RECORD_STATE_FLAG;

struct RecordPage {
    Transaction *tx;
    Block *blk;
    Layout *layout;
};
typedef struct RecordPage RecordPage;

RecordPage *new_RecordPage(Transaction *tx, Block *blk, Layout *layout);
void init_RecordPage(RecordPage *rp);
int get_int_from_RecordPage(RecordPage *rp, int slot, char *fieldname);
int get_string_from_RecordPage(RecordPage *rp, int slot, char *fieldname, char *val);
void set_int_to_RecordPage(RecordPage *rp, int slot, char *fieldname, int val);
void set_string_to_RecordPage(RecordPage *rp, int slot, char *fieldname, char *val, int val_size);
void delete_record_RecordPage(RecordPage *rp, int slot);
int next_after_slot_RecordPage(RecordPage *rp, int slot);
int insert_after_slot_RecordPage(RecordPage *rp, int slot);
