#ifndef LOG_MANAGER_H
#define LGO_MANAGER_H

#include "file_manager.h"

typedef struct LogManager LogManager;
struct LogManager {
    char log_filename[MAX_FILENAME];
    Page *log_page;
    Block *current_blk;
    int latest_LSN;
    int last_written_LSN;
};

LogManager* new_LogManager(char[MAX_FILENAME]);
void log_flush_to_lsn(LogManager*, int);
int log_append(LogManager*, unsigned char*);

#endif