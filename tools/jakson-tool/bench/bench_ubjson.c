#include "bench_ubjson.h"
#include "bench_format_handler.h"

#include <libs/jansson/jansson.h>
#include <libs/jansson/load.c>
#include <libs/jansson/error.c>
#include <libs/jansson/strbuffer.h>
#include <libs/jansson/value.c>

/*
 * Copyright (c) 2016 Tomasz Sieprawski
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
*/

/* js2ubj_main.c */
typedef struct json_t json_t;
typedef struct ctx ctx;
struct ctx
{
    ubjs_bool verbose;
    ubjs_bool pretty_print_output;

    unsigned int verbose_before;
    unsigned int verbose_after;
};
/*
static void js2ubj_main_writer_context_would_write(void *userdata, uint8_t *data,
                                                   unsigned int len)
{
    ctx *my_ctx = (ctx *)userdata;

    if (UTRUE == my_ctx->verbose)
    {
        printf("After: [%u]\n", len);
    }

    fwrite((void *)data, sizeof(uint8_t), len, stdout);

    if (UTRUE == my_ctx->verbose)
    {
        my_ctx->verbose_after = len;
        printf("\nCompression/expansion: [%u percent]\n",
               100 * my_ctx->verbose_after / my_ctx->verbose_before);
    }
}
*/
/*
static void js2ubj_main_writer_context_would_print(void *userdata, char *data,
                                                   unsigned int len)
{
    UNUSED(userdata)
    char *tmp = (char *)malloc(sizeof(char) * (len + 1));

    strncpy(tmp, data, len);
    tmp[len] = 0;
    printf("Pretty-printed [%u]: %s\n", len, tmp);
    free(tmp);
}
*/
/*
static void js2ubj_main_writer_context_free(void *userdata)
{
    UNUSED(userdata)
}
 */
static void js2ubj_main_encode_json_to_ubjson(json_t *jsoned, ubjs_library *lib, ubjs_prmtv **pobj)
{
    size_t index;
    const char *key;
    json_t *jsoned_item;
    ubjs_prmtv *item;

    switch (json_typeof(jsoned))
    {
        case JSON_NULL:
            *pobj = ubjs_prmtv_null();
            break;

        case JSON_TRUE:
            *pobj = ubjs_prmtv_true();
            break;

        case JSON_FALSE:
            *pobj = ubjs_prmtv_false();
            break;

        case JSON_INTEGER:
            ubjs_prmtv_int(lib, json_integer_value(jsoned), pobj);;
            break;

        case JSON_REAL:
            ubjs_prmtv_float64(lib, json_real_value(jsoned), pobj);
            break;

        case JSON_STRING:
            ubjs_prmtv_str(lib, json_string_length(jsoned), (char *)json_string_value(jsoned),
                           pobj);
            break;

        case JSON_ARRAY:
            ubjs_prmtv_array(lib, pobj);
            json_array_foreach(jsoned, index, jsoned_item)
            {
                js2ubj_main_encode_json_to_ubjson(jsoned_item, lib, &item);
                ubjs_prmtv_array_add_last(*pobj, item);
            }
            break;

        case JSON_OBJECT:
            ubjs_prmtv_object(lib, pobj);
            json_object_foreach(jsoned, key, jsoned_item)
            {
                js2ubj_main_encode_json_to_ubjson(jsoned_item, lib, &item);
                ubjs_prmtv_object_set(*pobj, strlen(key), (char *)key, item);
            }
            break;

        default:
            break;
    }
}

/* Copyright End */

bool bench_ubjson_error_create(bench_ubjson_error *ubjsonError, bench_error *benchError)
{
    ERROR_IF_NULL(ubjsonError)
    ERROR_IF_NULL(benchError)

    struct err *err = malloc(sizeof(*err));
    error_init(err);

    ubjsonError->benchErr = benchError;
    ubjsonError->err = err;

    return true;
}

bool bench_ubjson_error_destroy(bench_ubjson_error *error) {
    ERROR_IF_NULL(error)

    CHECK_SUCCESS(error_drop(error->err));
    free(error);

    return true;
}

bool bench_ubjson_error_write(bench_ubjson_error *error, const char *msg, const char *file, u32 line, size_t docOffset)
{
    ERROR_IF_NULL(error)
    ERROR_IF_NULL(msg)

    error_set_wdetails(error->err, ERR_FAILED, __FILE__, __LINE__, msg);

    error->benchErr->msg = strdup(msg);
    error->benchErr->file = strdup(file);
    error->benchErr->line = line;
    error->benchErr->offset = docOffset;

    error_print_to_stderr(error->err);

    return true;
}

bool bench_ubjson_mgr_create_from_file(bench_ubjson_mgr *manager, bench_ubjson_error *ubjsonError, bench_error *benchError, const char *filePath)
{
    ERROR_IF_NULL(manager)
    ERROR_IF_NULL(ubjsonError)
    ERROR_IF_NULL(benchError)
    ERROR_IF_NULL(filePath)

    bench_ubjson_mgr_create_empty(manager, ubjsonError, benchError);

    ubjs_prmtv *obj = 0;
    ubjs_library *lib = 0;
    json_error_t jError;
    json_t *json = 0;
    FILE *file;
    file = fopen(filePath, "r");

    json = json_loadf(file, JSON_DECODE_ANY, &jError);
    if(json == 0) {
        BENCH_UBJSON_ERROR_WRITE(ubjsonError, "Failed to read JSON file", jError.line)
        return false;
    }
    fclose(file);

    js2ubj_main_encode_json_to_ubjson(json, lib, &obj);
    //obj = 0;

    manager->obj = obj;
    manager->lib = lib;

    return true;
}

bool bench_ubjson_mgr_create_empty(bench_ubjson_mgr *manager, bench_ubjson_error *ubjsonError, bench_error *benchError)
{
    ERROR_IF_NULL(manager);
    ERROR_IF_NULL(ubjsonError);
    ERROR_IF_NULL(benchError);

    bench_ubjson_error_create(ubjsonError, benchError);

    ubjs_library_builder builder;
    ubjs_library *lib = 0;
    ubjs_library_builder_init(&builder);
    ubjs_library_builder_build(&builder, &lib);
    ubjs_prmtv *obj = 0;
    //unsigned int len = -1;
    ubjs_prmtv_object(lib, &obj);

    manager->obj = obj;
    manager->lib = lib;
    manager->error = ubjsonError;

    return true;
}

bool bench_ubjson_append_doc(bench_ubjson_mgr *manager, const char *filePath)
{
    ERROR_IF_NULL(manager);
    ERROR_IF_NULL(filePath);

    ubjs_prmtv *obj = 0;
    json_error_t jError;
    json_t *json = 0;
    FILE *file;
    file = fopen(filePath, "r");
    static int doc_count = 0;
    char doc_key[32];

    json = json_loadf(file, JSON_DECODE_ANY, &jError);
    if(json == 0) {
        BENCH_UBJSON_ERROR_WRITE(manager->error, "Failed to read JSON file", jError.line)
        return false;
    }
    fclose(file);
    sprintf(doc_key,"%d", doc_count);
    js2ubj_main_encode_json_to_ubjson(json, manager->lib, &obj);
    ubjs_prmtv_object_set(manager->obj, strlen(doc_key), doc_key, obj);
    free(json);

    doc_count++;

    return true;
}

bool bench_ubjson_convert_doc(size_t *conv_size, bench_ubjson_mgr *manager, const char *filePath)
{
    ERROR_IF_NULL(conv_size);
    ERROR_IF_NULL(manager);
    ERROR_IF_NULL(filePath);

    ubjs_prmtv *obj = 0;
    json_error_t jError;
    json_t *json = 0;
    FILE *file;
    file = fopen(filePath, "r");
    ubjs_library_builder library_builder;
    ubjs_library *lib = 0;

    ubjs_library_builder_init(&library_builder);
    ubjs_library_builder_build(&library_builder, &lib);

    json = json_loadf(file, JSON_DECODE_ANY, &jError);
    if(json == 0) {
        BENCH_UBJSON_ERROR_WRITE(manager->error, "Failed to read JSON file", jError.line)
        return false;
    }
    fclose(file);

    js2ubj_main_encode_json_to_ubjson(json, lib, &obj);

    *conv_size = bench_format_handler_get_process_size();
    pthread_attr_t atr;
    pthread_attr_getstacksize(&atr, conv_size);

    ubjs_prmtv_free(&obj);
    ubjs_library_free(&lib);

    return true;
}

bool bench_ubjson_mgr_destroy(bench_ubjson_mgr *manager)
{
    ERROR_IF_NULL(manager)

    ubjs_prmtv_free(&manager->obj);
    ubjs_library_free(&manager->lib);
    bench_ubjson_error_destroy(manager->error);
    return true;
}

bool bench_ubjson_get_doc(char *str, bench_ubjson_mgr *manager)
{
    UNUSED(str)
    ERROR_IF_NULL(manager)

    ubjs_object_iterator *it;
    ubjs_prmtv_object_iterate(manager->obj, &it);

    while(UR_OK == ubjs_object_iterator_next(it)) {
        unsigned int key_length;
        unsigned int value_length;
        char *key;
        char *value;
        int32_t val;
        ubjs_prmtv *item;

        ubjs_object_iterator_get_key_length(it, &key_length);
        key = (char*) malloc(sizeof(char) * key_length);
        ubjs_object_iterator_copy_key(it, key);
        ubjs_object_iterator_get_value(it, &item);
        ubjs_prmtv_str_get_length(item, &value_length);
        value = (char*)malloc(sizeof(char) * value_length);
        // TODO : Implement for all value types
        ubjs_prmtv_int32_get(item, &val);
        sprintf(value, "%d", val);

        strcat(str, key);
        strcat(str, ":");
        strcat(str, value);
        strcat(str, "\n");

        free(key);
        free(value);
    }

    ubjs_object_iterator_free(&it);

    return true;
}

size_t bench_ubjson_iterate(ubjs_prmtv *obj)
{
    ERROR_IF_NULL(obj);

    //ubjs_prmtv_int3

    return false;
}

size_t bench_ubjson_get_doc_size(bench_ubjson_mgr *manager)
{
    ERROR_IF_NULL(manager);
    uint32_t doc_len;
    UNUSED(doc_len);
    ubjs_object_iterator *it;
    ubjs_prmtv_object_iterate(manager->obj, &it);

    while(UR_OK == ubjs_object_iterator_next(it)) {
        unsigned int key_length;
        char *key;
        ubjs_prmtv *item;
        ubjs_object_iterator_get_key_length(it, &key_length);
        key = (char *)malloc(sizeof(char) * key_length);
        /* Note that this string is NOT null terminated. */
        ubjs_object_iterator_copy_key(it, key);
        ubjs_object_iterator_get_value(it, &item);
    }

    //ubjs_prmtv_object_get_length(manager->obj, &doc_len);


    return false;
}

uint32_t bench_ubjson_get_reads(bench_ubjson_mgr *manager)
{
    ERROR_IF_NULL(manager);

    return manager->reads;
}
uint32_t bench_ubjson_get_updates(bench_ubjson_mgr *manager)
{
    ERROR_IF_NULL(manager);

    return manager->updates;
}
/*
bool bench_ubjson_insert_int32(bench_ubjson_mgr *manager, char *key, int32_t val)
{
    ERROR_IF_NULL(manager)
    ERROR_IF_NULL(key)
    ERROR_IF_NULL(val)

    ubjs_prmtv *item = 0;
    ubjs_prmtv_int32(manager->lib, val, &item);

    if(UR_OK == ubjs_prmtv_object_set(manager->obj, sizeof(key), key, item))
        return true;
    free(item);
    return false;
}

bool bench_ubjson_find_int32(bench_ubjson_mgr *manager, ubjs_array_iterator *it, char *key, int32_t val)
{
    ERROR_IF_NULL(manager)
    ERROR_IF_NULL(key)
    UNUSED(val)
    UNUSED(it)

    ubjs_prmtv *item = 0;
    if(UR_OK == ubjs_prmtv_object_get(manager->obj, sizeof(key), key, &item))
        return true;
    free(item);
    return false;
}
*/

bool bench_ubjson_insert_int8(bench_ubjson_mgr *manager, uint32_t numOperations, container_type type)
{
    ubjs_prmtv *item;
    char msg[ERROR_MSG_SIZE];

    if(type == BENCH_CONTAINER_TYPE_ARRAY) {
        ubjs_prmtv *arrayObj;
        ubjs_prmtv_array_with_length(manager->lib, numOperations, &arrayObj);
        //ubjs_prmtv_array(manager->lib, &arrayObj);
        ubjs_prmtv_object_set(manager->obj, 1, "0", arrayObj);

        for (uint32_t i = 0; i < numOperations; i++) {
            int8_t val = random() % INT8_MAX;

            ubjs_prmtv_int8(manager->lib, val, &item);
            if(UR_OK != ubjs_prmtv_array_add_last(arrayObj, item)) {
                sprintf(msg, "Failed to insert int8 value '%d' into array at position: %d", val, i);
                BENCH_UBJSON_ERROR_WRITE(manager->error, msg, i);
                return false;
            }
            manager->numEntries++;
        }
    } else if (type == BENCH_CONTAINER_TYPE_OBJECT) {
        ubjs_prmtv *nestedObj;
        ubjs_prmtv_object_with_length(manager->lib, numOperations, &nestedObj);
        //ubjs_prmtv_object(manager->lib, &nestedObj);
        ubjs_prmtv_object_set(manager->obj, 1, "0", nestedObj);

        for (uint32_t i = 0; i < numOperations; i++) {
            int8_t val = random() % INT8_MAX;
            char key[UINT32_MAX_DIGITS + 1];
            sprintf(key, "%d", i);

            ubjs_prmtv_int8(manager->lib, val, &item);
            if(UR_OK != ubjs_prmtv_object_set(nestedObj, strlen(key), key, item)) {
                sprintf(msg, "Failed to insert int8 value '%d' into object at position: %d", val, i);
                BENCH_UBJSON_ERROR_WRITE(manager->error, msg, i);
                return false;
            }
            manager->numEntries++;
        }
    }

    return true;
}

bool bench_ubjson_insert_int16(bench_ubjson_mgr *manager, uint32_t numOperations, container_type type)
{
    ubjs_prmtv *item;
    char msg[ERROR_MSG_SIZE];

    if(type == BENCH_CONTAINER_TYPE_ARRAY) {
        ubjs_prmtv *arrayObj;
        ubjs_prmtv_array_with_length(manager->lib, numOperations, &arrayObj);
        //ubjs_prmtv_array(manager->lib, &arrayObj);
        ubjs_prmtv_object_set(manager->obj, 1, "0", arrayObj);

        for (uint32_t i = 0; i < numOperations; i++) {
            int16_t val = random() % INT16_MAX;

            ubjs_prmtv_int16(manager->lib, val, &item);
            if(UR_OK != ubjs_prmtv_array_add_last(arrayObj, item)) {
                sprintf(msg, "Failed to insert int16 value '%d' into array at position: %d", val, i);
                BENCH_UBJSON_ERROR_WRITE(manager->error, msg, i);
                return false;
            }
            manager->numEntries++;
        }
    } else if (type == BENCH_CONTAINER_TYPE_OBJECT) {
        ubjs_prmtv *nestedObj;
        ubjs_prmtv_object_with_length(manager->lib, numOperations, &nestedObj);
        //ubjs_prmtv_object(manager->lib, &nestedObj);
        ubjs_prmtv_object_set(manager->obj, 1, "0", nestedObj);

        for (uint32_t i = 0; i < numOperations; i++) {
            int16_t val = random() % INT16_MAX;
            char key[UINT32_MAX_DIGITS + 1];
            sprintf(key, "%d", i);

            ubjs_prmtv_int16(manager->lib, val, &item);
            if(UR_OK != ubjs_prmtv_object_set(nestedObj, strlen(key), key, item)) {
                sprintf(msg, "Failed to insert int16 value '%d' into object at position: %d", val, i);
                BENCH_UBJSON_ERROR_WRITE(manager->error, msg, i);
                return false;
            }
            manager->numEntries++;
        }
    }

    return true;
}

bool bench_ubjson_insert_int32(bench_ubjson_mgr *manager, uint32_t numOperations, container_type type)
{
    ubjs_prmtv *item;
    char msg[ERROR_MSG_SIZE];
    if(type == BENCH_CONTAINER_TYPE_ARRAY) {
        ubjs_prmtv *arrayObj;
        ubjs_prmtv_array_with_length(manager->lib, numOperations, &arrayObj);
        //ubjs_prmtv_array(manager->lib, &arrayObj);
        ubjs_prmtv_object_set(manager->obj, 1, "0", arrayObj);

        for (uint32_t i = 0; i < numOperations; i++) {
            int32_t val = random() % INT32_MAX;

            ubjs_prmtv_int32(manager->lib, val, &item);
            if(UR_OK != ubjs_prmtv_array_add_last(arrayObj, item)) {
                sprintf(msg, "Failed to insert int32 value '%d' into array at position: %d", val, i);
                BENCH_UBJSON_ERROR_WRITE(manager->error, msg, i);
                return false;
            }
            manager->numEntries++;
        }
    } else if (type == BENCH_CONTAINER_TYPE_OBJECT) {
        ubjs_prmtv *nestedObj;
        ubjs_prmtv_object_with_length(manager->lib, numOperations, &nestedObj);
        //ubjs_prmtv_object(manager->lib, &nestedObj);
        ubjs_prmtv_object_set(manager->obj, 1, "0", nestedObj);

        for (uint32_t i = 0; i < numOperations; i++) {
            int32_t val = random() % INT32_MAX;
            char key[UINT32_MAX_DIGITS + 1];
            sprintf(key, "%d", i);

            ubjs_prmtv_int32(manager->lib, val, &item);
            if(UR_OK != ubjs_prmtv_object_set(nestedObj, strlen(key), key, item)) {
                sprintf(msg, "Failed to insert int32 value '%d' into object at position: %d", val, i);
                BENCH_UBJSON_ERROR_WRITE(manager->error, msg, i);
                return false;
            }
            manager->numEntries++;
        }
    }

    return true;
}

bool bench_ubjson_insert_int64(bench_ubjson_mgr *manager, uint32_t numOperations, container_type type)
{
    ubjs_prmtv *item;
    char msg[ERROR_MSG_SIZE];

    if(type == BENCH_CONTAINER_TYPE_ARRAY) {
        ubjs_prmtv *arrayObj;
        ubjs_prmtv_array_with_length(manager->lib, numOperations, &arrayObj);
        //ubjs_prmtv_array(manager->lib, &arrayObj);
        ubjs_prmtv_object_set(manager->obj, 1, "0", arrayObj);

        for (uint32_t i = 0; i < numOperations; i++) {
            int64_t val = random() % INT64_MAX;

            ubjs_prmtv_int64(manager->lib, val, &item);
            if(UR_OK != ubjs_prmtv_array_add_last(arrayObj, item)) {
                sprintf(msg, "Failed to insert int64 value '%ld' into array at position: %d", val, i);
                BENCH_UBJSON_ERROR_WRITE(manager->error, msg, i);
                return false;
            }
            manager->numEntries++;
        }
    } else if (type == BENCH_CONTAINER_TYPE_OBJECT) {
        ubjs_prmtv *nestedObj;
        ubjs_prmtv_object_with_length(manager->lib, numOperations, &nestedObj);
        //ubjs_prmtv_object(manager->lib, &nestedObj);
        ubjs_prmtv_object_set(manager->obj, 1, "0", nestedObj);

        for (uint32_t i = 0; i < numOperations; i++) {
            int64_t val = random() % INT64_MAX;
            char key[UINT32_MAX_DIGITS + 1];
            sprintf(key, "%d", i);

            ubjs_prmtv_int64(manager->lib, val, &item);
            if(UR_OK != ubjs_prmtv_object_set(nestedObj, strlen(key), key, item)) {
                sprintf(msg, "Failed to insert int64 value '%ld' into object at position: %d", val, i);
                BENCH_UBJSON_ERROR_WRITE(manager->error, msg, i);
                return false;
            }
            manager->numEntries++;
        }
    }

    return true;
}

bool bench_ubjson_insert_string(bench_ubjson_mgr *manager, uint32_t numOperations, container_type type)
{
    ubjs_prmtv *item;
    char msg[ERROR_MSG_SIZE];

    if(type == BENCH_CONTAINER_TYPE_ARRAY) {
        ubjs_prmtv *arrayObj;
        ubjs_prmtv_array_with_length(manager->lib, numOperations, &arrayObj);
        //ubjs_prmtv_array(manager->lib, &arrayObj);
        ubjs_prmtv_object_set(manager->obj, 1, "0", arrayObj);

        for (uint32_t i = 0; i < numOperations; i++) {
            int64_t _val = random() % INT64_MAX;
            char val[(int) (floor(log10(labs(_val))) + 1)];
            sprintf(val, "%ld", _val);
            ubjs_prmtv_str(manager->lib, strlen(val), val, &item);
            if(UR_OK != ubjs_prmtv_array_add_last(arrayObj, item)) {
                sprintf(msg, "Failed to insert int64 value '%s' into array at position: %d", val, i);
                BENCH_UBJSON_ERROR_WRITE(manager->error, msg, i);
                return false;
            }
            manager->numEntries++;
        }
    } /*else if (type == BENCH_CONTAINER_TYPE_OBJECT) {
        ubjs_prmtv *nestedObj;
        ubjs_prmtv_object_with_length(manager->lib, numOperations, &nestedObj);
        //ubjs_prmtv_object(manager->lib, &nestedObj);
        ubjs_prmtv_object_set(manager->obj, 1, "0", nestedObj);

        for (uint32_t i = 0; i < numOperations; i++) {
            int64_t val = random() % INT64_MAX;
            char key[UINT32_MAX_DIGITS + 1];
            sprintf(key, "%d", i);

            ubjs_prmtv_int64(manager->lib, val, &item);
            if(UR_OK != ubjs_prmtv_object_set(nestedObj, strlen(key), key, item)) {
                sprintf(msg, "Failed to insert int64 value '%ld' into object at position: %d", val, i);
                BENCH_UBJSON_ERROR_WRITE(manager->error, msg, i);
                return false;
            }
            manager->numEntries++;
        }
    }*/

    return true;
}

bool bench_ubjson_read(bench_ubjson_mgr *manager, uint32_t numOperations, container_type type)
{
    ubjs_prmtv *item = 0;
    char msg[ERROR_MSG_SIZE];
    clock_t startPoint = clock();
    clock_t timePassed = clock();

    if(type == BENCH_CONTAINER_TYPE_ARRAY) {
        for (uint32_t i = 0; i < numOperations || timePassed - startPoint < BENCH_OPERATION_MIN_TIME; i++) {
            ubjs_prmtv *arrayObj;
            ubjs_prmtv_object_get(manager->obj, 1, "0", &arrayObj);
            int32_t val;

            if (UR_OK != ubjs_prmtv_array_get_at(arrayObj, random() % manager->numEntries, &item)) {
                sprintf(msg, "Failed to read value in array at position: %d", i);
                BENCH_UBJSON_ERROR_WRITE(manager->error, msg, i);
                return false;
            }
            ubjs_prmtv_int32_get(item, &val);
            manager->reads++;
            timePassed = clock();
        }
    } else if (type == BENCH_CONTAINER_TYPE_OBJECT) {


        for (uint32_t i = 0; i < numOperations || timePassed - startPoint < BENCH_OPERATION_MIN_TIME; i++) {
            char key[UINT32_MAX_DIGITS + 1];
            ubjs_prmtv *nestedObj;
            ubjs_prmtv_object_get(manager->obj, 1, "0", &nestedObj);

            sprintf(key, "%d", (uint32_t) (random() % manager->numEntries));
            if(UR_OK != ubjs_prmtv_object_get(nestedObj, strlen(key), key, &item)) {
                sprintf(msg, "Failed to read value in object at position: %d", i);
                BENCH_UBJSON_ERROR_WRITE(manager->error, msg, i);
                return false;
            }
            manager->reads++;
            timePassed = clock();
        }
    }
    return true;
}

bool bench_ubjson_update_int8(bench_ubjson_mgr *manager, uint32_t numOperations, container_type type)
{
    ubjs_prmtv *item = 0;
    char msg[ERROR_MSG_SIZE];
    clock_t startPoint = clock();
    clock_t timePassed = clock();

    if(type == BENCH_CONTAINER_TYPE_ARRAY) {

        for (uint32_t i = 0; i < numOperations || timePassed - startPoint < BENCH_OPERATION_MIN_TIME; i++) {
            ubjs_prmtv *arrayObj;
            ubjs_prmtv_object_get(manager->obj, 1, "0", &arrayObj);
            int8_t *val = malloc(sizeof(*val));

            if (UR_OK != ubjs_prmtv_array_get_at(arrayObj, random() % manager->numEntries, &item)) {
                sprintf(msg, "Failed to read int8 value for update operation in array at position: %d", i);
                BENCH_UBJSON_ERROR_WRITE(manager->error, msg, i);
                return false;
            }
            ubjs_prmtv_int8_get(item, val);
            *val = random() % INT8_MAX;
            timePassed = clock();
            manager->updates++;
        }
    } else if (type == BENCH_CONTAINER_TYPE_OBJECT) {

        for (uint32_t i = 0; i < numOperations || timePassed - startPoint < BENCH_OPERATION_MIN_TIME; i++) {
            ubjs_prmtv *nestedObj;
            ubjs_prmtv_object_get(manager->obj, 1, "0", &nestedObj);
            int8_t *val = malloc(sizeof(*val));
            char key[UINT32_MAX_DIGITS + 3];
            sprintf(key, "%d", (uint32_t) (random() % manager->numEntries));

            if(UR_OK != ubjs_prmtv_object_get(nestedObj, strlen(key), key, &item)) {
                sprintf(msg, "Failed to read int8 value for update operation in object at position: %d", i);
                BENCH_UBJSON_ERROR_WRITE(manager->error, msg, i);
                return false;
            }
            ubjs_prmtv_int8_get(item, val);
            *val = random() % INT8_MAX;
            timePassed = clock();
            manager->updates++;
        }
    }
    return true;
}

bool bench_ubjson_update_int16(bench_ubjson_mgr *manager, uint32_t numOperations, container_type type)
{
    ubjs_prmtv *item = 0;
    char msg[ERROR_MSG_SIZE];
    clock_t startPoint = clock();
    clock_t timePassed = clock();

    if(type == BENCH_CONTAINER_TYPE_ARRAY) {

        for (uint32_t i = 0; i < numOperations || timePassed - startPoint < BENCH_OPERATION_MIN_TIME; i++) {
            ubjs_prmtv *arrayObj;
            ubjs_prmtv_object_get(manager->obj, 1, "0", &arrayObj);
            int16_t *val = malloc(sizeof(*val));

            if (UR_OK != ubjs_prmtv_array_get_at(arrayObj, random() % manager->numEntries, &item)) {
                sprintf(msg, "Failed to read int16 value for update operation in array at position: %d", i);
                BENCH_UBJSON_ERROR_WRITE(manager->error, msg, i);
                return false;
            }
            ubjs_prmtv_int16_get(item, val);
            *val = random() % INT16_MAX;
            timePassed = clock();
            manager->updates++;
        }
    } else if (type == BENCH_CONTAINER_TYPE_OBJECT) {

        for (uint32_t i = 0; i < numOperations || timePassed - startPoint < BENCH_OPERATION_MIN_TIME; i++) {
            ubjs_prmtv *nestedObj;
            ubjs_prmtv_object_get(manager->obj, 1, "0", &nestedObj);
            int16_t *val = malloc(sizeof(*val));
            char key[UINT32_MAX_DIGITS + 3];
            sprintf(key, "%d", (uint32_t) (random() % manager->numEntries));

            if(UR_OK != ubjs_prmtv_object_get(nestedObj, strlen(key), key, &item)) {
                sprintf(msg, "Failed to read int16 value for update operation in object at position: %d", i);
                BENCH_UBJSON_ERROR_WRITE(manager->error, msg, i);
                return false;
            }
            ubjs_prmtv_int16_get(item, val);
            *val = random() % INT16_MAX;
            timePassed = clock();
            manager->updates++;
        }
    }
    return true;
}

bool bench_ubjson_update_int32(bench_ubjson_mgr *manager, uint32_t numOperations, container_type type)
{
    ubjs_prmtv *item = 0;
    char msg[ERROR_MSG_SIZE];
    clock_t startPoint = clock();
    clock_t timePassed = clock();

    if(type == BENCH_CONTAINER_TYPE_ARRAY) {

        for (uint32_t i = 0; i < numOperations || timePassed - startPoint < BENCH_OPERATION_MIN_TIME; i++) {
            ubjs_prmtv *arrayObj;
            ubjs_prmtv_object_get(manager->obj, 1, "0", &arrayObj);
            int32_t *val = malloc(sizeof(*val));
            if (UR_OK != ubjs_prmtv_array_get_at(arrayObj, random() % manager->numEntries, &item)) {
                sprintf(msg, "Failed to read int32 value for update operation in array at position: %d", i);
                BENCH_UBJSON_ERROR_WRITE(manager->error, msg, i);
                return false;
            }
            ubjs_prmtv_int32_get(item, val);
            *val = random() % INT32_MAX;
            timePassed = clock();
            manager->updates++;
        }
    } else if (type == BENCH_CONTAINER_TYPE_OBJECT) {

        for (uint32_t i = 0; i < numOperations || timePassed - startPoint < BENCH_OPERATION_MIN_TIME; i++) {
            ubjs_prmtv *nestedObj;
            ubjs_prmtv_object_get(manager->obj, 1, "0", &nestedObj);
            int32_t *val = malloc(sizeof(*val));
            char key[UINT32_MAX_DIGITS + 3];
            sprintf(key, "%d", (uint32_t) (random() % manager->numEntries));

            if(UR_OK != ubjs_prmtv_object_get(nestedObj, strlen(key), key, &item)) {
                sprintf(msg, "Failed to read int32 value for update operation in object at position: %d", i);
                BENCH_UBJSON_ERROR_WRITE(manager->error, msg, i);
                return false;
            }
            ubjs_prmtv_int32_get(item, val);
            *val = random() % INT32_MAX;
            timePassed = clock();
            manager->updates++;
        }
    }
    return true;
}

bool bench_ubjson_update_int64(bench_ubjson_mgr *manager, uint32_t numOperations, container_type type)
{
    ubjs_prmtv *item = 0;
    char msg[ERROR_MSG_SIZE];
    clock_t startPoint = clock();
    clock_t timePassed = clock();

    if(type == BENCH_CONTAINER_TYPE_ARRAY) {

        for (uint32_t i = 0; i < numOperations || timePassed - startPoint < BENCH_OPERATION_MIN_TIME; i++) {
            ubjs_prmtv *arrayObj;
            ubjs_prmtv_object_get(manager->obj, 1, "0", &arrayObj);
            int64_t *val = malloc(sizeof(*val));

            if (UR_OK != ubjs_prmtv_array_get_at(arrayObj, random() % manager->numEntries, &item)) {
                sprintf(msg, "Failed to read int64 value for update operation in array at position: %d", i);
                BENCH_UBJSON_ERROR_WRITE(manager->error, msg, i);
                return false;
            }
            ubjs_prmtv_int64_get(item, val);
            *val = random() % INT64_MAX;
            timePassed = clock();
            manager->updates++;
        }
    } else if (type == BENCH_CONTAINER_TYPE_OBJECT) {

        for (uint32_t i = 0; i < numOperations || timePassed - startPoint < BENCH_OPERATION_MIN_TIME; i++) {
            ubjs_prmtv *nestedObj;
            ubjs_prmtv_object_get(manager->obj, 1, "0", &nestedObj);
            int64_t *val = malloc(sizeof(*val));
            char key[UINT32_MAX_DIGITS + 3];
            sprintf(key, "%d", (uint32_t) (random() % manager->numEntries));

            if(UR_OK != ubjs_prmtv_object_get(nestedObj, strlen(key), key, &item)) {
                sprintf(msg, "Failed to read int64 value for update operation in object at position: %d", i);
                BENCH_UBJSON_ERROR_WRITE(manager->error, msg, i);
                return false;
            }
            ubjs_prmtv_int64_get(item, val);
            *val = random() % INT64_MAX;
            timePassed = clock();
            manager->updates++;
        }
    }
    return true;
}

bool bench_ubjson_update_string(bench_ubjson_mgr *manager, uint32_t numOperations, container_type type)
{
    ubjs_prmtv *item = 0;
    char msg[ERROR_MSG_SIZE];
    clock_t startPoint = clock();
    clock_t timePassed = clock();

    if(type == BENCH_CONTAINER_TYPE_ARRAY) {

        for (uint32_t i = 0; i < numOperations || timePassed - startPoint < BENCH_OPERATION_MIN_TIME; i++) {
            ubjs_prmtv *arrayObj;
            ubjs_prmtv_object_get(manager->obj, 1, "0", &arrayObj);

            if (UR_OK != ubjs_prmtv_array_get_at(arrayObj, random() % manager->numEntries, &item)) {
                sprintf(msg, "Failed to read int64 value for update operation in array at position: %d", i);
                BENCH_UBJSON_ERROR_WRITE(manager->error, msg, i);
                return false;
            }

            ubjs_prmtv_str_t *itemv;
            itemv = (ubjs_prmtv_str_t *)item;
            int64_t _val = random() % INT64_MAX;
            char val[(int) (floor(log10(labs(_val))) + 1)];
            sprintf(val, "%ld", _val);
            memcpy(itemv->value, val, strlen(val));
            timePassed = clock();
            manager->updates++;
        }
    } else if (type == BENCH_CONTAINER_TYPE_OBJECT) {

        for (uint32_t i = 0; i < numOperations || timePassed - startPoint < BENCH_OPERATION_MIN_TIME; i++) {
            ubjs_prmtv *nestedObj;
            ubjs_prmtv_object_get(manager->obj, 1, "0", &nestedObj);
            int64_t *val = malloc(sizeof(*val));
            char key[UINT32_MAX_DIGITS + 3];
            sprintf(key, "%d", (uint32_t) (random() % manager->numEntries));

            if(UR_OK != ubjs_prmtv_object_get(nestedObj, strlen(key), key, &item)) {
                sprintf(msg, "Failed to read int64 value for update operation in object at position: %d", i);
                BENCH_UBJSON_ERROR_WRITE(manager->error, msg, i);
                return false;
            }
            ubjs_prmtv_int64_get(item, val);
            *val = random() % INT64_MAX;
            timePassed = clock();
            manager->updates++;
        }
    }
    return true;
}

bool bench_ubjson_delete(bench_ubjson_mgr *manager, uint32_t numOperations, container_type type)
{
    char msg[ERROR_MSG_SIZE];
    int8_t entryList[manager->numEntries];
    //clock_t startPoint = clock();
    //clock_t timePassed = clock();

    if(type == BENCH_CONTAINER_TYPE_ARRAY) {

        for(uint32_t i = 0; i < numOperations /*|| timePassed - startPoint < BENCH_OPERATION_MIN_TIME*/; i++) {
            ubjs_prmtv *arrayObj;
            ubjs_prmtv_object_get(manager->obj, 1, "0", &arrayObj);
            uint32_t j;
            do {
                j = random() % manager->numEntries;
            } while(entryList[j] != 0);
            entryList[j] = 1;

            if(UR_OK != ubjs_prmtv_array_delete_at(arrayObj, j)) {
                sprintf(msg, "Failed to delete value in array at position: %d", i);
                BENCH_UBJSON_ERROR_WRITE(manager->error, msg, i);
                return false;
            }
            manager->numEntries--;
            //timePassed = clock();
        }
    } else if(type == BENCH_CONTAINER_TYPE_OBJECT) {

        for (uint32_t i = 0; i < numOperations /*|| timePassed - startPoint < BENCH_OPERATION_MIN_TIME*/; i++) {
            ubjs_prmtv *nestedObj;
            ubjs_prmtv_object_get(manager->obj, 1, "0", &nestedObj);
            char key[UINT32_MAX_DIGITS + 1];

            uint32_t j;
            do {
                j = random() % manager->numEntries;
            } while(entryList[j] != 0);
            entryList[j] = 1;
            sprintf(key, "%d", j);

            if (UR_OK != ubjs_prmtv_object_delete(nestedObj, strlen(key), key)) {
                sprintf(msg, "Failed to delete value in array at position: %d", i);
                BENCH_UBJSON_ERROR_WRITE(manager->error, msg, i);
                return false;
            }
            //timePassed = clock();
        }
    }

    return true;
}

bool bench_ubjson_test(bench_ubjson_mgr *manager) {
    ERROR_IF_NULL(manager)

    char msg[ERROR_MSG_SIZE];
    int32_t testData[] = {1, 2, 3, 4, 5, 6, 7, 8, 9 , 10};
    uint8_t testLen = 10;
    {
        ubjs_prmtv *arrayObj;
        ubjs_prmtv_array(manager->lib, &arrayObj);
        ubjs_prmtv_object_set(manager->obj, 1, "0", arrayObj);
        ubjs_prmtv *item;
        for (uint8_t i = 0; i < testLen; i++) {
            int32_t val = testData[i];

            ubjs_prmtv_int32(manager->lib, val, &item);
            if (UR_OK != ubjs_prmtv_array_add_last(arrayObj, item)) {
                sprintf(msg, "Failed to insert int32 value '%d' into array at position: %d", val, i);
                BENCH_UBJSON_ERROR_WRITE(manager->error, msg, i);
                return false;
            }
        }
    }
    {
        for (uint8_t i = 0; i < testLen; i++) {
            ubjs_prmtv *arrayObj;
            ubjs_prmtv_object_get(manager->obj, 1, "0", &arrayObj);
            ubjs_prmtv *item;
            int32_t val;
            if (UR_OK != ubjs_prmtv_array_get_at(arrayObj, i, &item)) {
                sprintf(msg, "Failed to read value in array at position: %d", i);
                BENCH_UBJSON_ERROR_WRITE(manager->error, msg, i);
                return false;
            }
            ubjs_prmtv_int32_get(item, &val);
            assert(val == testData[i]);
        }
    }
    return true;
}

bool bench_ubjson_execute_benchmark_operation_int8(bench_ubjson_mgr *manager, bench_operation_type opType,
        uint32_t numOperations, container_type contType)
{
    if(opType == BENCH_OP_TYPE_INSERT) {
        return bench_ubjson_insert_int8(manager, numOperations, contType);
    } else if(opType == BENCH_OP_TYPE_READ) {
        return bench_ubjson_read(manager, numOperations, contType);
    } else if(opType == BENCH_OP_TYPE_UPDATE) {
        return bench_ubjson_update_int8(manager, numOperations, contType);
    } else if(opType == BENCH_OP_TYPE_DELETE) {
        // Random access deletions are not that great.
        return bench_ubjson_delete(manager, numOperations, contType);
        //return true;
    } else {
        return false;
    }
}

bool bench_ubjson_execute_benchmark_operation_int16(bench_ubjson_mgr *manager, bench_operation_type opType,
        uint32_t numOperations, container_type contType)
{
    if(opType == BENCH_OP_TYPE_INSERT) {
        return bench_ubjson_insert_int16(manager, numOperations, contType);
    } else if(opType == BENCH_OP_TYPE_READ) {
        return bench_ubjson_read(manager, numOperations, contType);
    } else if(opType == BENCH_OP_TYPE_UPDATE) {
        return bench_ubjson_update_int16(manager, numOperations, contType);
    } else if(opType == BENCH_OP_TYPE_DELETE) {
        // Random access deletions are not that great.
        return bench_ubjson_delete(manager, numOperations, contType);
        //return true;
    } else {
        return false;
    }
}

bool bench_ubjson_execute_benchmark_operation_int32(bench_ubjson_mgr *manager, bench_operation_type opType,
        uint32_t numOperations, container_type contType)
{
    if(opType == BENCH_OP_TYPE_INSERT) {
        return bench_ubjson_insert_int32(manager, numOperations, contType);
    } else if(opType == BENCH_OP_TYPE_READ) {
        return bench_ubjson_read(manager, numOperations, contType);
    } else if(opType == BENCH_OP_TYPE_UPDATE) {
        return bench_ubjson_update_int32(manager, numOperations, contType);
    } else if(opType == BENCH_OP_TYPE_DELETE) {
        // Random access deletions are not that great.
        return bench_ubjson_delete(manager, numOperations, contType);
        //return true;
    } else {
        return false;
    }
}

bool bench_ubjson_execute_benchmark_operation_int64(bench_ubjson_mgr *manager, bench_operation_type opType,
        uint32_t numOperations, container_type contType)
{
    if(opType == BENCH_OP_TYPE_INSERT) {
        return bench_ubjson_insert_int64(manager, numOperations, contType);
    } else if(opType == BENCH_OP_TYPE_READ) {
        return bench_ubjson_read(manager, numOperations, contType);
    } else if(opType == BENCH_OP_TYPE_UPDATE) {
        return bench_ubjson_update_int64(manager, numOperations, contType);
    } else if(opType == BENCH_OP_TYPE_DELETE) {
        // Random access deletions are not that great.
        return bench_ubjson_delete(manager, numOperations, contType);
        //return true;
    } else {
        return false;
    }
}

bool bench_ubjson_execute_benchmark_operation_string(bench_ubjson_mgr *manager, bench_operation_type opType,
                                                    uint32_t numOperations, container_type contType)
{
    if(opType == BENCH_OP_TYPE_INSERT) {
        return bench_ubjson_insert_string(manager, numOperations, contType);
    } else if(opType == BENCH_OP_TYPE_READ) {
        return bench_ubjson_read(manager, numOperations, contType);
    } else if(opType == BENCH_OP_TYPE_UPDATE) {
        return bench_ubjson_update_string(manager, numOperations, contType);
    } else if(opType == BENCH_OP_TYPE_DELETE) {
        return bench_ubjson_delete(manager, numOperations, contType);
    } else {
        return false;
    }
}

bool bench_ubjson_execute_benchmark_operation(bench_ubjson_mgr *manager, bench_type type, bench_operation_type opType,
                                              uint32_t numOperations, container_type contType)
{
    ERROR_IF_NULL(manager);
    ERROR_IF_NULL(type);
    ERROR_IF_NULL(opType);
    ERROR_IF_NULL(contType);

    if(type == BENCH_TYPE_INT8) {
        return bench_ubjson_execute_benchmark_operation_int8(manager, opType, numOperations, contType);
    } else if(type == BENCH_TYPE_INT16) {
        return bench_ubjson_execute_benchmark_operation_int16(manager, opType, numOperations, contType);
    } else if(type == BENCH_TYPE_INT32) {
        return bench_ubjson_execute_benchmark_operation_int32(manager, opType, numOperations, contType);
    } else if(type == BENCH_TYPE_INT64) {
        return bench_ubjson_execute_benchmark_operation_int64(manager, opType, numOperations, contType);
    } else if(type == BENCH_TYPE_STRING) {
        return bench_ubjson_execute_benchmark_operation_string(manager, opType, numOperations, contType);
    } else if(type == BENCH_TYPE_TEST) {
        return bench_ubjson_test(manager);
    } else {
        return false;
    }
}
