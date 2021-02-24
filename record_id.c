#include <stdlib.h>
#include "record_id.h"

void set_RecordID(RecordID *rid, int blknum, int slot) {
    rid->blknum = blknum;
    rid->slot = slot;
    return rid;
}

int is_equal_RID(RecordID rid0, RecordID rid1) {
    return rid0.blknum == rid1.blknum && rid0.slot == rid1.slot;
}