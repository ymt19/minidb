#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include "file_manager.h"

static unsigned char checksum(unsigned char*, int);
static void read_all(int, unsigned char*, unsigned int);
static void write_all(int, unsigned char*, unsigned int);

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
 * @note
 * 
 * @attention
 * open(2)のエラー発生時、ファイルが存在しない場合のみ処理を行う。それ以外
 * のエラーの場合、システムを終了させる。
 */
int fm_read(FileManager *fm, Block *blk, Page *page) {
    int fd;
    unsigned char bytes[fm->data_size];     // Pageの変更前のデータ
    unsigned char chsum;                    // チェックサム

    // fileが存在しない場合やそのBlockがまだfileに書き込まれていない場合は、
    // 成功を表す値を返し、Pageの内容をクリアする
    if (blk->blk_number >= fm_file_size(fm, blk->filename)) {
        clear_page(page);
        return 1;
    }

    // checksum不一致の場合、Pageを復元するために
    // Pageの変更前のデータを保存しておく
    memcpy(bytes, page->data, fm->data_size);

    if ((fd = open(blk->filename, O_RDONLY)) == -1) {
        perror("open");
        exit(1);
    }

    if (lseek(fd, blk->blk_number * fm->blk_size, SEEK_SET) == -1) {
        perror("lseek");
        exit(1);
    }

    // read変更 blk_size分
    // PageにBlockの内容を読み込む
    // if (read(fd, page->data, fm->data_size) == -1) {
    //     perror("read");
    //     exit(1);
    // }
    read_all(fd, page->data, fm->data_size);

    // Blockの末尾にあるchecksumの値を抽出する
    if (read(fd, &chsum, sizeof(unsigned char)) == -1) {
        perror("read");
        exit(1);
    }

    // checksum一致
    if (chsum == checksum(page->data, fm->data_size)) {
        close(fd);
        return 1;
    }

    // checksum不一致
    close(fd);
    // Pageの情報を復元する
    memcpy(page->data, bytes, fm->data_size);
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
    // if (write(fd, page->data, fm->data_size) == -1) {
    //     perror("write");
    //     exit(1);
    // }
    write_all(fd, page->data, fm->data_size);

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
 * fileの情報とそのfileの新しいblk_numを保持する構造体Blockを
 * 作成するだけで、実際にfileにblk_size分のデータが追加される
 * ことはない。
 * 
 * fm       : FileManager
 * filename : Blockを追加するfile名
 * @return 成功したら、追加したBlockを指すポインタ
 * @return 失敗したら、NULL
 */
Block* fm_append_newblk(FileManager *fm, char *filename) {
    // int fd;
    int new_blk_number;     // 追加するBlockのblock number
    Block *blk;             // 追加するBlock
    // unsigned char *bytes;   // 追加するBlockに書き込むデータ

    // fileのブロックサイズが、次に追加するBlockのblock number
    new_blk_number = fm_file_size(fm, filename);
    blk = new_block(filename, new_blk_number);
    // bytes = calloc(1, sizeof(unsigned char) * fm->blk_size);

    // if ((fd = open(filename, O_WRONLY | O_CREAT, 0777)) == -1) {
    //     perror("open");
    //     exit(1);
    // }

    // if (lseek(fd, new_blk_number * fm->blk_size, SEEK_SET) == -1) {
    //     perror("lseek");
    //     exit(1);
    // }

    // // if (write(fd, bytes, fm->blk_size) == -1) {
    // //     perror("write");
    // //     exit(1);
    // // }
    // write_all(fd, bytes, fm->blk_size);

    // close(fd);
    // free(bytes);
    return blk;
}

/**
 * 指定のfileのBlockサイズを返す。
 * fileに保存されているデータ量から、その値を計算する。
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

/**
 * 指定する範囲のデータをすべて読み込むことを保証する
 * read(2)のラッパー関数
 * 
 * fd       : ファイルディスクリプタ
 * bytes    : readするデータ
 * goal_size: readするデータサイズ
 */
void read_all(int fd, unsigned char *bytes, unsigned int goal_size) {
    int rd_size;            // 1回のread(2)で読み込まれたデータサイズ
    int total_rd_size = 0;  // 読み込まれたデータサイズの合計

    while (total_rd_size != goal_size) {
        if ((rd_size = read(fd, bytes + total_rd_size, goal_size - total_rd_size)) == -1) {
            perror("read");
            exit(1);
        }

        total_rd_size += rd_size;
    }
}

/**
 * 指定する範囲のデータをすべて書き込むことを保証する
 * write(2)のラッパー関数
 * 
 * fd       : ファイルディスクリプタ
 * bytes    : writeするデータ
 * goal_size: writeするデータサイズ
 */
void write_all(int fd, unsigned char *bytes, unsigned int goal_size) {
    int wr_size;             // 1回のwrite(2)で書き込まれたデータサイズ
    int total_wr_size = 0;   // 書き込まれたデータサイズの合計

    while (total_wr_size != goal_size) {
        if ((wr_size = write(fd, bytes + total_wr_size, goal_size - total_wr_size)) == -1) {
            perror("write");
            exit(1);
        }

        total_wr_size += wr_size;
    }
}