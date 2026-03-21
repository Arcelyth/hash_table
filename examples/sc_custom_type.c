#include "hashtable_sc.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

typedef struct Fruit {
    float price;
    float weight;
} Fruit;

// use struct type as value
HT_INIT(FRUIT_TABLE, char*, Fruit); 

typedef struct Person {
    char* name;
} Person;

uint32_t person_hash(Person* p) {
    return _ht_djb2_internal(p->name, strlen(p->name));
}

bool person_eq(Person* a, Person* b) {
    return strcmp(a->name, b->name) == 0;
}

// use struct type as key you need use struct's pointer as key's type
HT_INIT(PERSON_TABLE, Person*, int);

int main() {
    HT_FRUIT_TABLE table;
    FRUIT_TABLE_INIT(&table);

    FRUIT_TABLE_INSERT(&table, "Apple", (Fruit){.price = 5.2, .weight = 4.3});

    Fruit apple;
    if (FRUIT_TABLE_GET(&table, "Apple", &apple) == HT_OK) {
        printf("apple's price is: %.2f, weight: %.2f\n", apple.price, apple.weight);
    }

    // specify the config of hashtable
    HT_PERSON_TABLE table2;
    HTConfig_PERSON_TABLE config = {.hash_fn = person_hash, .eq_fn = person_eq};
    PERSON_TABLE_WITH_CONFIG(&table2, &config);

    Person p1 = {.name = "Alice"};
    PERSON_TABLE_INSERT(&table2, &p1, 20);

    int age;
    Person search_key = {.name = "Alice"};
    if (PERSON_TABLE_GET(&table2, &search_key, &age) == HT_OK) {
        printf("Alice's age is: %d\n", age);
    }
    
    FRUIT_TABLE_DESTROY(&table);
    PERSON_TABLE_DESTROY(&table2);
}
