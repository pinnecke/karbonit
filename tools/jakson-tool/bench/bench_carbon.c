
#include "bench_carbon.h"
#include "bench_format_handler.h"

bool bench_carbon_error_create(bench_carbon_error *carbonError, bench_error *benchError)
{
    ERROR_IF_NULL(carbonError)
    ERROR_IF_NULL(benchError)

    struct err *err = malloc(sizeof(*err));
    error_init(err);

    carbonError->benchErr = benchError;
    carbonError->err = err;

    return true;
}

bool bench_carbon_error_destroy(bench_carbon_error *error)
{
    ERROR_IF_NULL(error)

    CHECK_SUCCESS(error_drop(error->err));
    free(error);

    return true;
}

bool bench_carbon_error_write(bench_carbon_error *error, const char *msg, const char *file, u32 line, size_t docOffset)
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

bool bench_carbon_mgr_create_from_file(bench_carbon_mgr *manager, bench_carbon_error *carbonError, bench_error *benchError, const char *filePath)
{
    ERROR_IF_NULL(manager)
    ERROR_IF_NULL(carbonError)
    ERROR_IF_NULL(benchError)
    ERROR_IF_NULL(filePath)

    carbon *doc = malloc(sizeof(*doc));
    bench_carbon_error_create(carbonError, benchError);

    FILE *f = fopen(filePath, "rb");
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *jsonContent = MALLOC(fsize + 1);
    size_t nread = fread(jsonContent, fsize, 1, f);
    UNUSED(nread)
    fclose(f);
    jsonContent[fsize] = 0;

    CHECK_SUCCESS(carbon_from_json(doc, jsonContent, CARBON_KEY_NOKEY, NULL, carbonError->err));

    manager->error = carbonError;
    manager->doc = doc;

    return true;
}

bool bench_carbon_mgr_create_empty(bench_carbon_mgr *manager, bench_carbon_error *carbonError, bench_error *benchError)
{
    ERROR_IF_NULL(manager)
    ERROR_IF_NULL(carbonError)
    ERROR_IF_NULL(benchError)

    carbon *doc = malloc(sizeof(*doc));
    bench_carbon_error_create(carbonError, benchError);

    if(!carbon_create_empty(doc, CARBON_LIST_UNSORTED_MULTISET, CARBON_KEY_NOKEY))
        BENCH_CARBON_ERROR_WRITE(carbonError, "Failed to create CARBON doc", 0);

    manager->doc = doc;
    manager->error = carbonError;

    return true;
}

bool bench_carbon_append_doc(bench_carbon_mgr *manager, const char *filePath)
{
    ERROR_IF_NULL(manager);
    ERROR_IF_NULL(filePath);

    carbon *doc = malloc(sizeof(*doc));
    carbon_array_it arr_it, append_arr_it;
    carbon_insert ins;
    carbon_insert_array_state state;
    static int doc_count = 0;
    char doc_key[32];

    FILE *f = fopen(filePath, "rb");
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *jsonContent = MALLOC(fsize + 1);
    size_t nread = fread(jsonContent, fsize, 1, f);
    UNUSED(nread)
    fclose(f);
    jsonContent[fsize] = 0;

    CHECK_SUCCESS(carbon_from_json(doc, jsonContent, CARBON_KEY_NOKEY, NULL, manager->error->err));

    //carbon_revise_begin(ctx, rev_doc, manager->doc);
    //carbon_patch_begin(&arr_it, manager->doc);
    carbon_array_it_insert_begin(&ins, &arr_it);

    carbon_iterator_open(&append_arr_it, doc);
    offset_t start = carbon_array_it_tell(&append_arr_it);
    carbon_array_it_fast_forward(&append_arr_it);
    offset_t end = carbon_array_it_tell(&append_arr_it);
    //carbon_insert_prop_binary(ins, doc_key, mem->memblock[start], end - start, );
    //obj_ins = *carbon_insert_object_begin(&state, &ins, 6);

    sprintf(doc_key, "%d", doc_count);
    carbon_insert_prop_array_begin(&state, &ins, doc_key, 1);
    if(!memblock_write(manager->doc->memblock, ins.position, &memblock_raw_data(doc->memblock)[start], end - start)) {
        BENCH_CARBON_ERROR_WRITE(manager->error, "Failed to append document.", 0)
        return false;
    }
    carbon_insert_prop_array_end(&state);

    return true;
}

// TODO : Implement
bool bench_carbon_convert_doc(size_t *conv_size, bench_carbon_mgr *manager, const char *filePath)
{
    ERROR_IF_NULL(conv_size);
    ERROR_IF_NULL(manager);
    ERROR_IF_NULL(filePath);

    return false;
}

bool bench_carbon_mgr_destroy(bench_carbon_mgr *manager)
{
    ERROR_IF_NULL(manager)
    CHECK_SUCCESS(carbon_drop(manager->doc));
    CHECK_SUCCESS(bench_carbon_error_destroy(manager->error));

    return true;
}

size_t bench_carbon_get_doc_size(bench_carbon_mgr *manager)
{
    ERROR_IF_NULL(manager);

    return strlen(carbon_to_json_compact_dup(manager->doc));
}

bool bench_carbon_to_file(bench_carbon_mgr *manager, const char *filePath)
{
    ERROR_IF_NULL(manager);
    ERROR_IF_NULL(filePath);

    FILE *file = fopen(filePath, "w");

    fprintf(file, "%s", carbon_to_json_compact_dup(manager->doc));

    fclose(file);

    return true;
}
/*
bool bench_carbon_insert_int32(bench_carbon_mgr *manager, carbon_object_it *it, carbon_insert *ins, char *key,  int32_t val)
{
    ERROR_IF_NULL(manager)
    UNUSED(it)
    ERROR_IF_NULL(ins)
    ERROR_IF_NULL(key)
    //carbon_insert ins;
    //carbon_int_insert_create_for_object(&ins, it);

    // TODO : Initialize inserter and put it at the end of doc if null

    return carbon_insert_prop_i32(ins, key, val);
}
*/
#define MAX_KEY_LENGTH 128
bool bench_carbon_find_int32(bench_carbon_mgr *manager, carbon_object_it *it, char *key, int32_t val)
{
    ERROR_IF_NULL(manager)
    UNUSED(it)
    ERROR_IF_NULL(key)
    UNUSED(val)
    // Convert to dot path
    char _key[MAX_KEY_LENGTH];
    snprintf(_key, sizeof(_key), "0.\"%s%s", key, "\"");
    carbon_find find;
    //carbon_patch_find_begin(&find, _key, manager->doc);
    carbon_field_type_e type;
    carbon_find_result_type(&type, &find);

    if(type != CARBON_FIELD_NUMBER_I32) {
        BENCH_CARBON_ERROR_WRITE(manager->error, strcat("Could not find int32 key ", _key), 0);
        return false;
    }

    //it = FN_GET_PTR(carbon_object_it, carbon_find_result_object(&find));

    return true;
}

bool bench_carbon_insert_int8(bench_carbon_mgr *manager, uint32_t numOperations, container_type type)
{
    carbon *rev_doc = malloc(sizeof(*rev_doc));
    carbon_revise revise;
    carbon_insert ins;
    carbon_array_it it;
    char msg[ERROR_MSG_SIZE];

    carbon_revise_begin(&revise, rev_doc, manager->doc);
    carbon_revise_iterator_open(&it, &revise);
    carbon_array_it_insert_begin(&ins, &it);
    if(type == BENCH_CONTAINER_TYPE_ARRAY) {
        carbon_insert_array_state arrayState;
        carbon_insert *nestedIns = carbon_insert_array_begin(&arrayState, &ins, numOperations);
        for (uint32_t i = 0; i < numOperations; i++) {
            int8_t val = random() % INT8_MAX;
            char key[UINT32_MAX_DIGITS + 1];
            sprintf(key, "%d", i);

            if (!carbon_insert_i8(nestedIns, val)) {
                sprintf(msg, "Failed to insert int8 value '%d' into array at position: %d", val, i);
                BENCH_CARBON_ERROR_WRITE(manager->error, msg, i);
                return false;
            }
            manager->numEntries++;
        }
        carbon_insert_array_end(&arrayState);
    } else if (type == BENCH_CONTAINER_TYPE_COLUMN) {
        carbon_insert_column_state columnState;
        carbon_insert *nestedIns = carbon_insert_column_begin(&columnState, &ins, CARBON_COLUMN_TYPE_I32, numOperations);
        for (uint32_t i = 0; i < numOperations; i++) {
            int8_t val = random() % INT8_MAX;
            char key[UINT32_MAX_DIGITS + 1];
            sprintf(key, "%d", i);

            if (!carbon_insert_i8(nestedIns, val)) {
                sprintf(msg, "Failed to insert int8 value '%d' into column at position: %d", val, i);
                BENCH_CARBON_ERROR_WRITE(manager->error, msg, i);
                return false;
            }
            manager->numEntries++;
        }
        carbon_insert_column_end(&columnState);
    } else if (type == BENCH_CONTAINER_TYPE_OBJECT) {
        carbon_insert_object_state objectState;
        carbon_insert *nestedIns = carbon_insert_object_begin(&objectState, &ins, numOperations);
        for (uint32_t i = 0; i < numOperations; i++) {
            int8_t val = random() % INT8_MAX;
            char key[UINT32_MAX_DIGITS + 1];
            sprintf(key, "%d", i);

            if (!carbon_insert_prop_i8(nestedIns, key, val)) {
                sprintf(msg, "Failed to insert int8 value '%d' into object at position: %d", val, i);
                BENCH_CARBON_ERROR_WRITE(manager->error, msg, i);
                return false;
            }
            manager->numEntries++;
        }
        carbon_insert_object_end(&objectState);
    }
    carbon_array_it_insert_end(&ins);
    carbon_revise_iterator_close(&it);
    carbon_revise_end(&revise);

    carbon_drop(manager->doc);
    manager->doc = rev_doc;

    return true;
}

bool bench_carbon_insert_int16(bench_carbon_mgr *manager, uint32_t numOperations, container_type type)
{
    carbon *rev_doc = malloc(sizeof(*rev_doc));
    carbon_revise revise;
    carbon_insert ins;
    carbon_array_it it;
    char msg[ERROR_MSG_SIZE];

    carbon_revise_begin(&revise, rev_doc, manager->doc);
    carbon_revise_iterator_open(&it, &revise);
    carbon_array_it_insert_begin(&ins, &it);
    if(type == BENCH_CONTAINER_TYPE_ARRAY) {
        carbon_insert_array_state arrayState;
        carbon_insert *nestedIns = carbon_insert_array_begin(&arrayState, &ins, numOperations);
        for (uint32_t i = 0; i < numOperations; i++) {
            int16_t val = random() % INT16_MAX;
            char key[UINT32_MAX_DIGITS + 1];
            sprintf(key, "%d", i);

            if (!carbon_insert_i16(nestedIns, val)) {
                sprintf(msg, "Failed to insert int16 value '%d' into array at position: %d", val, i);
                BENCH_CARBON_ERROR_WRITE(manager->error, msg, i);
                return false;
            }
            manager->numEntries++;
        }
        carbon_insert_array_end(&arrayState);
    } else if (type == BENCH_CONTAINER_TYPE_COLUMN) {
        carbon_insert_column_state columnState;
        carbon_insert *nestedIns = carbon_insert_column_begin(&columnState, &ins, CARBON_COLUMN_TYPE_I16, numOperations);
        for (uint32_t i = 0; i < numOperations; i++) {
            int16_t val = random() % INT16_MAX;
            char key[UINT32_MAX_DIGITS + 1];
            sprintf(key, "%d", i);

            if (!carbon_insert_i16(nestedIns, val)) {
                sprintf(msg, "Failed to insert int16 value '%d' into column at position: %d", val, i);
                BENCH_CARBON_ERROR_WRITE(manager->error, msg, i);
                return false;
            }
            manager->numEntries++;
        }
        carbon_insert_column_end(&columnState);
    } else if (type == BENCH_CONTAINER_TYPE_OBJECT) {
        carbon_insert_object_state objectState;
        carbon_insert *nestedIns = carbon_insert_object_begin(&objectState, &ins, numOperations);
        for (uint32_t i = 0; i < numOperations; i++) {
            int16_t val = random() % INT16_MAX;
            char key[UINT32_MAX_DIGITS + 1];
            sprintf(key, "%d", i);

            if (!carbon_insert_prop_i16(nestedIns, key, val)) {
                sprintf(msg, "Failed to insert int16 value '%d' into object at position: %d", val, i);
                BENCH_CARBON_ERROR_WRITE(manager->error, msg, i);
                return false;
            }
            manager->numEntries++;
        }
        carbon_insert_object_end(&objectState);
    }
    carbon_array_it_insert_end(&ins);
    carbon_revise_iterator_close(&it);
    carbon_revise_end(&revise);

    carbon_drop(manager->doc);
    manager->doc = rev_doc;

    return true;
}

bool bench_carbon_insert_int32(bench_carbon_mgr *manager, uint32_t numOperations, container_type type)
{
    carbon *rev_doc = malloc(sizeof(*rev_doc));
    carbon_revise revise;
    carbon_insert ins;
    carbon_array_it it;
    char msg[ERROR_MSG_SIZE];

    carbon_revise_begin(&revise, rev_doc, manager->doc);
    carbon_revise_iterator_open(&it, &revise);
    carbon_array_it_insert_begin(&ins, &it);
    if(type == BENCH_CONTAINER_TYPE_ARRAY) {
        carbon_insert_array_state arrayState;
        carbon_insert *nestedIns = carbon_insert_array_begin(&arrayState, &ins, numOperations);
        for (uint32_t i = 0; i < numOperations; i++) {
            int32_t val = random() % INT32_MAX;
            char key[UINT32_MAX_DIGITS + 1];
            sprintf(key, "%d", i);

            if (!carbon_insert_i32(nestedIns, val)) {
                sprintf(msg, "Failed to insert int32 value '%d' into array at position: %d", val, i);
                BENCH_CARBON_ERROR_WRITE(manager->error, msg, i);
                return false;
            }
            manager->numEntries++;
        }
        carbon_insert_array_end(&arrayState);
    } else if (type == BENCH_CONTAINER_TYPE_COLUMN) {
        carbon_insert_column_state columnState;
        carbon_insert *nestedIns = carbon_insert_column_begin(&columnState, &ins, CARBON_COLUMN_TYPE_I32, numOperations);
        for (uint32_t i = 0; i < numOperations; i++) {
            int32_t val = random() % INT32_MAX;
            char key[UINT32_MAX_DIGITS + 1];
            sprintf(key, "%d", i);

            if (!carbon_insert_i32(nestedIns, val)) {
                sprintf(msg, "Failed to insert int32 value '%d' into column at position: %d", val, i);
                BENCH_CARBON_ERROR_WRITE(manager->error, msg, i);
                return false;
            }
            manager->numEntries++;
        }
        carbon_insert_column_end(&columnState);
    } else if (type == BENCH_CONTAINER_TYPE_OBJECT) {
        carbon_insert_object_state objectState;
        carbon_insert *nestedIns = carbon_insert_object_begin(&objectState, &ins, numOperations);
        for (uint32_t i = 0; i < numOperations; i++) {
            int32_t val = random() % INT32_MAX;
            char key[UINT32_MAX_DIGITS + 1];
            sprintf(key, "%d", i);

            if (!carbon_insert_prop_i32(nestedIns, key, val)) {
                sprintf(msg, "Failed to insert int32 value '%d' into object at position: %d", val, i);
                BENCH_CARBON_ERROR_WRITE(manager->error, msg, i);
                return false;
            }
            manager->numEntries++;
        }
        carbon_insert_object_end(&objectState);
    }
    carbon_array_it_insert_end(&ins);
    carbon_revise_iterator_close(&it);
    carbon_revise_end(&revise);

    carbon_drop(manager->doc);
    manager->doc = rev_doc;

    return true;
}

bool bench_carbon_insert_int64(bench_carbon_mgr *manager, uint32_t numOperations, container_type type)
{
    carbon *rev_doc = malloc(sizeof(*rev_doc));
    carbon_revise revise;
    carbon_insert ins;
    carbon_array_it it;
    char msg[ERROR_MSG_SIZE];

    carbon_revise_begin(&revise, rev_doc, manager->doc);
    carbon_revise_iterator_open(&it, &revise);
    carbon_array_it_insert_begin(&ins, &it);
    if(type == BENCH_CONTAINER_TYPE_ARRAY) {
        carbon_insert_array_state arrayState;
        carbon_insert *nestedIns = carbon_insert_array_begin(&arrayState, &ins, numOperations);
        for (uint32_t i = 0; i < numOperations; i++) {
            int64_t val = random() % INT64_MAX;
            char key[UINT32_MAX_DIGITS + 1];
            sprintf(key, "%d", i);

            if (!carbon_insert_i64(nestedIns, val)) {
                sprintf(msg, "Failed to insert int64 value '%ld' into array at position: %d", val, i);
                BENCH_CARBON_ERROR_WRITE(manager->error, msg, i);
                return false;
            }
            manager->numEntries++;
        }
        carbon_insert_array_end(&arrayState);
    } else if (type == BENCH_CONTAINER_TYPE_COLUMN) {
        carbon_insert_column_state columnState;
        carbon_insert *nestedIns = carbon_insert_column_begin(&columnState, &ins, CARBON_COLUMN_TYPE_I64, numOperations);
        for (uint32_t i = 0; i < numOperations; i++) {
            int64_t val = random() % INT64_MAX;
            char key[UINT32_MAX_DIGITS + 1];
            sprintf(key, "%d", i);

            if (!carbon_insert_i64(nestedIns, val)) {
                sprintf(msg, "Failed to insert int64 value '%ld' into column at position: %d", val, i);
                BENCH_CARBON_ERROR_WRITE(manager->error, msg, i);
                return false;
            }
            manager->numEntries++;
        }
        carbon_insert_column_end(&columnState);
    } else if (type == BENCH_CONTAINER_TYPE_OBJECT) {
        carbon_insert_object_state objectState;
        carbon_insert *nestedIns = carbon_insert_object_begin(&objectState, &ins, numOperations);
        for (uint32_t i = 0; i < numOperations; i++) {
            int64_t val = random() % INT64_MAX;
            char key[UINT32_MAX_DIGITS + 1];
            sprintf(key, "%d", i);

            if (!carbon_insert_prop_i64(nestedIns, key, val)) {
                sprintf(msg, "Failed to insert int64 value '%ld' into object at position: %d", val, i);
                BENCH_CARBON_ERROR_WRITE(manager->error, msg, i);
                return false;
            }
            manager->numEntries++;
        }
        carbon_insert_object_end(&objectState);
    }
    carbon_array_it_insert_end(&ins);
    carbon_revise_iterator_close(&it);
    carbon_revise_end(&revise);

    carbon_drop(manager->doc);
    manager->doc = rev_doc;

    return true;
}

bool bench_carbon_read(bench_carbon_mgr *manager, uint32_t numOperations, container_type type)
{
    carbon_find find;
    char msg[ERROR_MSG_SIZE];

    if(type == BENCH_CONTAINER_TYPE_ARRAY || type == BENCH_CONTAINER_TYPE_COLUMN) {
        for (uint32_t i = 0; i < numOperations; i++) {
            char path[UINT32_MAX_DIGITS + 3];
            sprintf(path, "0.%d", 1/*(uint32_t) (random() % manager->numEntries)*/);
            carbon_find_open(&find, path, manager->doc);
            if (!carbon_find_has_result(&find)) {
                sprintf(msg, "Failed to read item:%s", path);
                BENCH_CARBON_ERROR_WRITE(manager->error, msg, 0);
                return false;
            }
        }
    } else if(type == BENCH_CONTAINER_TYPE_OBJECT) {
        for (uint32_t i = 0; i < numOperations; i++) {
            char path[UINT32_MAX_DIGITS + 5];
            sprintf(path, "0.\"%d\"", (uint32_t) (random() % manager->numEntries));
            carbon_find_open(&find, path, manager->doc);
            if (!carbon_find_has_result(&find)) {
                sprintf(msg, "Failed to read item:%s", path);
                BENCH_CARBON_ERROR_WRITE(manager->error, msg, 0);
                return false;
            }
        }
    }


    carbon_find_drop(&find);

    return true;
}

bool bench_carbon_update_int8(bench_carbon_mgr *manager, uint32_t numOperations)
{
    carbon *rev_doc = malloc(sizeof(*rev_doc));
    carbon_revise revise;
    char msg[ERROR_MSG_SIZE];

    carbon_revise_begin(&revise, rev_doc, manager->doc);
    for(uint32_t i = 0; i < numOperations; i++) {
        char path[UINT32_MAX_DIGITS + 3];
        int8_t newVal = random() % INT8_MAX;
        sprintf(path, "0.%d", (uint32_t) (random() % numOperations));

        if(!carbon_update_set_i8(&revise, path, newVal)) {
            sprintf(msg, "Failed to update int8 item:%s", path);
            BENCH_CARBON_ERROR_WRITE(manager->error, msg, 0);
            return false;
        }
    }
    carbon_revise_end(&revise);

    carbon_drop(manager->doc);
    manager->doc = rev_doc;

    return true;
}

bool bench_carbon_update_int16(bench_carbon_mgr *manager, uint32_t numOperations)
{
    carbon *rev_doc = malloc(sizeof(*rev_doc));
    carbon_revise revise;
    char msg[ERROR_MSG_SIZE];

    carbon_revise_begin(&revise, rev_doc, manager->doc);
    for(uint32_t i = 0; i < numOperations; i++) {
        char path[UINT32_MAX_DIGITS + 3];
        int16_t newVal = random() % INT16_MAX;
        sprintf(path, "0.%d", (uint32_t) (random() % numOperations));

        if(!carbon_update_set_i16(&revise, path, newVal)) {
            sprintf(msg, "Failed to update int16 item:%s", path);
            BENCH_CARBON_ERROR_WRITE(manager->error, msg, 0);
            return false;
        }
    }
    carbon_revise_end(&revise);

    carbon_drop(manager->doc);
    manager->doc = rev_doc;

    return true;
}

bool bench_carbon_update_int32(bench_carbon_mgr *manager, uint32_t numOperations, container_type type)
{
    carbon *rev_doc = malloc(sizeof(*rev_doc));
    carbon_revise revise;
    char msg[ERROR_MSG_SIZE];

    carbon_revise_begin(&revise, rev_doc, manager->doc);
    if(type == BENCH_CONTAINER_TYPE_ARRAY || type == BENCH_CONTAINER_TYPE_COLUMN) {
        for (uint32_t i = 0; i < numOperations; i++) {
            char path[UINT32_MAX_DIGITS + 3];
            int32_t newVal = random() % INT32_MAX;
            sprintf(path, "0.%d", (uint32_t) (random() % numOperations));

            if (!carbon_update_set_i32(&revise, path, newVal)) {
                sprintf(msg, "Failed to update int32 item:%s", path);
                BENCH_CARBON_ERROR_WRITE(manager->error, msg, 0);
                return false;
            }
        }
    } else if(type == BENCH_CONTAINER_TYPE_OBJECT) {
        for (uint32_t i = 0; i < numOperations; i++) {
            char path[UINT32_MAX_DIGITS + 5];
            int32_t newVal = random() % INT32_MAX;
            sprintf(path, "0.\"%d\"", (uint32_t) (random() % manager->numEntries));

            // TODO : Make that work
            if (!carbon_update_set_i32(&revise, path, newVal)) {
                sprintf(msg, "Failed to update int32 item:%s", path);
                BENCH_CARBON_ERROR_WRITE(manager->error, msg, 0);
                return false;
            }
        }
    }
    carbon_revise_end(&revise);

    carbon_drop(manager->doc);
    manager->doc = rev_doc;

    return true;
}

bool bench_carbon_update_int64(bench_carbon_mgr *manager, uint32_t numOperations, container_type type)
{
    carbon *rev_doc = malloc(sizeof(*rev_doc));
    carbon_revise revise;
    char msg[ERROR_MSG_SIZE];

    carbon_revise_begin(&revise, rev_doc, manager->doc);
    if(type == BENCH_CONTAINER_TYPE_ARRAY || type == BENCH_CONTAINER_TYPE_COLUMN) {
        for (uint32_t i = 0; i < numOperations; i++) {
            char path[UINT32_MAX_DIGITS + 3];
            int64_t newVal = random() % INT64_MAX;
            sprintf(path, "0.%d", (uint32_t) (random() % numOperations));

            if (!carbon_update_set_i64(&revise, path, newVal)) {
                sprintf(msg, "Failed to update int64 item:%s", path);
                BENCH_CARBON_ERROR_WRITE(manager->error, msg, 0);
                return false;
            }
        }
    } else if(type == BENCH_CONTAINER_TYPE_OBJECT) {
        for (uint32_t i = 0; i < numOperations; i++) {
            char path[UINT32_MAX_DIGITS + 5];
            int64_t newVal = random() % INT64_MAX;
            sprintf(path, "0.\"%d\"", (uint32_t) (random() % manager->numEntries));

            // TODO : Make that work
            if (!carbon_update_set_i64(&revise, path, newVal)) {
                sprintf(msg, "Failed to update int64 item:%s", path);
                BENCH_CARBON_ERROR_WRITE(manager->error, msg, 0);
                return false;
            }
        }
    }
    carbon_revise_end(&revise);

    carbon_drop(manager->doc);
    manager->doc = rev_doc;

    return true;
}
/*
bool bench_carbon_change_val_int32(bench_carbon_mgr *manager, carbon_object_it *it, char *key, int32_t newVal)
{
    ERROR_IF_NULL(manager)
    if(!it) {
        ERROR_IF_NULL(key)
        carbon_find find;
        // Empty dot path searches everywhere?
        //carbon_patch_find_begin(&find, key, manager->doc);
        if (carbon_find_has_result(&find)) {
            carbon_field_type_e type;
            carbon_find_result_type(&type, &find);
            if (carbon_find_result_array(&find)) {
                // TODO : if array
            } else if (type == CARBON_FIELD_NUMBER_I32) {
                //it = FN_GET_PTR(carbon_object_it, carbon_find_result_object(&find));
            } else {
                // TODO : Error handling
                return false;
            }
        } else {
            //carbon_find_end(&find);
            return false;
        }
        //carbon_find_end(&find);
    }

    int32_t *val = 0;
    if (!carbon_object_it_i32_value(val, it)) {
        // TODO : Error handling
        return false;
    }
    *val = newVal;

    return true;
}
*/
bool bench_carbon_delete(bench_carbon_mgr *manager, uint32_t numOperations)
{
    char removedKeys[numOperations][UINT32_MAX_DIGITS + 3];
    int8_t entryList[manager->numEntries];
    carbon *rev_doc = malloc(sizeof(*rev_doc));
    carbon_revise revise;

    carbon_revise_begin(&revise, rev_doc, manager->doc);

    for(uint32_t i = 0; i < numOperations; i++) {
        char path[UINT32_MAX_DIGITS + 3];

        uint32_t j;
        do {
            j = random() % manager->numEntries;
        } while(entryList[j] != 0);
        sprintf(path, "0.%d", j);
        entryList[j] = 1;

        carbon_revise_remove(path, &revise);
        strcat(removedKeys[i], path);
        manager->numEntries--;
    }
    carbon_revise_end(&revise);

    carbon_drop(manager->doc);
    manager->doc = rev_doc;

    return true;
}

bool bench_carbon_execute_benchmark_operation_int8(bench_carbon_mgr *manager, bench_operation_type opType,
        uint32_t numOperations, container_type contType)
{
    if(opType == BENCH_OP_TYPE_INSERT) {
        return bench_carbon_insert_int8(manager, numOperations, contType);
    } else if(opType == BENCH_OP_TYPE_READ) {
        return bench_carbon_read(manager, numOperations, contType);
    } else if(opType == BENCH_OP_TYPE_UPDATE) {
        return bench_carbon_update_int8(manager, numOperations);
    } else if(opType == BENCH_OP_TYPE_DELETE) {
        // Random access deletions are not that great.
        return bench_carbon_delete(manager, numOperations);
        //return true;
    } else {
        return false;
    }
}

bool bench_carbon_execute_benchmark_operation_int16(bench_carbon_mgr *manager, bench_operation_type opType,
        uint32_t numOperations, container_type contType)
{
    if(opType == BENCH_OP_TYPE_INSERT) {
        return bench_carbon_insert_int16(manager, numOperations, contType);
    } else if(opType == BENCH_OP_TYPE_READ) {
        return bench_carbon_read(manager, numOperations, contType);
    } else if(opType == BENCH_OP_TYPE_UPDATE) {
        return bench_carbon_update_int16(manager, numOperations);
    } else if(opType == BENCH_OP_TYPE_DELETE) {
        // Random access deletions are not that great.
        return bench_carbon_delete(manager, numOperations);
        //return true;
    } else {
        return false;
    }
}

bool bench_carbon_execute_benchmark_operation_int32(bench_carbon_mgr *manager, bench_operation_type opType,
        uint32_t numOperations, container_type contType)
{
    if(opType == BENCH_OP_TYPE_INSERT) {
        return bench_carbon_insert_int32(manager, numOperations, contType);
    } else if(opType == BENCH_OP_TYPE_READ) {
        return bench_carbon_read(manager, numOperations, contType);
    } else if(opType == BENCH_OP_TYPE_UPDATE) {
        return bench_carbon_update_int32(manager, numOperations, contType);
    } else if(opType == BENCH_OP_TYPE_DELETE) {
        // Random access deletions are not that great.
        return bench_carbon_delete(manager, numOperations);
        //return true;
    } else {
        return false;
    }
}

bool bench_carbon_execute_benchmark_operation_int64(bench_carbon_mgr *manager, bench_operation_type opType,
        uint32_t numOperations, container_type contType)
{
    if(opType == BENCH_OP_TYPE_INSERT) {
        return bench_carbon_insert_int64(manager, numOperations, contType);
    } else if(opType == BENCH_OP_TYPE_READ) {
        return bench_carbon_read(manager, numOperations, contType);
    } else if(opType == BENCH_OP_TYPE_UPDATE) {
        return bench_carbon_update_int64(manager, numOperations, contType);
    } else if(opType == BENCH_OP_TYPE_DELETE) {
        // Random access deletions are not that great.
        return bench_carbon_delete(manager, numOperations);
        //return true;
    } else {
        return false;
    }
}

bool bench_carbon_execute_benchmark_operation(bench_carbon_mgr *manager, bench_type type, bench_operation_type opType,
        uint32_t numOperations, container_type contType)
{
    ERROR_IF_NULL(manager);
    ERROR_IF_NULL(type);
    ERROR_IF_NULL(opType);
    ERROR_IF_NULL(contType);

    if(type == BENCH_TYPE_INT8) {
        return bench_carbon_execute_benchmark_operation_int8(manager, opType, numOperations, contType);
    } else if(type == BENCH_TYPE_INT16) {
        return bench_carbon_execute_benchmark_operation_int16(manager, opType, numOperations, contType);
    } else if(type == BENCH_TYPE_INT32) {
        return bench_carbon_execute_benchmark_operation_int32(manager, opType, numOperations, contType);
    } else if(type == BENCH_TYPE_INT64) {
        return bench_carbon_execute_benchmark_operation_int64(manager, opType, numOperations, contType);
    } else {
        return false;
    }
    /*
    carbon_array_it arr_it;
    //carbon_object_it obj_it;
    //carbon_field_type_e type;
    //carbon *rev_doc;
    carbon_insert ins, obj_ins;
    carbon_insert_object_state state;
    //carbon_revise revise;

    //carbon_create_empty(&doc, CARBON_LIST_UNSORTED_MULTISET, CARBON_KEY_NOKEY);
    //carbon_revise_begin(&revise, &rev_doc, manager->doc);
    //carbon_patch_begin(&arr_it, manager->doc);
    //carbon_revise_iterator_open(&arr_it, &revise);
    //carbon_array_it_next(&arrIt);
    //carbon_array_it_field_type(&type, &arrIt);
    //objIt = *carbon_array_it_object_value(&arrIt);
    carbon_array_it_insert_begin(&ins, &arr_it);
    obj_ins = *carbon_insert_object_begin(&state, &ins, 6);

    assert(bench_carbon_insert_int32(manager, 0, &obj_ins, "Test1", 41));
    assert(bench_carbon_insert_int32(manager, 0, &obj_ins, "Test2", 42));
    assert(bench_carbon_insert_int32(manager, 0, &obj_ins, "Test3", 43));
    assert(bench_carbon_insert_int32(manager, 0, &obj_ins, "Test4", 44));
    assert(bench_carbon_insert_int32(manager, 0, &obj_ins, "Test5", 45));
    assert(bench_carbon_insert_int32(manager, 0, &obj_ins, "Test6", 46));

    if(!bench_carbon_find_int32(manager, 0, "Test3", 0))
        bench_carbon_error_write(manager->error, "Failed to find int32 value.", 0);
    if(!bench_carbon_change_val_int32(manager, 0, "Test3", 21))
        bench_carbon_error_write(manager->error, "Failed to change int32 value.", 0);

    if(!bench_carbon_convert_entry_int64(manager, 0, "Test4"))
        return bench_carbon_error_write(manager->error, "Failed to convert to int64 entry.", 0);
    if(!bench_carbon_convert_entry_int32(manager, 0, "Test4"))
        return bench_carbon_error_write(manager->error, "Failed to convert to int32 entry.", 0);
    if(!bench_carbon_delete_int32(manager, 0, "Test4"))
        return bench_carbon_error_write(manager->error, "Failed to delete int32 entry.", 0);


    carbon_insert_object_end(&state);
    //carbon_revise_end(&revise);
    //manager->doc = rev_doc;
    */
    return true;
}
