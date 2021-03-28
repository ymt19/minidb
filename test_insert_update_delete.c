#include "student_table_operation.h"

#define Dir         "tmp_test_insert_update_delete"
#define Logfile     "logfile"
#define BlockSize   2000
#define BuffsNum    10
#define PinLimit    1000

int main() {
    FileManager *fm = new_FileManager(Dir, BlockSize);
    LogManager *lm = new_LogManager(fm, Logfile);
    BufferManager *bm = new_BufferManager(fm, lm, BuffsNum, PinLimit);
    TableManager *tm = create_TableManager();

    prepare_new(fm, lm, bm, tm);

    Transaction *tx1 = start_transaction(fm, lm, bm);
    insert(tm, tx1, 1, "sushi", 100, 2020);
    insert(tm, tx1, 2, "ramen", 101, 2015);
    insert(tm, tx1, 3, "takoyaki", 201, 2018);
    print_table(tm, tx1);
    commit(tx1);

    Transaction *tx2 = start_transaction(fm, lm, bm);
    update_int(tm, tx2, COLUMN_MAJORID, 1, 200);
    update_int(tm, tx2, COLUMN_ADMISSION_YEAR, 2, 2030);
    update_varchar(tm, tx2, COLUMN_NAME, 3, "okonomiyaki");
    print_table(tm, tx2);
    commit(tx2);

    Transaction *tx3 = start_transaction(fm, lm, bm);
    delete(tm, tx3, 2);
    print_table(tm, tx3);
    insert(tm, tx3, 4, "gyoza", 300, 2022);
    print_table(tm, tx3);
    commit(tx3);

    return 0;
}