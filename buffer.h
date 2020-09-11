#pragma once

#include "file_manager.h"
#include "log_manager.h"

/**
 * @struct  Buffer
 * @brief   Buffer構造体
 * @note
 */
typedef struct {
    // Bufferを管理するFileManager
    FileManager *fm;

    // Bufferを管理するLogManager
    LogManager *lm;

    // Bufferとして扱うPage
    Page *page;

    // Bufferとして扱うPageに相当するBlock
    Block *blk;

    // pinされている数
    int pins;

    // 変更したトランザクションのtxnum
    // -1の場合は、未変更
    int txnum;

    // Bufferが変更されたら更新されるLSN
    int lsn;
} Buffer;

Buffer* new_Buffers(FileManager*, LogManager*, int);
void buffer_modified(Buffer*, int, int);
int buffer_is_pinned(Buffer*);
void buffer_assign_to_block(Buffer *buff, Block *blk)
void buffer_flush(Buffer*);
void buffer_pin(Buffer*);
void buffer_unpin(Buffer*);