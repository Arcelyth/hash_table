#include "hashtable_sc.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

HT_INIT(TEST_TABLE, char*, int); 

int main() {
    HT_TEST_TABLE table;
    HT_TEST_TABLE table2;
    TEST_TABLE_INIT(&table);
    TEST_TABLE_INIT(&table2);
    TEST_TABLE_INSERT(&table, "one", 1);
    TEST_TABLE_INSERT(&table, "two", 2);

    assert(TEST_TABLE_CONTAINS(&table, "one"));
    assert(!TEST_TABLE_CONTAINS(&table, "three"));

    assert(TEST_TABLE_REMOVE(&table, "one") == HT_OK);
    assert(!TEST_TABLE_CONTAINS(&table, "one"));

    int val;
    assert(TEST_TABLE_GET(&table, "two", &val) == HT_OK);
    assert(val == 2);

    assert(!TEST_TABLE_IS_EMPTY(&table));

    assert(TEST_TABLE_COPY(&table, &table2) == HT_OK);
    assert(TEST_TABLE_CONTAINS(&table2, "two"));

    TEST_TABLE_CLEAR(&table);
    assert(!TEST_TABLE_CONTAINS(&table, "two"));
    assert(TEST_TABLE_IS_EMPTY(&table));
    TEST_TABLE_INSERT(&table, "one", 1);
    int one;
    assert(TEST_TABLE_GET(&table, "one", &one) == HT_OK);
    assert(one == 1);
    TEST_TABLE_INSERT(&table, "one", 10);
    assert(TEST_TABLE_GET(&table, "one", &one) == HT_OK);
    assert(one == 10);


    TEST_TABLE_DESTROY(&table);
    printf("[PASS] test_sc\n");
    return 0;
}
