#include <stdlib.h>
#include "record_page.h"
#include "field_list.h"

static int search_after_slot(RecordPage *rp, int slot, RECORD_STATE_FLAG flag);
static void set_flag(RecordPage *rp, int slot, RECORD_STATE_FLAG flag);
static int get_offset(RecordPage *rp, int slot);
static int is_valid_slot(RecordPage *rp, int slot);

/**
 * @brief   構造体RecordPageのメモリを動的確保する
 * @param   tx transaction
 * @param   blk block
 * @param   layout layout
 * @return  確保した構造体RecordPageを指すポインタ
 * @note
 */
RecordPage *new_RecordPage(Transaction *tx, Block *blk, Layout *layout) {
    RecordPage *rp = malloc(sizeof(RecordPage));
    if (rp == NULL) {
        return NULL;
    }

    rp->tx = tx;
    rp->blk = blk;
    rp->layout = layout;

    return rp;
}

/**
 * @brief   RecordPageの指すblockの全てのslotのfieldの値を初期化する
 * @param   rp RecordPage
 * @return
 * @note
 * 初期化のため変更はlogに書き込まない
 */
void init_RecordPage(RecordPage *rp) {
    int slot = 0;
    // slotが有効な間、それぞれのfieldを初期化する
    while (is_valid_slot(rp, slot)) {
        tx_set_int(rp->tx, rp->blk, get_offset(rp, slot), EMPTY, 1);    // logに書き込まない
        Field *field = rp->layout->schema->fields;

        // 現在のslotの全てのfieldを初期化する
        while (field != NULL) {
            int fld_pos = get_offset(rp, slot) + get_offset_layout(rp->layout, field->fieldname);

            if (field->type == FLD_INTEGER) {
                tx_set_int(rp->tx, rp->blk, fld_pos, 0, 1);     // logに書き込まない
            } else if (field->type == FLD_VARCHAR) {
                tx_set_string(rp->tx, rp->blk, fld_pos, "", 0, 1); // logに書き込まない
            }
        }
        slot++;
    }
}

/**
 * @brief   slotの指定の数値をもつfieldの値を得る
 * @param   rp RecordPage
 * @param   slot slot
 * @param   fieldname field名(FLD_INTEGERとする)
 * @return  取得した数値
 * @note
 * fieldの型が文字列であった場合を想定しない。
 */
int get_int_from_RecordPage(RecordPage *rp, int slot, char *fieldname) {
    int fld_pos = get_offset(rp, slot) + get_offset_layout(rp->layout, fieldname);
    return tx_get_int(rp->tx, rp->blk, fld_pos);
}

/**
 * @brief   slotの指定の文字列をもつfieldの値を得る
 * @param   rp RecordPage
 * @param   slot slot
 * @param   fieldname field名(FLD_VARCHARとする)
 * @param   val 取得した文字列
 * @return  取得した文字列のサイズ
 * @note
 * fieldの型が数値であった場合を想定しない。
 */
int get_string_from_RecordPage(RecordPage *rp, int slot, char *fieldname, char *val) {
    int fld_pos = get_offset(rp, slot) + get_offset_layout(rp->layout, fieldname);
    return tx_get_string(rp->tx, rp->blk, fld_pos, val);
}

/**
 * @brief   slotの指定のfieldに数値を書き込む
 * @param   rp RecordPage
 * @param   slot slot
 * @param   fieldname field名(FLD_INTEGERとする)
 * @param   val 書き込む数値
 * @return  取得した数値
 * @note
 * fieldの型が文字列であった場合を想定しない。
 */
void set_int_to_RecordPage(RecordPage *rp, int slot, char *fieldname, int val) {
    int fld_pos = get_offset(rp, slot) + get_offset_layout(rp->layout, fieldname);
    tx_set_int(rp->tx, rp->blk, fld_pos, val, 0);
}

/**
 * @brief   slotの指定のfieldに数値を書き込む
 * @param   rp RecordPage
 * @param   slot slot
 * @param   fieldname field名(FLD_VARCHARとする)
 * @param   val 書き込む文字列を指すポインタ
 * @param   val_size 書き込む文字列のサイズ
 * @return  取得した数値
 * @note
 * fieldの型が文字列であった場合を想定しない。
 */
void set_string_to_RecordPage(RecordPage *rp, int slot, char *fieldname, char *val, int val_size) {
    int fld_pos = get_offset(rp, slot) + get_offset_layout(rp->layout, fieldname);
    tx_set_string(rp->tx, rp->blk, fld_pos, val, val_size, 0);
}

/**
 * @brief   slotのflagをEMPTYに設定する
 * @param   rp RecordPage
 * @param   slot slot
 * @return
 * @note
 */
void delete_record_RecordPage(RecordPage *rp, int slot) {
    set_flag(rp, slot, EMPTY);
}

/**
 * @brief   指定のslot以降のflagがUSEDな最初のslotを得る
 * @param   rp RecodPage
 * @param   slot 指定するslot
 * @return  見つけたslot、見つからなかったら-1
 * @note
 */
int next_after_slot_RecordPage(RecordPage *rp, int slot) {
    return search_after_slot(rp, slot, USED);
}

/**
 * @brief   指定のslot以降のflagがEMPTYな最初のslotのflagをUSEDに変更し、得る
 * @param   rp RecodPage
 * @param   slot 指定するslot
 * @return  見つけたslot、見つからなかったら-1
 * @note
 */
int insert_after_slot_RecordPage(RecordPage *rp, int slot) {
    int new_slot = search_after_slot(rp, slot, EMPTY);
    if (new_slot >= 0) {
        set_flag(rp, new_slot, USED);
    }
    return new_slot;
}

/**
 * @brief   指定のslot以降の指定のflagと同様な最初のslotを得る
 * @param   rp RecodPage
 * @param   slot 指定するslot
 * @param   flag 指定するflag
 * @return  見つけたslot、見つからなかったら-1
 * @note
 */
static int search_after_slot(RecordPage *rp, int slot, RECORD_STATE_FLAG flag) {
    slot++;
    while (is_valid_slot(rp, slot)) {
        if (tx_get_int(rp->tx, rp->blk, get_offset(rp, slot)) == flag) {
            return slot;
        }
        slot++;
    }
    return -1;
}

/**
 * @brief   slotに指定のflagを設定する
 * @param   rp RecordPage
 * @param   slot slot
 * @param   flag 設置するflag
 * @return
 * @note
 */
static void set_flag(RecordPage *rp, int slot, RECORD_STATE_FLAG flag) {
    tx_set_int(rp->tx, rp->blk, get_offset(rp, slot), flag, 0); // logに書き込む
}

/**
 * @brief   page内のslotの先頭のoffsetを取得する
 * @param   rp RecordPage
 * @param   slot slot
 * @return  取得したslotのoffset
 * @note
 */
static int get_offset(RecordPage *rp, int slot) {
    return rp->layout->slotsize * slot;
}

/**
 * @brief   slotが有効なものか確認する
 * @param   rp RecordPage
 * @param   slot 確認するslot number
 * @return  有効ならば真、それ以外なら偽
 * @note
 */
static int is_valid_slot(RecordPage *rp, int slot) {
    return get_offset(rp, slot+1) <= rp->tx->fm->blk_size;
}