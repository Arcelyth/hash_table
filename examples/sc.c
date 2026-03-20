#include "hashtable_sc.h"
#include <stdio.h>
#include <string.h>

HT_INIT(MY_TABLE, char*, int); 

int main() {
    HT_MY_TABLE table;
    MY_TABLE_INIT(&table);
    MY_TABLE_INSERT(&table, "apple", 10);
    MY_TABLE_INSERT(&table, "banana", 20);


    int val;
    if (MY_TABLE_GET(&table, "apple", &val) == HT_OK) {
        printf("Apple's price is %d\n", val);
    }

    if (MY_TABLE_CONTAINS(&table, "banana")) {
        printf("Contains banana!\n");
    }
    MY_TABLE_REMOVE(&table, "apple");
    if (!MY_TABLE_CONTAINS(&table, "apple")) {
        printf("Not contains apple!\n");
    }

    printf("Now contains %ld fruits.\n", MY_TABLE_SIZE(&table));
    MY_TABLE_DESTROY(&table);
    return 0;
}
