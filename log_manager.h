#ifndef LOG_MANAGER_H
#define LGO_MANAGER_H

#include "block.h"
#include "page.h"

typedef struct LogManager LogManager;
struct LogManager {
    char log_filename[MAX_FILENAME];
    Page *log_page;
    Block *current_blk;
    int latest_LSN;
    int last_saved_LSN;
};

#endif