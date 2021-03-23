#pragma once

#include "transaction.h"
#include "layout.h"

#define MAX_TABLENAME_SIZE  (16)
#define MAX_FIELDNAME_SIZE  (16)

#define TBLCAT          "tblcat"
#define TBLCAT_TBLNAME  "tblname"
#define TBLCAT_SLOTSIZE "slotsize"

#define FLDCAT          "fldcat"
#define FLDCAT_TBLNAME  "tblname"
#define FLDCAT_FLDNAME  "fldname"
#define FLDCAT_TYPE     "type"
#define FLDCAT_LENGTH   "length"
#define FLDCAT_OFFSET   "offset"

struct TableManager {
    // 存在するtableの情報を保持するtableのlayout
    // field: tblname, slotsize
    Layout *tblcat_layout;

    // 存在するtableのfieldの情報を保持するtableのlayout
    // field: tblname, fldname, type, length, offset
    Layout *fldcat_layout;
};
typedef struct TableManager TableManager;


TableManager *create_TableManager();
void tm_save_TableManager(TableManager* tm, Transaction *tx);
void free_TableManager(TableManager *tm);
void tm_create_table(TableManager *tm, char *tblname, Schema *sch, Transaction *tx);
Layout *tm_get_layout(TableManager *tm, char *target_tblname, Transaction *tx);