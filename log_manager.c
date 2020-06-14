#include <string.h>
#include <stdlib.h>
#include "log_manager.h"

static void log_flush(LogManager*);
static Block* lm_append_newblk(LogManager*);

/**
 * Create Log Manager.
 */
LogManager* new_LogManager(char *log_filename) {
    LogManager* lm = malloc(sizeof(LogManager));
    if (lm == NULL) {
        return NULL;
    }

    strncpy(lm->log_filename, log_filename, sizeof(lm->log_filename));
    if ((lm->log_page = new_page()) == NULL) {
        return NULL;
    }

    int logsize = file_size(lm->log_filename);
    if (logsize == 0) {
        // log file is empty.
        lm->current_blk = append_newblk_lm(lm);
    } else {
        lm->current_blk = new_block(lm->log_filename, logsize - 1);
        fm_read_page_from_blk(lm->current_blk, lm->log_page);
    }

    lm->latest_LSN = 0;
    lm->last_written_LSN = 0;
}

/**
 * Write a log page to a file.
 */
void log_flush(LogManager *lm) {
    fm_write_page_to_blk(lm->current_blk, lm->log_page);
    lm->last_written_LSN = lm->latest_LSN;
}

/**
 * Write a log up to specified LSN to a file.
 */
void log_flush_to_lsn(LogManager *lm, int lsn) {
    if (lsn >= lm->last_written_LSN) {
        log_flush(lm);
    }
}

/**
 * Add a new block to the log file.
 */
Block* lm_append_newblk(LogManager *lm) {
    Block *block = fm_append_newblk(lm->log_filename);
    clear_page(lm->log_page);
    set_int_to_page(lm->log_page, 0, g_blksize);
    fm_write_page_to_blk(block, lm->log_page);
    return block;
}

/**
 * Add a log record from right to left.
 * The boundary is written to the first of the page.
 * @return positive number on success -1 on have problem with log page.
 */
int log_append(LogManager *lm, unsigned char* log_record) {
    int boundary, record_size, needed_size;
    if (get_int_from_page(lm->log_page, 0, &boundary) == 0) {
        return -1;
    }
    // size of log record
    record_size = sizeof(log_record);
    // size needed to write the record to page
    needed_size = record_size + sizeof(int);
    if (boundary - needed_size < sizeof(int)) {
        // record does not fit into page.
        log_flush(lm);
        lm->current_blk = lm_append_newblk(lm);
        get_int_from_page(lm->log_page, 0, &boundary);
    }

    // new boundary
    boundary -= needed_size;
    set_bytes_to_page(lm->log_page, boundary, log_record);
    set_int_to_page(lm->log_page, 0, boundary);
    lm->latest_LSN++;
    return lm->latest_LSN;
}