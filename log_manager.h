#pragma once

#include "file_manager.h"

typedef struct {
    char log_filename[MAX_FILENAME];
    Page *log_page;
    Block *current_blk;
    int latest_LSN;
    int last_written_LSN;
} LogManager;

LogManager* new_LogManager(char[MAX_FILENAME]);
void log_flush_to_lsn(LogManager*, int);
int log_append(LogManager*, unsigned char*);