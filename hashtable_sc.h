#ifndef HASH_TABLE_SC_H
#define HASH_TABLE_SC_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define HT_ERROR -1
#define HT_OK 0
#define HT_INIT_CAPACITY 8
#define HT_LOAD_FACTOR 0.75

typedef uint32_t (*hash_func_t)(const void* key, size_t len);

// djb2 string hashing algorithm
static inline uint32_t _ht_djb2_internal(const void* raw_key, size_t len) {
    uint32_t hash = 5381;
    const unsigned char* p = (const unsigned char*)raw_key;
    for (size_t i = 0; i < len; i++) {
        hash = ((hash << 5) + hash) ^ p[i];
    }
    return hash;
}

#define HT_AUTO_HASH(key) _Generic((key), \
    char*:              _ht_djb2_internal(key, strlen((char*)(uintptr_t)key)), \
    const char*:        _ht_djb2_internal(key, strlen((const char*)(uintptr_t)key)), \
    default:            _ht_djb2_internal(&(key), sizeof(key)) \
)

#define HT_AUTO_EQ(a, b) _Generic((a), \
    char*:              (strcmp((char*)(uintptr_t)a, (char*)(uintptr_t)b) == 0), \
    const char*:        (strcmp((const char*)(uintptr_t)a, (const char*)(uintptr_t)b) == 0), \
    default:            (memcmp(&(a), &(b), sizeof(a)) == 0) \
)

#define HT_INIT(table_name, k_type, v_type) \
    typedef uint32_t (*hash_func_##table_name)(k_type key); \
    typedef bool     (*eq_func_##table_name)(k_type a, k_type b); \
\
    typedef struct HTNode_##table_name { \
        k_type key; \
        v_type value; \
        uint32_t hash; \
        struct HTNode_##table_name* next; \
    } HTNode_##table_name; \
\
    typedef struct { \
        size_t count; \
        size_t capacity; \
        hash_func_##table_name hash_func; \
        eq_func_##table_name eq_func; \
        HTNode_##table_name** buckets; \
    } HT_##table_name; \
\
    static inline uint32_t _default_hash_##table_name(k_type key) { \
        return HT_AUTO_HASH(key); \
    } \
\
    static inline bool _default_eq_##table_name(k_type a, k_type b) { \
        return HT_AUTO_EQ(a, b); \
    } \
\
    static int _table_name##_RESIZE(HT_##table_name* table, size_t new_cap) {  \
        HTNode_##table_name** new_buckets = (HTNode_##table_name**)calloc(new_cap, sizeof(HTNode_##table_name*)); \
        if (!new_buckets) return HT_ERROR; \
        for (size_t i = 0; i < table->capacity; i++) { \
            HTNode_##table_name* node = table->buckets[i]; \
            while (node) { \
                HTNode_##table_name* next = node->next; \
                size_t dest = node->hash % new_cap; \
                node->next = new_buckets[dest]; \
                new_buckets[dest] = node; \
                node = next; \
            } \
        } \
        free(table->buckets); \
        table->buckets = new_buckets; \
        table->capacity = new_cap; \
        return HT_OK; \
    } \
\
    int table_name##_INIT_EX( \
        HT_##table_name* table, \
        size_t capacity, \
        hash_func_##table_name h_fn, \
        eq_func_##table_name e_fn \
    ) { \
        table->count = 0; \
        table->capacity = capacity; \
        table->buckets = (HTNode_##table_name**)calloc(capacity, sizeof(HTNode_##table_name*)); \
        if (!table->buckets) return HT_ERROR; \
        table->hash_func = h_fn ? h_fn : _default_hash_##table_name; \
        table->eq_func = e_fn ? e_fn : _default_eq_##table_name; \
        return HT_OK; \
    } \
\
    int table_name##_INIT(HT_##table_name* table) { \
        return table_name##_INIT_EX(table, HT_INIT_CAPACITY, NULL, NULL); \
    } \
\
    int table_name##_WITH_CAPACITY(HT_##table_name* table, size_t capacity) { \
        return table_name##_INIT_EX(table, capacity, NULL, NULL); \
    } \
\
    int table_name##_INSERT(HT_##table_name* table, k_type key, v_type value) { \
        if (table->count >= table->capacity * HT_LOAD_FACTOR) { \
            if (_table_name##_RESIZE(table, table->capacity * 2) == HT_ERROR) \
                return HT_ERROR; \
        } \
        uint32_t hash = table->hash_func(key); \
        size_t idx = hash % table->capacity; \
        HTNode_##table_name* node = table->buckets[idx]; \
        while (node) { \
            if (node->hash == hash && HT_AUTO_EQ(node->key, key)) { \
                node->value = value; \
                return HT_OK; \
            } \
            node = node->next; \
        } \
        HTNode_##table_name* new_node = (HTNode_##table_name*)malloc(sizeof(HTNode_##table_name)); \
        if (!new_node) return HT_ERROR; \
        new_node->key = key; \
        new_node->value = value; \
        new_node->hash = hash; \
        new_node->next = table->buckets[idx]; \
        table->buckets[idx] = new_node; \
        table->count++; \
        return HT_OK; \
    } \
\
    int table_name##_REMOVE(HT_##table_name* table, k_type key) { \
        if (table->count == 0) return HT_ERROR; \
        uint32_t hash = table->hash_func(key); \
        size_t idx = hash % table->capacity; \
        HTNode_##table_name* node = table->buckets[idx]; \
        HTNode_##table_name* prev = NULL; \
        while (node) { \
            if (node->hash == hash && HT_AUTO_EQ(node->key, key)) { \
                if (prev) prev->next = node->next; \
                else table->buckets[idx] = node->next; \
                free(node); \
                table->count--; \
                return HT_OK; \
            } \
            prev = node; \
            node = node->next; \
        } \
        return HT_ERROR; \
    } \
\
    int table_name##_CONTAINS(const HT_##table_name* table, k_type key) { \
        if (table->count == 0) return 0; \
        uint32_t hash = table->hash_func(key); \
        size_t idx = hash % table->capacity; \
        HTNode_##table_name* node = table->buckets[idx]; \
        while (node) { \
            if (node->hash == hash && HT_AUTO_EQ(node->key, key)) return 1; \
            node = node->next; \
        } \
        return 0; \
    } \
\
    int table_name##_GET(const HT_##table_name* table, k_type key, v_type* value) { \
        if (table->count == 0) return HT_ERROR; \
        uint32_t hash = table->hash_func(key); \
        size_t idx = hash % table->capacity; \
        HTNode_##table_name* node = table->buckets[idx]; \
        while (node) { \
            if (node->hash == hash && HT_AUTO_EQ(node->key, key)) { \
                if (value) *value = node->value; \
                return HT_OK; \
            } \
            node = node->next; \
        } \
        return HT_ERROR; \
    } \
\
    int table_name##_CLEAR(HT_##table_name* table) { \
        if (!table || !table->buckets) return HT_OK; \
        for (size_t i = 0; i < table->capacity; i++) { \
            HTNode_##table_name* curr = table->buckets[i]; \
            while (curr) { \
                HTNode_##table_name* next_node = curr->next; \
                free(curr); \
                curr = next_node; \
            } \
            table->buckets[i] = NULL; \
        } \
        table->count = 0; \
        return HT_OK; \
    } \
\
    int table_name##_DESTROY(HT_##table_name* table) { \
        if (!table) return HT_OK; \
        table_name##_CLEAR(table); \
        if (table->buckets) { \
            free(table->buckets); \
            table->buckets = NULL; \
        } \
        table->capacity = 0; \
        table->count = 0; \
        return HT_OK; \
    } \
\
    int table_name##_COPY(const HT_##table_name* src, HT_##table_name* dest) { \
        for (size_t i = 0; i < src->capacity; i++) { \
            HTNode_##table_name* node = src->buckets[i]; \
            while (node) { \
                if (table_name##_INSERT(dest, node->key, node->value) != HT_OK) \
                    return HT_ERROR; \
                node = node->next; \
            } \
        } \
        return HT_OK; \
    } \
\
    int table_name##_IS_EMPTY(const HT_##table_name* table) { \
        return table->count == 0 ? 1 : 0; \
    } \
\
    size_t table_name##_SIZE(const HT_##table_name* table) { \
        return table->count; \
    } \
\
    k_type* table_name##_KEYS(const HT_##table_name* table) { \
        k_type* keys = (k_type*)malloc(sizeof(k_type) * table->count); \
        if (!keys) return NULL; \
        size_t key_count = 0; \
        for (size_t i = 0; i < table->capacity; i++) { \
            HTNode_##table_name* node = table->buckets[i]; \
            while (node) { \
                keys[key_count++] = node->key; \
                node = node->next; \
            } \
        } \
        return keys; \
    } \
\
    const k_type* table_name##_KEYS_CONST(const HT_##table_name* table) { \
        return (const k_type*)table_name##_KEYS(table); \
    } \
\
    v_type* table_name##_VALUES(const HT_##table_name* table) { \
        v_type* values = (v_type*)malloc(sizeof(v_type) * table->count); \
        if (!values) return NULL; \
        size_t value_count = 0; \
        for (size_t i = 0; i < table->capacity; i++) { \
            HTNode_##table_name* node = table->buckets[i]; \
            while (node) { \
                values[value_count++] = node->value; \
                node = node->next; \
            } \
        } \
        return values; \
    } \
\
    const v_type* table_name##_VALUES_CONST(const HT_##table_name* table) { \
        return (const v_type*)table_name##_VALUES(table); \
    } \

#endif


