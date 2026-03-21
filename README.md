# Hash_table

Hashtable implementation in pure C macro.

## Usage 

```c
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
```

Iterating the hashtable:
```c
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
```

