#pragma once

#include "schema.h"
#include "string_int_list.h"

/**
 * @struct  Layout
 * @note
 * recordの物理情報(record pageの情報)を持つ。
 */
struct Layout {
    // このlayoutを構成するschema
    Schema *schema;
    
    // それぞれのfieldに対するrecordのオフセット
    StringIntList *offsets;

    // page内においての1つのrecordのサイズ
    int slotsize;
};
typedef struct Layout Layout;


Layout *new_layout(Schema *sch);
int get_offset_layout(Layout *layout, char *fieldname);
