#pragma once

#include "page.h"

/**
 * @enum
 * @brief   fieldの型の種類
 */
enum FieldType {
    FLD_INTEGER,    // -2147483648 ～ 2147483647
    FLD_VARCHAR,    // サイズMAX_STRING_SIZEまでの文字列
};
typedef enum FieldType FieldType;

/**
 * @struct  Field
 * @note
 * schemaを表すfieldのリストのノードを表す。
 */
struct Field {
    // field名
    // 文字列末尾に終端記号が含まれる
    char fieldname[MAX_STRING_SIZE+1];

    // fieldの型
    FieldType type;

    // INTEGERの場合、0
    // VARCHARの場合、その最大文字数
    int length;

    // 次のfield
    struct Field *next;
};
typedef struct Field Field;


void add_field(Field **head, char *fieldname, FieldType type, int lenght);
void add_all_field(Field **copy, Field *original);
Field *search_field(Field *head, char *fieldname);