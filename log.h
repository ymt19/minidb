#pragma once

#include "log.h"
#include "log_manager.h"
#include "transaction.h"


/**
 * @enum    LogType
 * @brief   log recordの種類
 * @note    undo-only loggingを用いる
 */
enum LogType {
    START_LOG,
    COMMIT_LOG,
    ROLLBACK_LOG,
    SETINT_LOG,
    SETSTRING_LOG,
    CHECKPOINT_LOG,
    ERROR_LOG,
};
typedef enum LogType LogType;

extern int MAX_RECORD_SIZE;     // log recordの最大サイズ

int write_start_log(LogManager*, int);
int write_commit_log(LogManager*, int);
int write_rollback_log(LogManager*, int);
int write_set_int_log(LogManager*, int, Block*, int, int);
int write_set_string_log(LogManager*, int, Block*, int, char*, int);
int write_checkpoint_log(LogManager*);
void undo(Transaction*, char*, int);
LogType search_log_type(char*, int);
int search_log_txnum(char*, int);