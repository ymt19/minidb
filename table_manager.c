#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "table_manager.h"
#include "transaction.h"
#include "schema.h"
#include "layout.h"
#include "table_iterator.h"

static int get_slotsize_from_tblcat(TableManager *tm, char *target_tblname, Transaction *tx);
static Layout *get_layout_from_fldcat(TableManager *tm, char *target_tblname, int slotsize, Transaction *tx);

/**
 * @brief   TableManagerのメモリを確保する
 * @param   is_new 呼び出し元のdbが新しいdbかどうか
 * @param   tx tblcat, fldcatを作成するトランザクション
 * @return  作成したTableManagerのメモリを指すポインタ
 * @note
 * @todo
 * 
 */
TableManager *create_TableManager() {
    TableManager *tm = malloc(sizeof(TableManager));
    if (tm == NULL) {
        return NULL;
    }

    Schema *tblcat_schema = new_schema();
    add_string_field_to_schema(tblcat_schema, TBLCAT_TBLNAME, MAX_TABLENAME_SIZE);
    add_int_field_to_schema(tblcat_schema, TBLCAT_SLOTSIZE);
    tm->tblcat_layout = new_layout(tblcat_schema);

    Schema *fldcat_schema = new_schema();
    add_string_field_to_schema(fldcat_schema, FLDCAT_TBLNAME, MAX_TABLENAME_SIZE);
    add_string_field_to_schema(fldcat_schema, FLDCAT_FLDNAME, MAX_FIELDNAME_SIZE);
    add_int_field_to_schema(fldcat_schema, FLDCAT_TYPE);
    add_int_field_to_schema(fldcat_schema, FLDCAT_LENGTH);
    add_int_field_to_schema(fldcat_schema, FLDCAT_OFFSET);
    tm->fldcat_layout = new_layout(fldcat_schema);

    return tm;
}

/**
 * @brief   create_TableManager呼出し後に，そのtmが新規であったら呼び出す
 * @param   tm 作成したtm
 * @param   tx tblcatとfldcatを作成するトランザクション
 * @return
 * @note
 * tblcatとfldcatというテーブルを作成する.
 */
void tm_save_TableManager(TableManager* tm, Transaction *tx) {
    tm_create_table(tm, TBLCAT, tm->tblcat_layout->schema, tx);
    tm_create_table(tm, FLDCAT, tm->fldcat_layout->schema, tx);
}

/**
 * @brief   TableManagerのメモリを解放する
 * @param   tm
 * @return
 * @note
 */
void free_TableManager(TableManager *tm) {
    free_layout(tm->tblcat_layout);
    free_layout(tm->fldcat_layout);
}

/**
 * @brief   tableをtblcat，fldcatに登録する
 * @param   tm TableManager
 * @param   tblname 作成するtableのtable名
 * @param   sch 作成するtableのschema
 * @param   tx tableを作成するtransaction
 * @return
 * @note
 * @todo
 * tblcat, fldcatの文字列を定数にする
 */
void tm_create_table(TableManager *tm, char *tblname, Schema *sch, Transaction *tx) {
    int tblname_size = strlen(tblname);
    if (tblname_size > MAX_TABLENAME_SIZE) {
        fprintf(stderr, "error: table name[%s](%d) is too logn.", tblname, tblname_size);
        exit(1);
    }

    // ここで作成するtabaleのlayout
    Layout *layout = new_layout(sch);

    TableIterator *tblcat_itr = new_TableIterator(tx, tm->tblcat_layout, TBLCAT);
    tblitr_insert(tblcat_itr);
    tblitr_set_string_to_current_slot(tblcat_itr, TBLCAT_TBLNAME, tblname, strlen(tblname));
    tblitr_set_int_to_current_slot(tblcat_itr, TBLCAT_SLOTSIZE, layout->slotsize);
    free_TableIterator(&tblcat_itr);

    TableIterator *fldcat_itr = new_TableIterator(tx, tm->fldcat_layout, FLDCAT);
    Field *now = sch->fields;
    while (now != NULL) {
        tblitr_insert(fldcat_itr);
        tblitr_set_string_to_current_slot(fldcat_itr, FLDCAT_TBLNAME, tblname, strlen(tblname));
        tblitr_set_string_to_current_slot(fldcat_itr, FLDCAT_FLDNAME, now->fieldname, strlen(now->fieldname));
        tblitr_set_int_to_current_slot(fldcat_itr, FLDCAT_TYPE, now->type);
        tblitr_set_int_to_current_slot(fldcat_itr, FLDCAT_LENGTH, now->length);
        tblitr_set_int_to_current_slot(fldcat_itr, FLDCAT_OFFSET, get_offset_layout(layout, now->fieldname));
        now = now->next;
    }
    free_TableIterator(&fldcat_itr);
}

/**
 * @brief   tableのlayoutを取得する
 * @param   tm TableManager
 * @param   target_tblname layoutを取得するtableのtable名
 * @param   tx layoutを取得する操作を行うtransaction
 * @return  取得したlayout，存在しないtarget_tblnameならNULL
 * @note
 */
Layout *tm_get_layout(TableManager *tm, char *target_tblname, Transaction *tx) {
    // target_tblnameのslotsizeをtblcatから取得する
    int slotsize = get_slotsize_from_tblcat(tm, target_tblname, tx);
    // target_tblnameが存在しない場合
    if (slotsize == -1) {
        return NULL;
    }

    // target_tblnameのlayoutを取得する
    return get_layout_from_fldcat(tm, target_tblname, slotsize, tx);
}

/**
 * @brief   tblcatからtable名のslotsizeを取得する
 * @param   tm TableManager
 * @param   target_tblname table名
 * @param   tx 実行するトランザクション
 * @return  target_tblnameのslotsize
 * @note
 * 
 * @attention
 */
static int get_slotsize_from_tblcat(TableManager *tm, char *target_tblname, Transaction *tx) {
    int slotsize = -1;

    TableIterator *tblcat_itr = new_TableIterator(tx, tm->tblcat_layout, TBLCAT);
    while (tblitr_go_to_next_record(tblcat_itr)) {
        char now_tblname[MAX_TABLENAME_SIZE];
        tblitr_get_string_from_current_slot(tblcat_itr, TBLCAT_TBLNAME, now_tblname);

        if (strcmp(target_tblname, now_tblname) == 0) {
            slotsize = tblitr_get_int_from_current_slot(tblcat_itr, TBLCAT_SLOTSIZE);
            break;
        }
    }
    free_TableIterator(&tblcat_itr);

    return slotsize;
}

/**
 * @brief   fldcatからtable名のfield情報を取得し，そのlayoutを作成する
 * @param   tm TableManager
 * @param   target_tblname 取得するlayoutのtable名
 * @param   slotsize 取得するlayoutのslotsize
 * @param   tx 実行するトランザクション
 * @return  target_tblnameのlayout
 * @note
 * @todo
 * tblnameが存在しない場合のコードの追加assert
 */
static Layout *get_layout_from_fldcat(TableManager *tm, char *target_tblname, int slotsize, Transaction *tx) {
    Schema *sch = new_schema();
    TableIterator *fldcat_itr = new_TableIterator(tx, tm->fldcat_layout, FLDCAT);
    StringIntList *offsets = NULL;

    while (tblitr_go_to_next_record(fldcat_itr)) {
        char now_tblname[MAX_TABLENAME_SIZE];
        tblitr_get_string_from_current_slot(fldcat_itr, FLDCAT_TBLNAME, now_tblname);
        if (strcmp(target_tblname, now_tblname) == 0) {
            // fieldの要素を抽出する
            char fldname[MAX_FIELDNAME_SIZE];
            tblitr_get_string_from_current_slot(fldcat_itr, FLDCAT_FLDNAME, fldname);
            int fldtype = tblitr_get_int_from_current_slot(fldcat_itr, FLDCAT_TYPE);
            int fldlen = tblitr_get_int_from_current_slot(fldcat_itr, FLDCAT_LENGTH);
            int offset = tblitr_get_int_from_current_slot(fldcat_itr, FLDCAT_OFFSET);
            add_StringIntList(&offsets, fldname, offset);
            add_field_to_schema(sch, fldname, fldtype, fldlen);
        }
    }
    free_TableIterator(&fldcat_itr);

    return new_layout_already(sch, offsets, slotsize);
}
