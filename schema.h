#pragma once

#include "field_list.h"
#include "page.h"

/**
 * @struct  Schema
 * @note
 * ユーザがテーブル作成時に用いる情報を持つ。
 * fieldの情報のリストを持つ。
 */
struct Schema {
    // 構造体Fieldのリストのヘッドポインタ
    Field *fields;
};
typedef struct Schema Schema;

Schema *new_schema();
void add_int_field_to_schema(Schema *sch, char *fieldname);
void add_string_field_to_schema(Schema *sch, char *fieldname, int lenght);
void add_schema_field_to_schema(Schema *copy, char *fieldname, Schema *original);
void add_all_schema_field_to_schema(Schema *copy, Schema *original);
int has_field_schema(Schema *schema, char *fieldname);
