#pragma once

#include "transaction.h"
#include "buffer_manager.h"
#include "file_manager.h"
#include "log_manager.h"
#include "table_iterator.h"
#include "table_manager.h"

#define TABLE                   "STUDENT"
#define COLUMN_STUDENTID        "StudendId"         // 数値
#define COLUMN_NAME             "Name"              // 文字列
#define COLUMN_MAJORID          "MajorId"           // 数値
#define COLUMN_ADMISSION_YEAR   "AdmissionYear"     // 数値

void prepare_new(FileManager *fm, LogManager *lm, BufferManager *bm, TableManager *tm);
void prepare(FileManager *fm, LogManager *lm, BufferManager *bm, TableManager *tm);
void update_int(TableManager *tm, Transaction *tx, char *set_column, int target_student_id, int val);
void update_varchar(TableManager *tm, Transaction *tx, char *set_column, int target_student_id, char *val);
void insert(TableManager *tm, Transaction *tx, int student_id, char *name, int major_id, int admission_year);
void delete(TableManager *tm, Transaction *tx, int target_student_id);
Transaction *start_transaction(FileManager *fm, LogManager *lm, BufferManager *bm);
void commit(Transaction *tx);
void rollback(Transaction *tx);
void print_table(TableManager *tm, Transaction *tx);