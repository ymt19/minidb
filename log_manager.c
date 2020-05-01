#include <string.h>
#include <stdlib.h>

#include "log_manager.h"

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
    lm->last_saved_LSN = 0;
}