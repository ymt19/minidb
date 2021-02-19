#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "field_list.h"


/**
 * @brief   指定のfieldのリストに新しいfieldを追加する
 * @param   head fieldのリストの先頭要素を指すポインタのポインタ
 * @param   filedname field名(文字列長はMAX_STRING_SIZE以下 かつ 末尾に終端記号を含む)
 * @param   filename_size field名のサイズ
 * @param   type fieldの型
 * @param   length VARCHARの場合fieldの最大文字列長、INTEGERの場合任意の数値
 * @return
 * @note
 * fieldname_sizeがMAX_STRING_SIZEを超えていた場合を想定しない。
 * @todo
 * 同じfieldnameの場合の処理。
 */
void add_field(Field **head, char *fieldname, FieldType type, int lenght) {
    Field *new_field = malloc(sizeof(Field));
    snprintf(new_field->fieldname, MAX_STRING_SIZE, "%s", fieldname);
    new_field->type = type;
    if (type == FLD_INTEGER) {
        new_field->length = 0;
    } else if (type == FLD_VARCHAR) {
        new_field->length = lenght;
    }
    new_field->next = NULL;

    // new_fieldを追加する
    if (*head == NULL) {          // schemaにfieldがない場合
        *head = new_field;
    } else {
        Field *now = *head;
        while (now->next != NULL) {     // すでにschemaにいくつかfieldがある場合
            now = now->next;
        }
        now->next = new_field;
    }
}

/**
 * @brief   Field(copy)のリストにField(original)のリストの全ての要素を追加する
 * @param   copy 追加されるリストの先頭を表すポインタ
 * @param   original 追加される要素を持つリストの先頭を表すポインタ
 * @return
 * @note
 */
void add_all_field(Field **copy, Field *original) {
    Field *now_original = original;
    while (now_original != NULL) {
        add_field(copy, now_original->fieldname, now_original->type, now_original->length);
        now_original = now_original->next;
    } 
}

/**
 * @brief   fieldのリストから指定されたfieldnameのノードを見つける
 * @param   head fieldのリストの先頭を示すポインタ
 * @param   fieldname リストから見つけるfieldname
 * @return  fieldnameを持つ指定されたリストのノード
 * @note
 */
Field *search_field(Field *head, char *fieldname) {
    Field *now = head;
    while (now != NULL) {
        if (strcmp(now->fieldname, fieldname) == 0) {
            return now;
        }
        now = now->next;
    }
    return NULL;
}
