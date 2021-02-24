#pragma once

/**
 * @struct
 * @brief   ファイル内のrecord識別子
 */
struct RecordID {
    // ファイル内のブロックを表すblock number
    int blknum;

    // ブロック内のslot
    int slot;
};
typedef struct RecordID RecordID;

void set_RecordID(RecordID *rid, int blknum, int slot);
int is_equal_RecordID(RecordID *rid0, RecordID *rid1);