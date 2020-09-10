#pragma once

#include "file_manager.h"

/**
 * @struct  LogManager
 * @brief   LogManagerの構造体
 * @note
 */
typedef struct {
    // FileManager
    FileManager *fm;

    //log file名
    char log_filename[MAX_FILENAME];

    // log page
    Page *log_page;

    // 現在のlog pageを書き込むBlock
    Block *current_blk;

    // 最新の更新されたLSN
    int latest_LSN;

    // log fileに書き込まれている最新のLSN
    int last_written_LSN;
} LogManager;

LogManager* new_LogManager(FileManager*, char*);
void lm_flush_log_to_lsn(LogManager*, int);
int lm_append_log(LogManager*, char*, int);