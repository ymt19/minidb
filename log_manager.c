#include <string.h>
#include <stdlib.h>

#include "log_manager.h"

static void log_flush(LogManager*);

LogManager* new_LogManager(char log_filename[MAX_FILENAME]) {
    LogManager* lm = malloc(sizeof(LogManager));
    if (lm == NULL) {
        return NULL;
    }

    strncpy(lm->log_filename, log_filename, sizeof(lm->log_filename));
    if ((lm->log_page = new_page()) == NULL) {
        return NULL;
    }
    // lm->current_block
    lm->current_blk = 0;
    lm->last_written_LSN = 0;
}

void log_flush(LogManager *lm) {
    write_page_to_blk(lm->current_blk, lm->log_page);
    lm->last_written_LSN = lm->latest_LSN;
}

void log_flush_to_lsn(LogManager *lm, int lsn) {
    if (lsn >= lm->last_written_LSN) {
        log_flush(lm);
    }
}