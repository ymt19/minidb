#include <assert.h>
#include <stdio.h>
#include "int_list.h"

int main() {
    IntList *list = NULL;

    // listは空の状態[]
    assert(search_IntList(list, 10) == 0);
    assert(search_IntList(list, 20) == 0);

    list = add_IntList(list, 10);
    // listは[10]
    assert(search_IntList(list, 10) == 1);
    assert(search_IntList(list, 20) == 0);

    list = add_IntList(list, 20);
    // listは[10, 20]
    assert(search_IntList(list, 10) == 1);
    assert(search_IntList(list, 20) == 1);

    free_all_IntList(&list);
    // listは[]
    assert(search_IntList(list, 10) == 0);
    assert(search_IntList(list, 20) == 0);
    assert(list == NULL);
}