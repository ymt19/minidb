#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include "file_manager.h"

static unsigned char checksum(unsigned char*, int); 

/**
 * 構造体FileManagerのメモリを確保し、指定されたディレクトリに移動する。
 * 存在しないディレクトリの場合、そのディレクトリを作成する。
 * 
 * pathname     : ディレクトリ名
 * data_size    : Blockのデータの大きさ
 * @return checksum一致なら、FileManagerのポインタ
 * @return checksum不一致なら、NULL
 */
FileManager* new_FileManager(char *pathname, unsigned int data_size) {
    FileManager *fm;
    struct stat buf;

    // ディレクトリが存在しない場合
    if (stat(pathname, &buf) == -1) {
        if (mkdir(pathname, 0777) == -1) {
            perror("mkdir");
            exit(1);
        }
    }

    if (chdir(pathname) == -1) {
        perror("chdir");
        exit(1);
    }

    if ((fm = malloc(sizeof(FileManager))) == NULL) {
        return NULL;
    }

    fm->data_size = data_size;
    fm->checksum_size = sizeof(unsigned char);  // checksumとして1バイト
    fm->blk_size = fm->data_size + fm->checksum_size;
    return fm;
}

/**
 * 指定のBlockの情報を指定のPageに読み込む。
 * checksumを元に誤り検知を行う。
 * 
 * fm   : FileManager
 * blk  : 読み込み元のBlock情報
 * page : 読み込み先のPage
 * @return 成功したら、1
 * @return 失敗したら、0
 */
int fm_read(FileManager *fm, Block *blk, Page *page) {
    int fd;
    unsigned char *bytes;   // Blockの全データを格納
    unsigned char chsum;    // チェックサム

    if ((bytes = malloc(sizeof(unsigned char) * fm->blk_size)) == NULL) {
        perror("malloc");
        return 0;
    }

    if ((fd = open(blk->filename, O_RDONLY)) == -1) {
        perror("open");
        exit(1);
    }

    if (lseek(fd, blk->blk_number * fm->blk_size, SEEK_SET) == -1) {
        perror("lseek");
        exit(1);
    }

    // read変更 blk_size分
    if (read(fd, bytes, fm->blk_size) == -1) {
        perror("read");
        exit(1);
    }

    // Blockの末尾にあるchecksumの値を抽出する
    memcpy(&chsum, bytes + fm->data_size, sizeof(unsigned char));

    if (chsum == checksum(bytes, fm->data_size)) {
        // checksum一致
        memcpy(page->data, bytes, fm->data_size);
        free(bytes);
        close(fd);
        return 1;
    }

    // checksum不一致
    free(bytes);
    close(fd);
    return 0;
}

/**
 * 指定のBlockに指定のPageの情報を書き込む。
 * checksumを元に誤り検知を行う。
 * 
 * fm   : FileManager
 * blk  : 書き込み先のBlock情報
 * page : 書き込み元のPage
 */
void fm_write(FileManager *fm, Block *blk, Page *page) {
    int fd;
    unsigned char chsum;     // 書き込みをしたデータに付けたチェックサム

    if ((fd = open(blk->filename, O_WRONLY | O_CREAT, 0777)) == -1) {
        perror("open");
        exit(1);
    }
    if (lseek(fd, blk->blk_number * fm->blk_size, SEEK_SET) == -1) {
        perror("lseek");
        exit(1);
    }
    // データ部への書き込み
    // writeAllの追加
    if (write(fd, page->data, fm->data_size) == -1) {
        perror("write");
        exit(1);
    }
    // チェックサム値の書き込み
    chsum = checksum(page->data, fm->data_size);
    if (write(fd, &chsum, fm->checksum_size) == -1) {
        perror("write");
        exit(1);
    }

    fsync(fd);
    close(fd);
}

/**
 * fileに新しいBlockを加える。
 * 
 * fm       : FileManager
 * filename : Blockを追加するfile名
 * @return 成功したら、追加したBlockを指すポインタ
 * @return 失敗したら、NULL
 */
Block* fm_append_newblk(FileManager *fm, char *filename) {
    int fd;
    int new_blk_number;     // 追加するBlockのblock number
    Block *blk;             // 追加するBlock
    unsigned char *bytes;   // 追加するBlockに書き込むデータ

    // fileのブロックサイズが、次に追加するBlockのblock number
    new_blk_number = fm_file_size(fm, filename);
    blk = new_block(filename, new_blk_number);
    bytes = calloc(1, sizeof(unsigned char) * fm->blk_size);

    if ((fd = open(filename, O_WRONLY | O_CREAT, 0777)) == -1) {
        perror("open");
        exit(1);
    }
    if (lseek(fd, new_blk_number * fm->blk_size, SEEK_SET) == -1) {
        perror("lseek");
        exit(1);
    }
    if (write(fd, bytes, fm->blk_size) == -1) {
        perror("read");
        exit(1);
    }
    close(fd);

    free(bytes);
    return blk;
}

/**
 * 指定のfileのBlockサイズを返す。
 * 
 * fm       : FileManager
 * filename : ファイル名
 * @return 成功したら、そのサイズ
 * @return 失敗したら、0
 */
int fm_file_size(FileManager *fm, char *filename) {
    struct stat buf;
    if (stat(filename, &buf) == -1) {
        // file doesnt exist or file access erorr
        return 0;
    }
    return (int)(buf.st_size/fm->blk_size);
}

/**
 * CheckSum(8bit)の計算を行う。
 * データ全体を8bitずつに分け、それぞれのbit位置に
 * 1が奇数個あれば1に、1が偶数個あれば0を取る値がchecksumとなる。
 * 
 * bytes    : Blockのデータ部
 * @return CheckSumの値
 */
unsigned char checksum(unsigned char *bytes, int size) {
    int i;
    unsigned char val;
    
    val = 0;
    for (i = 0; i < size; i++)
        val ^= bytes[i];
    
    return val;
}