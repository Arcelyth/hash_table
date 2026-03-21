#include "hashtable_sc.h"
#include <stdio.h>
#include <assert.h>

HT_INIT(TEST_TABLE, char*, int);

int main() {
    HT_TEST_TABLE table;
    TEST_TABLE_INIT(&table);

    TEST_TABLE_INSERT(&table, "Apple", 1);
    TEST_TABLE_INSERT(&table, "Banana", 2);
    TEST_TABLE_INSERT(&table, "Cherry", 3);

    HT_TEST_TABLE_ITER iter;
    TEST_TABLE_INTO_ITER(&table, &iter);

    printf("Iterating table:\n");
    while (iter.next != NULL) {
        printf("Key: %s, Value: %d\n", iter.next->key, iter.next->value);
        TEST_TABLE_NEXT(&iter);
    }

    TEST_TABLE_DESTROY(&table);
    return 0;
}
