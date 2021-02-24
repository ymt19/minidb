#include <stdlib.h>
#include <stdio.h>
#include "schema.h"

/**
 * @brief   構造体Schemaのメモリを確保する
 * @param
 * @return  動的確保したメモリのポインタ
 * @note
 */
Schema *new_schema() {
    Schema *sch = malloc(sizeof(Schema));
    if (sch == NULL) {
        return NULL;
    }
    
    sch->fields = NULL;

    return sch;
}

/**
 * @brief   指定のschemaにINTEGERのfieldを追加する
 * @param   sch 指定のschema
 * @param   fieldname 追加するfield名(文字列長はMAX_STRING_SIZE以下 かつ 末尾に終端記号を含む)
 * @return
 * @note
 */
void add_int_field_to_schema(Schema *sch, char *fieldname) {
    add_field(&(sch->fields), fieldname, FLD_INTEGER, 0);
}

/**
 * @brief   指定のschemaにVARCHARのfieldを追加する
 * @param   sch 指定のschema
 * @param   fieldname 追加するfield名(文字列長はMAX_STRING_SIZE以下 かつ 末尾に終端記号を含む)
 * @param   length 追加するfield(VARCHAR)の最大文字数
 * @return
 * @note
 */
void add_string_field_to_schema(Schema *sch, char *fieldname, int lenght) {
    add_field(&(sch->fields), fieldname, FLD_VARCHAR, lenght);
}

/**
 * @brief   指定のschemaに他のschemaのfieldを追加する
 * @param   copy fieldを追加するコピー先のschema
 * @param   fieldname 追加するfield名(文字列長はMAX_STRING_SIZE以下 かつ 末尾に終端記号を含む)
 * @param   original fieldnameのfieldを持つコピー元となるschema
 * @return
 * @note
 * original_schが指定したfieldnameのfieldを持たないことを想定しない。
 */
void add_schema_field_to_schema(Schema *copy, char *fieldname, Schema *original) {
    Field *field = search_field(original->fields, fieldname);
    add_field(&(copy->fields), field->fieldname, field->type, field->length);
}

/**
 * @brief   指定のschemaに他のschemaのすべてのfieldを追加する
 * @param   copy fieldを追加するコピー先のschema
 * @param   original コピー元となるschema
 * @return
 * @note
 */
void add_all_schema_field_to_schema(Schema *copy, Schema *original) {
    add_all_field(&(copy->fields), original->fields);
}

/**
 * @brief   schemaが指定のfield名のfieldを持つか確認する
 * @param   schema schema
 * @param   fieldname 確認するfield名
 * @return  存在するなら1、存在しないなら0
 * @note
 */
int has_field_schema(Schema *schema, char *fieldname) {

}