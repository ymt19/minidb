#include <stdio.h>
#include <string.h>
#include "student_table_operation.h"

/**
 * @brief   STUDENTテーブルを扱う前の準備(新しい場合)
 * @param
 * @return
 * @note
 */
void prepare_new(FileManager *fm, LogManager *lm, BufferManager *bm, TableManager *tm) {
    // 新規db作成時のtmの処理
    Transaction *tm_save = new_Transaction(fm, lm, bm);
    tm_save_TableManager(tm, tm_save);
    tx_commit(tm_save);

    // STUDENTテーブルの作成
    Transaction *tx_make_table = new_Transaction(fm, lm, bm);
    Schema *sch = new_schema();
    add_int_field_to_schema(sch, COLUMN_STUDENTID);
    add_string_field_to_schema(sch, COLUMN_NAME, 30);   // 30文字まで
    add_int_field_to_schema(sch, COLUMN_MAJORID);
    add_int_field_to_schema(sch, COLUMN_ADMISSION_YEAR);
    tm_create_table(tm, TABLE, sch, tx_make_table);
    tx_commit(tx_make_table);
}

/**
 * @brief   STUDENTテーブルを扱う前の準備(既に存在する場合)
 * @param
 * @return
 * @note
 */
void prepare(FileManager *fm, LogManager *lm, BufferManager *bm, TableManager *tm) {
    // recover処理を行う
    Transaction *recover = new_Transaction(fm, lm, bm);
    tx_recover(recover);
    tx_commit(recover);
}

void update_int(TableManager *tm, Transaction *tx, char *set_column, int target_student_id, int val) {
    int succeed = 0; // 成功したかどうか

    Layout *layout = tm_get_layout(tm, TABLE, tx);
    TableIterator *tblitr = new_TableIterator(tx, layout, TABLE);
    while (tblitr_go_to_next_record(tblitr)) {
        int now_student_id = tblitr_get_int_from_current_slot(tblitr, COLUMN_STUDENTID);
        if (now_student_id == target_student_id) {
            tblitr_set_int_to_current_slot(tblitr, set_column, val);
            succeed = 1;
            break;
        }
    }
    free_TableIterator(&tblitr);

    // 操作の完了を伝える文字列出力
    if (succeed) {
        printf("%-12s(txnum = %d): column == %s, student id == %d, new value => %d\n",
            "update int", tx->txnum, set_column, target_student_id, val);
    } else {
        printf("failure update int\n");
    }
}

void update_varchar(TableManager *tm, Transaction *tx, char *set_column, int target_student_id, char *val) {
    int succeed = 0; // 成功したかどうか

    Layout *layout = tm_get_layout(tm, TABLE, tx);
    TableIterator *tblitr = new_TableIterator(tx, layout, TABLE);
    while (tblitr_go_to_next_record(tblitr)) {
        int now_student_id = tblitr_get_int_from_current_slot(tblitr, COLUMN_STUDENTID);
        if (now_student_id == target_student_id) {
            tblitr_set_string_to_current_slot(tblitr, set_column, val, strlen(val));
            succeed = 1;
            break;
        }
    }
    free_TableIterator(&tblitr);

    // 操作の完了を伝える文字列出力
    if (succeed) {
        printf("%-12s(txnum = %d): column = %s, student id = %d, new value => %s\n",
            "update varchar", tx->txnum, set_column, target_student_id, val);
    } else {
        printf("failure update varchar\n");
    }
}

void insert(TableManager *tm, Transaction *tx, int student_id, char *name, int major_id, int admission_year) {
    Layout *layout = tm_get_layout(tm, TABLE, tx);
    TableIterator *tblitr = new_TableIterator(tx, layout, TABLE);
    tblitr_insert(tblitr);
    tblitr_set_int_to_current_slot(tblitr, COLUMN_STUDENTID, student_id);
    tblitr_set_string_to_current_slot(tblitr, COLUMN_NAME, name, strlen(name));
    tblitr_set_int_to_current_slot(tblitr, COLUMN_MAJORID, major_id);
    tblitr_set_int_to_current_slot(tblitr, COLUMN_ADMISSION_YEAR, admission_year);
    free_TableIterator(&tblitr);

    // 操作の完了を伝える文字列出力
    printf("%-12s(txnum = %d): %d, %s, %d, %d\n",
            "insert", tx->txnum, student_id, name, major_id, admission_year);
}

void delete(TableManager *tm, Transaction *tx, int target_student_id) {
    int succeed = 0; // 成功したかどうか

    Layout *layout = tm_get_layout(tm, TABLE, tx);
    TableIterator *tblitr = new_TableIterator(tx, layout, TABLE);
    while (tblitr_go_to_next_record(tblitr)) {
        int now_student_id = tblitr_get_int_from_current_slot(tblitr, COLUMN_STUDENTID);
        if (now_student_id == target_student_id) {
            tblitr_delete_slot(tblitr);
            succeed = 1;
        }
    }
    free_TableIterator(&tblitr);

    // 操作の完了を伝える文字列出力
    if (succeed) {
        printf("%-12s(txnum = %d): student_id = %d\n",
                "delete", tx->txnum, target_student_id);
    } else {
        printf("failure delete\n");
    }
}

Transaction *start_transaction(FileManager *fm, LogManager *lm, BufferManager *bm) {
    Transaction *new_tx = new_Transaction(fm, lm, bm);

    // 操作の完了を伝える文字列出力
    printf("<<%-12s : new txnum = %d>>\n", "start transactioin", new_tx->txnum);

    return new_tx;
}

void commit(Transaction *tx) {
    tx_commit(tx);

    // 操作の完了を伝える文字列出力
    printf("<<%-12s(txnum = %d)>>\n", "commit", tx->txnum);
}

void rollback(Transaction *tx) {
    tx_rollback(tx);

    // 操作の完了を伝える文字列出力
    printf("<<%-12s(txnum = %d)>>\n", "rollback", tx->txnum);
}

void print_table(TableManager *tm, Transaction *tx) {
    int num;
    char str[MAX_STRING_SIZE];

    Layout *layout = tm_get_layout(tm, TABLE, tx);

    TableIterator *tblitr = new_TableIterator(tx, layout, TABLE);
    printf("    | %15s | %30s | %15s | %15s |\n",
            COLUMN_STUDENTID, COLUMN_NAME, COLUMN_MAJORID, COLUMN_ADMISSION_YEAR);
    while (tblitr_go_to_next_record(tblitr)) {
        num = tblitr_get_int_from_current_slot(tblitr, COLUMN_STUDENTID);
        printf("    | %15d | ", num);
        
        tblitr_get_string_from_current_slot(tblitr, COLUMN_NAME, str);
        printf("%30s | ", str);

        num = tblitr_get_int_from_current_slot(tblitr, COLUMN_MAJORID);
        printf("%15d | ", num);

        num = tblitr_get_int_from_current_slot(tblitr, COLUMN_ADMISSION_YEAR);
        printf("%15d |\n", num);
    }
    free_TableIterator(&tblitr);
}