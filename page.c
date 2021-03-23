#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "page.h"

/**
 * @brief   構造体Pageのメモリ領域を確保する。
 * @param   (data_size) Pageのデータの大きさ
 * @return  成功したら、確保したメモリ領域
 * @return  失敗したら、NULL
 * @note
 * 構造体Pageとそのメンバdataをdata_size分の
 * 領域を確保する。
 * 
 * @attention
 */
Page* new_page(int data_size) {
    Page* page = malloc(sizeof(Page));
    if (page == NULL) {
        return NULL;
    }

    page->size = data_size;

    page->data = malloc(data_size);
    if (page->data == NULL) {
        free(page);
        return NULL;
    }

    clear_page(page);
    return page;
}

Page *new_page_bytes(char *bytes, int data_size) {
    Page *page = malloc(sizeof(Page));
    if (page == NULL) {
        return NULL;
    }

    page->size = data_size;

    page->data = malloc(data_size);
    if (page->data == NULL) {
        free(page);
        return NULL;
    }

    memcpy(page->data, bytes, data_size);
    return page;
}

/**
 * @brief   指定するPageのデータの内容を消去する。
 * @param   (page) 指定するPage
 * @note
 * 指定するPageのデータを0で埋めることで
 * データの内容を消去する。
 * 
 * @attention
 */
void clear_page(Page *page) {
    memset(page->data, 0, page->size);
}

/**
 * @brief   指定したPageのoffsetから数値を抽出する
 * @param   (page) 指定するPage
 * @param   (offset) Pageの指定するoffset
 * @return  抽出した数値
 * @note
 * Pageの指定したoffsetにある数値を抽出する。
 * 
 * @attention
 * ここで指定されるoffsetは、数値が設定されていることを
 * 前提とする。
 */
int get_int_from_page(Page *page, int offset) {
    int value;  // 抽出する数値
    memcpy(&value, page->data + offset, sizeof(int));
    return value;
}

/**
 * @brief   指定のPageのoffsetに数値を書き込む
 * @param   (page) 書き込まれるPage
 * @param   (offset) 指定するoffset
 * @param   (value) 書き込む数値
 * @note
 * 指定したPageのoffsetに数値を書き込む。
 * 
 * @attention
 * ここで指定されるoffsetは、他のデータが書き込まれていたら
 * そのデータは消去され新しいデータが上書きされる。
 */
void set_int_to_page(Page *page, int offset, int value) {
    memcpy(page->data + offset, &value, sizeof(int));
}

/**
 * @brief   指定したPageのoffsetから文字列を抽出する
 * @param   (page) 指定するPage
 * @param   (offset) 指定するoffset
 * @param   (value) 抽出する文字列の返り値
 * @return  抽出した文字列の長さ('\0'は含めない)
 * @note
 * Page内の文字列 : <--文字列長(数値)--><----文字列---->
 * 
 * @attention
 * 抽出する文字列には、最後尾に'\0'が付け足される。
 */
int get_string_from_page(Page *page, int offset, char *value) {
    int length;     // 抽出する文字列の長さ

    // offsetの位置から文字列の長さを受信する
    length = get_int_from_page(page, offset);

    // offsetを文字列開始位置に移動する
    offset += sizeof(int);
    
    // valueにlength分の文字列を抽出する
    memcpy(value, page->data + offset, length);

    // 抽出した文字列の最後尾の次の文字列に'\0'を付ける
    value[length] = '\0';

    return length;
}

/**
 * @brief   指定したPageのoffsetに文字列を書き込む
 * @param   (page) 指定するPage
 * @param   (offset) 指定するoffset
 * @param   (value) 書き込む文字列
 * @param   (length) 書き込む文字列の長さ('\0'を含めない)
 * @return  書き込みに成功した文字列の長さ
 * @note
 * 文字列が指定したoffsetから書き込むとPage内に収まらない場合、
 * 書き込むことができる最大の長さだけPageに書き込む。
 * 
 * Pageに格納される文字列は、その文字列の長さと文字列自体が
 * 合わせて格納される。まず、offsetの位置に文字列の長さとなる
 * 数値が保存されている。これは、Page内において文字列がある
 * offsetを参照したとき、その文字列がPage内のどこまでの範囲
 * にあるのかを確認できるようにしているためである。
 * Page内の文字列 : <--文字列長(数値)--><----文字列---->
 * 
 * @attention
 * 書き込む文字列長を指定するとき'\0'を含めない。
 */
int set_string_to_page(Page *page, int offset, char *value, int length) {
    int possible_length = length;   // 書き込みすることができる文字列長

    // 指定されたoffsetからPageのデータサイズを超える文字列は書き込めない
    if (possible_length + sizeof(int) + offset > page->size)
        possible_length = page->size - sizeof(int) - offset;
    
    // 書き込むことができる文字列長が0より少ない場合
    if (possible_length < 0)
        return 0;

    // 書き込む文字列の長さとなる数値(possible_length)をPageのoffsetに書き込む
    set_int_to_page(page, offset, possible_length);
    offset += sizeof(int);

    // Pageのoffsetに文字列を書き込む
    memcpy(page->data + offset, value, possible_length);

    return possible_length;
}