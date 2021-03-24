#include <stdio.h>
#include <string.h>
#include "transaction.h"
#include "buffer_manager.h"
#include "file_manager.h"
#include "log_manager.h"
#include "table_iterator.h"
#include "table_manager.h"

#define Dir         "data"
#define Logfile     "logfile"
#define BlockSize   2000
#define BuffsNum    10
#define PinLimit    1000

#define TABLE                   "STUDENT"
#define COLUMN_STUDENTID        "StudendId"         // 数値
#define COLUMN_NAME             "Name"              // 文字列
#define COLUMN_MAJORID          "MajorId"           // 数値
#define COLUMN_ADMISSION_YEAR   "AdmissionYear"     // 数値

FileManager *fm;
LogManager *lm;
BufferManager *bm;
TableManager *tm;

Schema *sch;

void update_int(Transaction *tx, char *set_column, int target_student_id, int val);
void update_varchar(Transaction *tx, char *set_column, int target_student_id, char *val);
void insert(Transaction *tx, int student_id, char *name, int major_id, int admission_year);
void commit(Transaction *tx);
void rollback(Transaction *tx);
void print_table(Transaction *tx);

int main() {
    fm = new_FileManager(Dir, BlockSize);
    lm = new_LogManager(fm, Logfile);
    bm = new_BufferManager(fm, lm, BuffsNum, PinLimit);
    tm = create_TableManager();

    // 新規db作成時のtmの処理
    Transaction *tm_save = new_Transaction(fm, lm, bm);
    tm_save_TableManager(tm, tm_save);
    tx_commit(tm_save);

    // STUDENTテーブルの作成
    Transaction *tx_make_table = new_Transaction(fm, lm, bm);
    sch = new_schema();
    add_int_field_to_schema(sch, COLUMN_STUDENTID);
    add_string_field_to_schema(sch, COLUMN_NAME, 30);   // 30文字まで
    add_int_field_to_schema(sch, COLUMN_MAJORID);
    add_int_field_to_schema(sch, COLUMN_ADMISSION_YEAR);
    tm_create_table(tm, TABLE, sch, tx_make_table);

    // dbの変更を行うトランザクションの作成
    Transaction *tx1 = new_Transaction(fm, lm, bm);
    Transaction *tx2 = new_Transaction(fm, lm, bm);

    insert(tx1, 1, "sushi", 100, 2020);
    insert(tx1, 2, "ramen", 101, 2015);
    print_table(tx1);
    rollback(tx1);

    print_table(tx2);
    insert(tx2, 1, "sushi", 100, 2020);
    insert(tx2, 2, "ramen", 101, 2015);
    insert(tx2, 3, "takoyaki", 201, 2018);
    print_table(tx2);
    update_int(tx2, COLUMN_MAJORID, 1, 200);
    update_int(tx2, COLUMN_ADMISSION_YEAR, 2, 2030);
    update_varchar(tx2, COLUMN_NAME, 3, "okonomiyaki");
    print_table(tx2);

    // トランザクションの終了
    tx_commit(tx2);
}


void update_int(Transaction *tx, char *set_column, int target_student_id, int val) {
    Layout *layout = tm_get_layout(tm, TABLE, tx);
    TableIterator *tblitr = new_TableIterator(tx, layout, TABLE);
    while (tblitr_go_to_next_record(tblitr)) {
        int now_student_id = tblitr_get_int_from_current_slot(tblitr, COLUMN_STUDENTID);
        if (now_student_id == target_student_id) {
            tblitr_set_int_to_current_slot(tblitr, set_column, val);
            break;
        }
    }
    free_TableIterator(&tblitr);

    // 操作の完了を伝える文字列出力
    printf("update int: column == %s, student id == %d, new value => %d\n", set_column, target_student_id, val);
}

void update_varchar(Transaction *tx, char *set_column, int target_student_id, char *val) {
    Layout *layout = tm_get_layout(tm, TABLE, tx);
    TableIterator *tblitr = new_TableIterator(tx, layout, TABLE);
    while (tblitr_go_to_next_record(tblitr)) {
        int now_student_id = tblitr_get_int_from_current_slot(tblitr, COLUMN_STUDENTID);
        if (now_student_id == target_student_id) {
            tblitr_set_string_to_current_slot(tblitr, set_column, val, strlen(val));
            break;
        }
    }
    free_TableIterator(&tblitr);

    // 操作の完了を伝える文字列出力
    printf("update varchar: column = %s, student id = %d, new value => %s\n", set_column, target_student_id, val);
}

void insert(Transaction *tx, int student_id, char *name, int major_id, int admission_year) {
    Layout *layout = tm_get_layout(tm, TABLE, tx);
    TableIterator *tblitr = new_TableIterator(tx, layout, TABLE);
    tblitr_insert(tblitr);
    tblitr_set_int_to_current_slot(tblitr, COLUMN_STUDENTID, student_id);
    tblitr_set_string_to_current_slot(tblitr, COLUMN_NAME, name, strlen(name));
    tblitr_set_int_to_current_slot(tblitr, COLUMN_MAJORID, major_id);
    tblitr_set_int_to_current_slot(tblitr, COLUMN_ADMISSION_YEAR, admission_year);
    free_TableIterator(&tblitr);

    // 操作の完了を伝える文字列出力
    printf("insert: %d, %s, %d, %d\n", student_id, name, major_id, admission_year);
}

void commit(Transaction *tx) {
    tx_commit(tx);

    // 操作の完了を伝える文字列出力
    printf("commit: txnum = %d\n", tx->txnum);
}

void rollback(Transaction *tx) {
    tx_rollback(tx);

    // 操作の完了を伝える文字列出力
    printf("rollback: txnum = %d\n", tx->txnum);
}

void print_table(Transaction *tx) {
    int num;
    char str[MAX_STRING_SIZE];

    Layout *layout = tm_get_layout(tm, TABLE, tx);

    TableIterator *tblitr = new_TableIterator(tx, layout, TABLE);
    printf("| %15s | %30s | %15s | %15s |\n",
            COLUMN_STUDENTID, COLUMN_NAME, COLUMN_MAJORID, COLUMN_ADMISSION_YEAR);
    while (tblitr_go_to_next_record(tblitr)) {
        num = tblitr_get_int_from_current_slot(tblitr, COLUMN_STUDENTID);
        printf("| %15d | ", num);
        
        tblitr_get_string_from_current_slot(tblitr, COLUMN_NAME, str);
        printf("%30s | ", str);

        num = tblitr_get_int_from_current_slot(tblitr, COLUMN_MAJORID);
        printf("%15d | ", num);

        num = tblitr_get_int_from_current_slot(tblitr, COLUMN_ADMISSION_YEAR);
        printf("%15d |\n", num);
    }
    free_TableIterator(&tblitr);
}