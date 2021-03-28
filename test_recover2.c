#include "student_table_operation.h"

#define Dir         "tmp_test_recover"
#define Logfile     "logfile"
#define BlockSize   2000
#define BuffsNum    10
#define PinLimit    1000

int main() {
    FileManager *fm = new_FileManager(Dir, BlockSize);
    LogManager *lm = new_LogManager(fm, Logfile);
    BufferManager *bm = new_BufferManager(fm, lm, BuffsNum, PinLimit);
    TableManager *tm = create_TableManager();

    prepare(fm, lm, bm, tm);

    Transaction *tx1 = start_transaction(fm, lm, bm);
    print_table(tm, tx1);
    commit(tx1);

    return 0;
}