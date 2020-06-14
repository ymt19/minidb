#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "file_manager.h"

int main(void) {
    int length;
    int pos1, pos2;
    char str1[] = "abcdefghijklmn";
    char str2[MAX_STRING_SIZE];
    unsigned int num1 = 1111;
    int num2;

    // This database has 400 byte blocks.
    FileManager *fm;
    fm = new_FileManager("tmp", 400);

    // ファイルにデータを書き込む
    // ファイル名tmp1のblknumが2のブロックを作成し
    // 新しいページを作成
    Block *blk = new_block("tmp1", 2);
    Page *page1 = new_page(fm->blksize);

    // page1のオフセットpos1を88としてそこにstr1をセットする
    pos1 = 88;
    length = set_string_to_page(page1, pos1, str1);
    // '\0'を含む文字列が格納される
    assert(length == strlen(str1)+1+sizeof(int));

    // page1のstr1をセットした直後にnum1をセットする
    pos2 = pos1 + length;
    length = set_int_to_page(page1, pos2, num1);
    assert(length == sizeof(int));

    // ページをブロック(file)に書き込む
    fm_write(fm, blk, page1);



    // ファイルからデータを読み込む
    // 上の書き込みに使用したblockから読み込む
    Page *page2 = new_page(fm->blksize);
    fm_read(fm, blk, page2);

    // オフセットpos1を文字列として受け取る
    length = get_string_from_page(page2, pos1, str2);
    assert(length == (int)strlen(str2)+1);
    assert(strcmp(str1, str2) == 0);
    
    // オフセットpos2を数値として受け取る
    length = get_int_from_page(page2, pos2, &num2);
    assert(length == sizeof(int));
    assert(num1 == num2);
    return 0;
}