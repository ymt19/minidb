#pragma once

#include "recovery_manager.h"
#include "transaction.h"

void rm_start(Transaction*);
void rm_commit(Transaction*);
void rm_rollback(Transaction*);
void rm_recover(Transaction*);
int rm_set_int(Transaction*, Buffer*, int);
int rm_set_string(Transaction*, Buffer*, int);
