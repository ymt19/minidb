#include <stdlib.h>
#include <stdio.h>
#include "table_iterator.h"
#include "record_id.h"
#include "record_page.h"

static void move_to_block(TableIterator *tblitr, int blknum);
static void move_to_new_block(TableIterator *tblitr);
static int at_last_block(TableIterator *tblitr);


/**
 * @brief   構造体TableIteratorのメモリを動的確保する
 * @param   tx transaction
 * @param   layout このtableのlayout
 * @param   tablename table名(文字列長はMAX_FILENAME-4以下)
 * @param   tablename_size table名の文字列長
 * @return
 * @note
 * tableの情報を持つファイルをtablename+".tblitr"として保存するため、
 * filenameのサイズ制限とは別の、tablenameのサイズ制限がある。
 */
TableIterator *new_TableIterator(Transaction *tx, Layout *layout, char *tablename, int tablename_size) {
    // ".tblitr"を含めたファイル名がMAX_FILENAMEより大きい場合
    if (tablename_size + 4 > MAX_FILENAME) {
        return NULL;
    }

    TableIterator *tblitr = malloc(sizeof(TableIterator));
    if (tblitr == NULL) {
        return NULL;
    }

    tblitr->tx = tx;
    tblitr->layout = layout;
    snprintf(tblitr->filename, MAX_FILENAME, "%s.tblitr", tablename);
    if (tx_filesize(tblitr->tx, tblitr->filename) == 0) { // fileが存在しない場合
        move_to_new_block(tblitr); // block number0を作り、それに移動する
    } else {
        move_to_block(tblitr, 0);  // block number0に移動する
    }
}

/**
 * @brief   TableIteratorのメモリを解放する
 * @param   tblitr TableIteratorのメモリを示すポインタ，NULLを格納する
 * @return
 * @note
 */
void free_TableIterator(TableIterator **tblitr) {
    TableIterator *target = *tblitr;
    if (target->rp != NULL) {
        free_RecordPage(&(target->rp));
    }
    free(target);
    target = NULL;
}

/**
 * @brief   TableIteratorのRecordPageの持つ情報をfileの最初のblockへ変更する
 * @param   tblitr TableIterator
 * @return
 * @note
 */
void move_first_block_TableIterator(TableIterator *tblitr) {
    move_to_block(tblitr, 0);
}

/**
 * @brief   Table全体として、current slot以降のUSEDなslotに移動する
 * @param   tblitr TableIterator
 * @return  USEDなslotがあれば1、存在しないなら0
 * @note
 */
int is_next_record_TableIterator(TableIterator *tblitr) {
    // 現在のRecordPage内でのcurrent slot以降のUSEDなslotへ移動
    tblitr->current_slot =  next_after_slot_RecordPage(tblitr->rp, tblitr->current_slot);

    while (tblitr->current_slot < 0) {
        if (at_last_block(tblitr)) {
            // USEDなslotが見つからない場合
            return 0;
        }
        move_to_block(tblitr, tblitr->rp->blk->blk_number + 1);
        tblitr->current_slot = next_after_slot_RecordPage(tblitr->rp, tblitr->current_slot);
    }
    // USEDなslotが見つかった場合
    return 1;
}

/**
 * @brief   TableIteratorのcurrent slotのfieldの数値を得る
 * @param   tblitr table iterator
 * @param   fieldname tblitrのINTEGERのfield名
 * @return
 * @note
 * fieldnameのfieldは、INTEGER型であり、そのTableItrのfieldでなければならない。それ以外の
 * fieldである場合は想定しない。
 * @todo
 * assert
 */
int get_int_from_TableIterator(TableIterator *tblitr, char *fieldname) {
    return get_int_from_RecordPage(tblitr->rp, tblitr->current_slot, fieldname);
}

/**
 * @brief   Tableのcurrent slotのfieldの文字列を得る
 * @param   tblitr table iterator
 * @param   fieldname tblのVARCHARのfield名
 * @return
 * @note
 * fieldnameのfieldは、VARCHAR型であり、そのTableのfieldでなければならない。それ以外の
 * fieldである場合は想定しない。
 * @todo
 * const char* fieldname
 */
int get_string_from_TableIterator(TableIterator *tblitr, char *fieldname, char *val) {
    return get_string_from_RecordPage(tblitr->rp, tblitr->current_slot, fieldname, val);
}

/**
 * @brief   Tableに指定のfieldが存在するか確認する
 * @param   tblitr table iterator
 * @param   fieldname 確認するfield名
 * @return  存在するなら1、存在しないなら0
 * @note
 */
int has_field_TableIterator(TableIterator *tblitr, char *fieldname) {
    return has_field_schema(tblitr->layout->schema, fieldname);
}

/**
 * @brief   Tableのcurrent slotのfieldに数値を書き込む
 * @param   tblitr table itr
 * @param   fieldname tblのINTEGERのfield名
 * @param   val 書き込む数値
 * @return
 * @note
 * fieldnameのfieldは、INTEGER型であり、そのTableIteratorのfieldでなければならない。それ以外の
 * fieldである場合は想定しない。
 */
void set_int_to_record_TableIterator(TableIterator *tblitr, char *fieldname, int val) {
    set_int_to_RecordPage(tblitr->rp, tblitr->current_slot, fieldname, val);
}

/**
 * @brief   TableIteratorのcurrent slotのfieldに文字列を書き込む
 * @param   tblitr table iterator
 * @param   fieldname tblitrのVARCHARのfield名
 * @param   val 書き込む文字列
 * @param   val_size 書き込む文字列のサイズ
 * @return
 * @note
 * fieldnameのfieldは、VARCHAR型であり、そのTableのfieldでなければならない。それ以外の
 * fieldである場合は想定しない。
 * 
 * val_sizeは、MAX_STRING_SIZEより小さい値でないといけない。それ以外の場合は想定しない。
 */
void set_string_to_record_TableIterator(TableIterator *tblitr, char *fieldname, char *val, int val_size) {
    set_string_to_RecordPage(tblitr->rp, tblitr->current_slot, fieldname, val, val_size);
}

/**
 * @brief   current slot以降のTableのEMPTYなslotに移動する
 * @param   tblitr table
 * @return
 * @note
 * もしcurrent slot以降の存在するBlockの全てのslotがUSEDなslotであった場合、新しいBlockを
 * 追加する。
 */
void insert_TableIterator(TableIterator *tblitr) {
    tblitr->current_slot = insert_after_slot_RecordPage(tblitr->rp, tblitr->current_slot);
    while (tblitr->current_slot < 0) {
        if (at_last_block(tblitr)) {
            move_to_new_block(tblitr);
        } else {
            move_to_block(tblitr, tblitr->rp->blk->blk_number + 1);
        }
        tblitr->current_slot = insert_after_slot_RecordPage(tblitr->rp, tblitr->current_slot);
    }
}

/**
 * @brief   current slotをEMPTYに設定する
 * @param   tblitr table
 * @return
 * @note
 */
void delete_slot_TableIterator(TableIterator *tblitr) {
    delete_slot_RecordPage(tblitr->rp, tblitr->current_slot);
}

/**
 * @brief   指定のRecordIDにcurrent slotを移動する
 * @param   tblitr table
 * @param   rid RecordID
 * @return
 * @note
 */
void move_to_rid_TableIterator(TableIterator *tblitr, RecordID rid) {
    if (tblitr->rp->blk->blk_number != rid.blknum) {
        if (tblitr->rp != NULL) {
            free_RecordPage(&(tblitr->rp));
        }
        Block *blk = new_block(tblitr->filename, rid.blknum);
        tblitr->rp = new_RecordPage(tblitr->tx, blk, tblitr->layout);
    }
    tblitr->current_slot = rid.slot;
}

/**
 * @brief   current slotのRecordIDを取得する
 * @param
 * @return
 * @note
 * 
 * @attention
 */
RecordID get_rid_TableIterator(TableIterator *tblitr) {

}

/**
 * @brief   指定のblock numberのBlockにcurrent slotを移動する
 * @param   tblitr table iterator
 * @param   blknum block number
 * @return
 * @note
 */
static void move_to_block(TableIterator *tblitr, int blknum) {
    if (tblitr->rp != NULL) {
        free_RecordPage(&(tblitr->rp));
    }
    Block *blk = new_block(tblitr->filename, blknum);
    tblitr->rp = new_RecordPage(tblitr->tx, blk, tblitr->layout);
    tblitr->current_slot = -1;
}

/**
 * @brief   fileに新しいBlockを追加し、そこにcurrent slotを移動する
 * @param   tblitr table iterator
 * @return
 * @note
 * @todo
 * current_slot -1の理由, 初期値
 */
static void move_to_new_block(TableIterator *tblitr) {
    if (tblitr->rp != NULL) {
        free_RecordPage(&(tblitr->rp));
    }
    Block *blk = tx_append_blk(tblitr->tx, tblitr->filename);
    tblitr->rp = new_RecordPage(tblitr->tx, blk, tblitr->layout);
    init_RecordPage(tblitr->rp);
    tblitr->current_slot = -1;
}

/**
 * @brief   tableのcurrent slotのBlockがファイルの最後のBlockであるのかを確認する
 * @param   tblitr table
 * @return  最後のBlockなら1、そうで無いなら0
 * @note
 */
static int at_last_block(TableIterator *tblitr) {
    return tblitr->rp->blk->blk_number == tx_filesize(tblitr->tx, tblitr->filename);
}