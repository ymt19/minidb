#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "file_manager.h"

int main(void) {
    int length, pos1, pos2;
    char str1[] = "abcdefghijklmn";
    char str2[MAX_STRING_SIZE];
    int num1 = 1111, num2;
    FileManager *fm;
    Block *blk;
    Page *page1, *page2;

    // データサイズを400バイトBlockと定義する。
    fm = new_FileManager("tmp_test_file_manager", 400);

    // ファイル名tmp1のblknumが2のBlockを作成し、
    // 新しいPageを作成する。
    blk = new_block("tmp1", 2);
    page1 = new_page(fm->data_size);

    // page1のオフセットpos1を88としてそこにstr1をセットする。
    pos1 = 88;
    length = set_string_to_page(page1, pos1, str1, strlen(str1));
    assert(length == strlen(str1));

    // page1のstr1をセットした直後にnum1をセットする
    pos2 = pos1 + length + sizeof(int);
    set_int_to_page(page1, pos2, num1);

    // ページをブロック(file)に書き込む
    fm_write(fm, blk, page1);

    // ファイルからデータを読み込む
    // 上の書き込みに使用したblockから読み込む
    page2 = new_page(fm->data_size);
    if (fm_read(fm, blk, page2) == 0) {
        fprintf(stderr, "failuer: fm_read\n");
        exit(1);
    }

    // オフセットpos1を文字列として受け取る
    length = get_string_from_page(page2, pos1, str2);
    assert(length == strlen(str2));
    assert(strcmp(str1, str2) == 0);

    // オフセットpos2を数値として受け取る
    num2 = get_int_from_page(page2, pos2);
    assert(num1 == num2);
    return 0;
}