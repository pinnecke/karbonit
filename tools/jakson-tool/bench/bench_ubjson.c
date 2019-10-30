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

bool bench_ubjson_change_val_int32(bench_ubjson_mgr *manager, ubjs_array_iterator *it, char *key, int32_t newVal)
{
    ERROR_IF_NULL(manager)
    ERROR_IF_NULL(key)
    ERROR_IF_NULL(newVal)
    UNUSED(it)

    ubjs_prmtv *item = 0;
    ubjs_prmtv_int32(manager->lib, newVal, &item);

    if(UR_OK == ubjs_prmtv_object_set(manager->obj, sizeof(key), key, item))
        return true;
    free(item);
    return false;
}

bool bench_ubjson_convert_entry_int32(bench_ubjson_mgr *manager, ubjs_array_iterator *it, char *key)
{
    ERROR_IF_NULL(manager)
    ERROR_IF_NULL(key)
    UNUSED(it)

    ubjs_prmtv *item = 0;
    int32_t itemVal;

    ubjs_prmtv_object_get(manager->obj, sizeof(key), key, &item);
    ubjs_prmtv_int32_get(item, &itemVal);
    ubjs_prmtv_int32(manager->lib, itemVal, &item);

    if(UR_OK == ubjs_prmtv_object_set(manager->obj, sizeof(key), key, item)) {
        return true;
    }
    free(item);
    return false;
}

bool bench_ubjson_convert_entry_int64(bench_ubjson_mgr *manager, ubjs_array_iterator *it, char *key)
{
    ERROR_IF_NULL(manager)
    ERROR_IF_NULL(key)
    UNUSED(it)

    ubjs_prmtv *item = 0;
    int64_t itemVal;

    ubjs_prmtv_object_get(manager->obj, sizeof(key), key, &item);
    ubjs_prmtv_int64_get(item, &itemVal);
    ubjs_prmtv_int64(manager->lib, itemVal, &item);

    if(UR_OK == ubjs_prmtv_object_set(manager->obj, sizeof(key), key, item)) {
        return true;
    }
    free(item);
    return false;

    return false;
}

bool bench_ubjson_delete_int32(bench_ubjson_mgr *manager, ubjs_array_iterator *it, char *key)
{
    ERROR_IF_NULL(manager)
    ERROR_IF_NULL(key)
    UNUSED(it)

    return ubjs_prmtv_object_delete(manager->obj, sizeof(uint32_t), key);
}

bool bench_ubjson_execute_benchmark(bench_ubjson_mgr *manager, const char *benchType)
{
    ERROR_IF_NULL(manager);
    UNUSED(benchType);

    assert(bench_ubjson_insert_int32(manager,"Test1", 41));
    assert(bench_ubjson_insert_int32(manager, "Test2", 42));
    assert(bench_ubjson_insert_int32(manager, "Test3", 43));
    assert(bench_ubjson_insert_int32(manager, "Test4", 44));
    assert(bench_ubjson_insert_int32(manager, "Test5", 45));
    assert(bench_ubjson_insert_int32(manager, "Test6", 46));

/*
    if(!bench_ubjson_find_int32(manager, 0, "Test3", 0))
        return bench_ubjson_error_write(manager->error, "Failed to find int32 value.", 0);

    if(!bench_ubjson_change_val_int32(manager, 0, "Test3", 21))
        return bench_ubjson_error_write(manager->error, "Failed to change int32 value.", 0);

    if(!bench_ubjson_convert_entry_int64(manager, 0, "Test4"))
        return bench_ubjson_error_write(manager->error, "Failed to convert to int64 entry.", 0);

    if(!bench_ubjson_convert_entry_int32(manager, 0, "Test4"))
        return bench_ubjson_error_write(manager->error, "Failed to convert to int32 entry.", 0);

    if(!bench_ubjson_delete_int32(manager, 0, "Test4"))
        return bench_ubjson_error_write(manager->error, "Failed to delete int32 entry.", 0);
*/
    return true;
}
