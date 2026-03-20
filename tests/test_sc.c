#include "hashtable_sc.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

HT_INIT(TEST_TABLE, char*, int); 

int contains_key(char** keys, size_t count, const char* target) {
    for (size_t i = 0; i < count; i++) {
        if (strcmp(keys[i], target) == 0) {
            return 1;
        }
    }
    return 0;
}

int contains_value(int* values, size_t count, const int target) {
    for (size_t i = 0; i < count; i++) {
        if (values[i] == target) {
            return 1;
        }
    }
    return 0;
}


int main() {
    HT_TEST_TABLE table;
    HT_TEST_TABLE table2;
    TEST_TABLE_INIT(&table);
    TEST_TABLE_WITH_CAPACITY(&table2, 10);

    // insert
    TEST_TABLE_INSERT(&table, "one", 1);
    TEST_TABLE_INSERT(&table, "two", 2);

    // contains
    assert(TEST_TABLE_CONTAINS(&table, "one"));
    assert(!TEST_TABLE_CONTAINS(&table, "three"));

    // remove
    assert(TEST_TABLE_REMOVE(&table, "one") == HT_OK);
    assert(!TEST_TABLE_CONTAINS(&table, "one"));

    // get
    int val;
    assert(TEST_TABLE_GET(&table, "two", &val) == HT_OK);
    assert(val == 2);

    // is_empty
    assert(!TEST_TABLE_IS_EMPTY(&table));

    // copy hashtable
    assert(TEST_TABLE_COPY(&table, &table2) == HT_OK);
    assert(TEST_TABLE_CONTAINS(&table2, "two"));

    // clear hashtable
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

    // get the number of hashtable's elements
    assert(TEST_TABLE_SIZE(&table) == (table.count == 1));

    TEST_TABLE_INSERT(&table, "three", 3);

    // get all the keys
    char** keys = TEST_TABLE_KEYS(&table); 
    assert(keys != NULL);
    assert(contains_key(keys, table.count, "one"));
    assert(contains_key(keys, table.count, "three"));
    free(keys);

    // get all the values
    int* values = TEST_TABLE_VALUES(&table); 
    assert(values != NULL);
    assert(contains_value(values, table.count, 10));
    assert(contains_value(values, table.count, 3));
    free(values);

    // destroy hashtable
    TEST_TABLE_DESTROY(&table);
    printf("[PASS] test_sc\n");
    return 0;
}
