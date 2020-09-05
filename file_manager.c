#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include "file_manager.h"

/**
 * 構造体FileManagerのメモリを確保し、指定されたディレクトリに移動する。
 * 存在しないディレクトリの場合、そのディレクトリを作成する。
 * 
 * pathname     : ディレクトリ名
 * data_size    : Blockのデータの大きさ
 * @return 成功したら、FileManagerのポインタ
 * @return 失敗したら、NULL
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

    if (fm = malloc(sizeof(FileManager))) == NULL) {
        return NULL;
    }

    // checksum用に1バイト追加する
    fm->blksize = data_size + 1;
    return fm;
}

/**
 * 指定のBlockの情報を指定のPageに読み込む。
 * 
 * fm   : FileManager
 * blk  : 読み込み元のBlock
 * page : 読み込み先のPage
 */
void fm_read(FileManager *fm, Block *blk, Page *page) {
    int fd;
    if ((fd = open(blk->filename, O_RDONLY)) == -1) {
        perror("open");
        exit(1);
    }

    if (lseek(fd, blk->blk_number * fm->blksize, SEEK_SET) == -1) {
        perror("lseek");
        exit(1);
    }

    if (read(fd, page->data, fm->blksize) == -1) {
        perror("read");
        exit(1);
    }

    close(fd);
}

/**
 * 指定のBlockに指定のPageの情報を書き込む。
 * 
 * fm   : FileManager
 * blk  : 書き込み先のBlock
 * page : 書き込み元のPage
 */
void fm_write(FileManager *fm, Block *blk, Page *page) {
    int fd;
    if ((fd = open(blk->filename, O_WRONLY | O_CREAT, 0777)) == -1) {
        perror("open");
        exit(1);
    }

    if (lseek(fd, blk->blk_number * fm->blksize, SEEK_SET) == -1) {
        perror("lseek");
        exit(1);
    }

    if (write(fd, page->data, fm->blksize) == -1) {
        perror("read");
        exit(1);
    }

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
    bytes = calloc(1, sizeof(unsigned char) * fm->blksize);

    if ((fd = open(filename, O_WRONLY | O_CREAT, 0777)) == -1) {
        perror("open");
        exit(1);
    }
    if (lseek(fd, new_blk_number * fm->blksize, SEEK_SET) == -1) {
        perror("lseek");
        exit(1);
    }
    if (write(fd, bytes, fm->blksize) == -1) {
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
    return (int)(buf.st_size/fm->blksize);
}