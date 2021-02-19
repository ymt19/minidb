#include <stdlib.h>
#include "layout.h"
#include "schema.h"
#include "field_list.h"
#include "string_int_list.h"

Layout *new_layout(Schema *sch) {
    Layout *layout = malloc(sizeof(Schema));
    if (layout == NULL) {
        return NULL;
    }

    layout->schema = sch;
    layout->offsets = NULL;
    // layout->offsetsの計算
    int pos = sizeof(int);   // 空かどうかのflag
    Field *now = sch->fields;
    while (now != NULL) {
        add_StringIntList(&(layout->offsets), now->fieldname, pos);
        if (now->type == FLD_INTEGER) {
            pos += sizeof(int);
        } else if (now->type == FLD_VARCHAR) {
            pos += sizeof(int) + now->length;
        }
        now = now->next;
    }
    layout->slotsize = pos;

    return layout;
}

/**
 * @brief   fieldnameのrecord pageのoffsetを得る
 * @param   layout fieldnameのfieldを持つlayout
 * @param   fieldname offsetを得るfield名
 * @return  fieldnameのoffset
 * @note
 * 
 * @todo
 * fieldnameのfieldが存在しない場合の処理の追加
 */
int get_offset_layout(Layout *layout, char *fieldname) {
    StringIntList *node = search_from_string_StringIntList(layout->offsets, fieldname);
    return node->num;
}