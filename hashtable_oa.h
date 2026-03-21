/*
 * Copyright (c) 2026 Arcelyth
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 * 
 */


#ifndef HASH_TABLE_SC_H
#define HASH_TABLE_SC_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define HT_ERROR -1
#define HT_OK 0
#define HT_INIT_CAPACITY 8
#define HT_LOAD_FACTOR 0.75

#define HT_STATE_EMPTY 0
#define HT_STATE_OCCUPIED 1
#define HT_STATE_DELETED 2

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
    char*:              _ht_djb2_internal((const void*)(uintptr_t)(key), strlen((const char*)(uintptr_t)(key))), \
    const char*:        _ht_djb2_internal((const void*)(uintptr_t)(key), strlen((const char*)(uintptr_t)(key))), \
    default:            _ht_djb2_internal(&(key), sizeof(key)) \
)

#define HT_AUTO_EQ(a, b) _Generic((a), \
    char*:              (strcmp((const char*)(uintptr_t)(a), (const char*)(uintptr_t)(b)) == 0), \
    const char*:        (strcmp((const char*)(uintptr_t)(a), (const char*)(uintptr_t)(b)) == 0), \
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
        uint8_t state; \
    } HTNode_##table_name; \
\
    typedef HTNode_##table_name table_name##_ENTRY; \
\
    typedef struct { \
        size_t count; \
        size_t capacity; \
        size_t used_slots; /* count + deleted slots */ \
        float load_factor; \
        hash_func_##table_name hash_func; \
        eq_func_##table_name eq_func; \
        table_name##_ENTRY* entries; \
    } HT_##table_name; \
\
    typedef struct { \
        HT_##table_name* hashtable; \
        size_t index; \
    } HT_##table_name##_ITER; \
\
    typedef struct { \
        size_t init_capacity; \
        float load_factor; \
        hash_func_##table_name hash_fn; \
        eq_func_##table_name eq_fn; \
    } HTConfig_##table_name; \
\
    static inline uint32_t _default_hash_##table_name(k_type key) { \
        return HT_AUTO_HASH(key); \
    } \
\
    static inline bool _default_eq_##table_name(k_type a, k_type b) { \
        return HT_AUTO_EQ(a, b); \
    } \
\
    static int _##table_name##_RESIZE(HT_##table_name* table, size_t new_cap) {  \
        table_name##_ENTRY* new_entries = (table_name##_ENTRY*)calloc(new_cap, sizeof(table_name##_ENTRY)); \
        if (!new_entries) return HT_ERROR; \
        for (size_t i = 0; i < table->capacity; i++) { \
            if (table->entries[i].state == HT_STATE_OCCUPIED) { \
                size_t dest = table->entries[i].hash % new_cap; \
                while (new_entries[dest].state == HT_STATE_OCCUPIED) { \
                    dest = (dest + 1) % new_cap; \
                } \
                new_entries[dest] = table->entries[i]; \
            } \
        } \
        free(table->entries); \
        table->entries = new_entries; \
        table->capacity = new_cap; \
        table->used_slots = table->count; \
        return HT_OK; \
    } \
\
    int table_name##_WITH_CONFIG(HT_##table_name* table, HTConfig_##table_name* config) { \
        size_t cap = (config && config->init_capacity > 0) ? config->init_capacity : HT_INIT_CAPACITY; \
        table->count = 0; \
        table->used_slots = 0; \
        table->capacity = cap; \
        table->entries = (table_name##_ENTRY*)calloc(cap, sizeof(table_name##_ENTRY)); \
        if (!table->entries) return HT_ERROR; \
        table->hash_func = (config && config->hash_fn) ? config->hash_fn : _default_hash_##table_name; \
        table->eq_func = (config && config->eq_fn) ? config->eq_fn : _default_eq_##table_name; \
        table->load_factor = (config && config->load_factor) ? config->load_factor : HT_LOAD_FACTOR; \
        return HT_OK; \
    } \
\
    int table_name##_INIT(HT_##table_name* table) { \
        return table_name##_WITH_CONFIG(table, NULL); \
    } \
\
    int table_name##_WITH_CAPACITY(HT_##table_name* table, size_t capacity) { \
        HTConfig_##table_name config = {.init_capacity = capacity}; \
        return table_name##_WITH_CONFIG(table, &config); \
    } \
\
    int table_name##_INSERT(HT_##table_name* table, k_type key, v_type value) { \
        if (table->used_slots >= table->capacity * table->load_factor) { \
            if (_##table_name##_RESIZE(table, table->capacity * 2) == HT_ERROR) \
                return HT_ERROR; \
        } \
        uint32_t hash = table->hash_func(key); \
        size_t idx = hash % table->capacity; \
        size_t first_deleted = (size_t)-1; \
        while (1) { \
            if (table->entries[idx].state == HT_STATE_EMPTY) { \
                /* Prioritize using the first deleted when encountered first empty position on the detection path */ \
                size_t target = (first_deleted != (size_t)-1) ? first_deleted : idx; \
                table->entries[target].key = key; \
                table->entries[target].value = value; \
                table->entries[target].hash = hash; \
                table->entries[target].state = HT_STATE_OCCUPIED; \
                table->count++; \
                if (target == idx) table->used_slots++; \
                return HT_OK; \
            } else if (table->entries[idx].state == HT_STATE_OCCUPIED) { \
                if (table->entries[idx].hash == hash && table->eq_func(table->entries[idx].key, key)) { \
                    table->entries[idx].value = value; \
                    return HT_OK; \
                } \
            } else if (table->entries[idx].state == HT_STATE_DELETED) { \
                if (first_deleted == (size_t)-1) first_deleted = idx; \
            } \
            idx = (idx + 1) % table->capacity; \
        } \
    } \
\
    int table_name##_REMOVE(HT_##table_name* table, k_type key) { \
        if (table->count == 0) return HT_ERROR; \
        uint32_t hash = table->hash_func(key); \
        size_t idx = hash % table->capacity; \
        while (table->entries[idx].state != HT_STATE_EMPTY) { \
            if (table->entries[idx].state == HT_STATE_OCCUPIED && \
                table->entries[idx].hash == hash && table->eq_func(table->entries[idx].key, key) \
            ) { \
                table->entries[idx].state = HT_STATE_DELETED; \
                table->count--; \
                return HT_OK; \
            } \
            idx = (idx + 1) % table->capacity; \
        } \
        return HT_ERROR; \
    } \
\
    int table_name##_CONTAINS(const HT_##table_name* table, k_type key) { \
        if (table->count == 0) return 0; \
        uint32_t hash = table->hash_func(key); \
        size_t idx = hash % table->capacity; \
        \
        while (table->entries[idx].state != HT_STATE_EMPTY) { \
            if (table->entries[idx].state == HT_STATE_OCCUPIED && \
                table->entries[idx].hash == hash && table->eq_func(table->entries[idx].key, key) \
            ) { \
                return 1; \
            } \
            idx = (idx + 1) % table->capacity; \
        } \
        return 0; \
    } \
\
    int table_name##_GET(const HT_##table_name* table, k_type key, v_type* value) { \
        if (table->count == 0) return HT_ERROR; \
        uint32_t hash = table->hash_func(key); \
        size_t idx = hash % table->capacity; \
        \
        while (table->entries[idx].state != HT_STATE_EMPTY) { \
            if (table->entries[idx].state == HT_STATE_OCCUPIED && \
                table->entries[idx].hash == hash && table->eq_func(table->entries[idx].key, key) \
            ) { \
                if (value) *value = table->entries[idx].value; \
                return HT_OK; \
            } \
            idx = (idx + 1) % table->capacity; \
        } \
        return HT_ERROR; \
    } \
\
    int table_name##_CLEAR(HT_##table_name* table) { \
        if (!table || !table->entries) return HT_OK; \
        memset(table->entries, 0, table->capacity * sizeof(table_name##_ENTRY)); \
        table->count = 0; \
        table->used_slots = 0; \
        return HT_OK; \
    } \
\
    int table_name##_DESTROY(HT_##table_name* table) { \
        if (!table) return HT_OK; \
        if (table->entries) { \
            free(table->entries); \
            table->entries = NULL; \
        } \
        table->capacity = 0; \
        table->count = 0; \
        table->used_slots = 0; \
        return HT_OK; \
    } \
\
    int table_name##_COPY(const HT_##table_name* src, HT_##table_name* dest) { \
        for (size_t i = 0; i < src->capacity; i++) { \
            if (src->entries[i].state == HT_STATE_OCCUPIED) { \
                if (table_name##_INSERT(dest, src->entries[i].key, src->entries[i].value) != HT_OK) \
                    return HT_ERROR; \
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
            if (table->entries[i].state == HT_STATE_OCCUPIED) { \
                keys[key_count++] = table->entries[i].key; \
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
            if (table->entries[i].state == HT_STATE_OCCUPIED) { \
                values[value_count++] = table->entries[i].value; \
            } \
        } \
        return values; \
    } \
\
    const v_type* table_name##_VALUES_CONST(const HT_##table_name* table) { \
        return (const v_type*)table_name##_VALUES(table); \
    } \
\
    table_name##_ENTRY* table_name##_INTO_ITER(HT_##table_name* table, HT_##table_name##_ITER* iter) { \
        iter->hashtable = table; \
        for (size_t i = 0; i < table->capacity; i++) { \
            if (table->entries[i].state == HT_STATE_OCCUPIED) { \
                iter->index = i; \
                return &table->entries[i]; \
            } \
        } \
        return NULL; \
    } \
\
    table_name##_ENTRY* table_name##_NEXT(HT_##table_name##_ITER* iter) { \
        for (size_t i = iter->index + 1; i < iter->hashtable->capacity; i++) { \
            if (iter->hashtable->entries[i].state == HT_STATE_OCCUPIED) { \
                iter->index = i; \
                return &iter->hashtable->entries[i]; \
            } \
        } \
        return NULL; \
    } \

#endif
