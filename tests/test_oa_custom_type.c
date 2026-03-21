#include "hashtable_oa.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

typedef struct Person {
    int age;
} Person;

// use struct type as value
HT_INIT(TEST_TABLE, char*, Person); 

uint32_t person_hash(Person* p) {
    return _ht_djb2_internal(&p->age, sizeof(p->age));
}

bool person_eq(Person* a, Person* b) {
    return a->age == b->age;
}

// use struct type as key 
HT_INIT(TEST_TABLE2, Person*, int);

int main() {
    HT_TEST_TABLE table;
    TEST_TABLE_INIT(&table);

    // insert
    Person p = {.age = 10};
    TEST_TABLE_INSERT(&table, "Peter", p);

    // contains
    assert(TEST_TABLE_CONTAINS(&table, "Peter"));

    HT_TEST_TABLE2 table2;
    HTConfig_TEST_TABLE2 config = {.hash_fn = person_hash, .eq_fn = person_eq};
    TEST_TABLE2_WITH_CONFIG(&table2, &config);
    Person p2 = {.age = 10};
    TEST_TABLE2_INSERT(&table2, &p2, 10);
    assert(TEST_TABLE2_CONTAINS(&table2, &p2));

    printf("[PASS] test_oa_custom_type\n");
}


