
#include "bench_bson.h"
#include "bench_format_handler.h"

bool bench_bson_error_create(bench_bson_error *bsonError, bench_error *benchError) {
    ERROR_IF_NULL(bsonError)
    ERROR_IF_NULL(benchError)

    struct err *err = malloc(sizeof(*err));
    error_init(err);

    bsonError->benchErr = benchError;
    bsonError->err = err;

    return true;
}

bool bench_bson_error_destroy(bench_bson_error *error)
{
    ERROR_IF_NULL(error)

    CHECK_SUCCESS(error_drop(error->err));
    free(error);
    return true;
}

// TODO : Use error write function implicitly by handling error messages in-function as additional parameter
bool bench_bson_error_write(bench_bson_error *error, const char *msg, const char *file, u32 line, size_t docOffset) {
    ERROR_IF_NULL(error)
    ERROR_IF_NULL(msg)

    error_set_wdetails(error->err, ERR_FAILED, file, line, msg);

    error->benchErr->msg = strdup(msg);
    error->benchErr->file = strdup(file);
    error->benchErr->line = line;
    error->benchErr->offset = docOffset;

    error_print_to_stderr(error->err);

    return true;
}

bool bench_bson_mgr_create_from_file(bench_bson_mgr *manager, bench_bson_error *bsonError, bench_error *benchError, const char* filePath)
{
    ERROR_IF_NULL(manager);
    ERROR_IF_NULL(bsonError);
    ERROR_IF_NULL(benchError);
    ERROR_IF_NULL(filePath);

    //bson_t b = BSON_INITIALIZER;
    bson_t *b;
    //bson_json_reader_t *jReader;
    bench_bson_error_create(bsonError, benchError);
    bson_error_t bError;
    char msg[ERROR_MSG_SIZE];
/*
    if(!(jReader = bson_json_reader_new_from_file(filePath, &bError))) {
        snprintf(msg, sizeof(msg), "BSON reader failed to open: %s : %s", filePath, bError.message);
        BENCH_BSON_ERROR_WRITE(bsonError, msg, 0);
        return false;
    }

    int readResult;
    while((readResult = bson_json_reader_read (jReader, &b, &bError))) {
        if(readResult < 0) {
            snprintf(msg, sizeof(msg), "Error in JSON parsing: %s", bError.message);
            BENCH_BSON_ERROR_WRITE(bsonError, msg, 0);
            return false;
        }
    }

    size_t errOffset;
    if(!bson_validate(&b, BSON_VALIDATE_NONE, &errOffset)) {
        snprintf(msg, sizeof(msg), "BSON document failed to validate at offset: %s", (char*) errOffset);
        BENCH_BSON_ERROR_WRITE(bsonError, "BSON Document failed to validate.", errOffset);
        return false;
    }
    bson_json_reader_destroy(jReader);
*/
    FILE *f = fopen(filePath, "rb");
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);
    unsigned char *jsonContent = MALLOC(fsize + 1);
    size_t nread = fread(jsonContent, fsize, 1, f);
    UNUSED(nread)
    fclose(f);
    jsonContent[fsize] = 0;

    b = bson_new_from_json(jsonContent, -1, &bError);
    if(!b) {
        snprintf(msg, sizeof(msg), "Failed to create BSON document from JSON.\nError: %s", bError.message);
        BENCH_BSON_ERROR_WRITE(bsonError, msg, 0);
    }

    bson_iter_t *it = malloc(sizeof(bson_iter_t));
    if (!bson_iter_init(it, manager->b)) {
        BENCH_BSON_ERROR_WRITE(bsonError, "Failed to initialize BSON iterator.", 0);
        return false;
    }

    manager->it = it;
    //manager->b = &b;
    manager->b = b;
    manager->error = bsonError;

    return true;
}

bool bench_bson_mgr_create_empty(bench_bson_mgr *manager, bench_bson_error *bsonError, bench_error *benchError)
{
    ERROR_IF_NULL(manager);
    ERROR_IF_NULL(bsonError);
    ERROR_IF_NULL(benchError);

    bson_t *b = bson_new();
    bench_bson_error_create(bsonError, benchError);

    manager->b = b;
    manager->error = bsonError;

    return true;
}

bool bench_bson_mgr_destroy(bench_bson_mgr *manager)
{
    ERROR_IF_NULL(manager)

    bson_destroy(manager->b);
    CHECK_SUCCESS(bench_bson_error_destroy(manager->error));
    return true;
}

bool bench_bson_get_doc(char* str, bench_bson_mgr *manager) {
    ERROR_IF_NULL(manager)
    ERROR_IF_NULL(str)

    size_t errOffset;
    //bson_iter_t it;
    if (!bson_validate(manager->b, BSON_VALIDATE_NONE, &errOffset)) {
        BENCH_BSON_ERROR_WRITE(manager->error, "The document failed to validate at offset: %u\n", errOffset);
        return false;
    }
    /*
    if (bson_iter_init(&it, manager->b)) {
        while(bson_iter_next(&it)) {
            str = strcat(str, bson_iter_key(&it));
        }
    } else {
        BENCH_BSON_ERROR_WRITE(manager->error, "Failed to initialize Iterator. Aborting...", 0);
        return false;
    }
    return str;
    */
    if(!(strcpy(str, bson_as_json(manager->b, 0)))) {
        BENCH_BSON_ERROR_WRITE(manager->error, "Failed to copy BSON document into buffer.", 0);
        return false;
    }

    return true;
}

size_t bench_bson_get_doc_size(bench_bson_mgr *manager)
{
    ERROR_IF_NULL(manager);
    return manager->b->len;
}

bool bench_bson_insert_int32(bench_bson_mgr *manager, bson_iter_t *it, const char *key, int32_t val)
{
    ERROR_IF_NULL(manager);

    if(!it) {
        return bson_append_int32(manager->b, key, strlen(key), val);
    } else {
        // TODO? : Insert at current iterator position
        return false;
    }
}

bool bench_bson_find_int32(bench_bson_mgr *manager, bson_iter_t *it, const char *key, int32_t val) {
    ERROR_IF_NULL(manager);
    UNUSED(val);

    return bson_iter_find(it, key);
}

bool bench_bson_change_val_int32(bench_bson_mgr *manager, bson_iter_t *it, const char *key, int32_t newVal) {
    ERROR_IF_NULL(manager);
    UNUSED(key);

    bson_iter_overwrite_int32(it, newVal);

    return true;
}

bool bench_bson_convert_entry_int32(bench_bson_mgr *manager, bson_iter_t *it, const char *key) {
    ERROR_IF_NULL(manager);
    ERROR_IF_NULL(key);
    UNUSED(it);

    bson_iter_t itStart;
    bson_t *bNew = bson_new();
    /*
    if(bson_iter_find(it, key)) {
        uint8_t *newDoc = malloc(sizeof(manager->b->padding) - sizeof(bson_iter_type(it)));
        memcpy(newDoc, manager->b->padding, bson_iter_offset(it));
    } else {
        return false;
    }
    */
    if (bson_iter_init(&itStart, manager->b)) {
        while (bson_iter_next (&itStart)) {
            if (!strcmp(key, bson_iter_key(&itStart))) {
                if (!bson_append_iter (bNew, NULL, 0, &itStart))
                    return false;
            } else {
                if (!bson_append_int32(bNew, key, strlen(key), (int32_t) bson_iter_value(&itStart)))
                    return false;
                bson_iter_init_from_data_at_offset(it, bson_get_data(bNew), bNew->len,
                                                   bson_iter_offset(&itStart), bson_iter_key_len(&itStart));
            }
        }
    }
    bson_free(manager->b);
    manager->b = bNew;
    return true;
}

bool bench_bson_convert_entry_int64(bench_bson_mgr *manager, bson_iter_t *it, const char *key) {
    ERROR_IF_NULL(manager);
    ERROR_IF_NULL(key);
    UNUSED(it);

    bson_iter_t itStart;
    bson_t *bNew = bson_new();
    if (bson_iter_init(&itStart, manager->b)) {
        while (bson_iter_next (&itStart)) {
            if (strcmp(key, bson_iter_key(&itStart))) {
                if (!bson_append_iter (bNew, NULL, 0, &itStart))
                    return false;
            } else {
                if (!bson_append_int64(bNew, key, strlen(key), (int64_t) bson_iter_value(&itStart)))
                    return false;
                bson_iter_init_from_data_at_offset(it, bson_get_data(bNew), bNew->len,
                                                    bson_iter_offset(&itStart), bson_iter_key_len(&itStart));
            }
        }
    }
    bson_free(manager->b);
    manager->b = bNew;
    return true;
}

bool bench_bson_delete_int32(bench_bson_mgr *manager, bson_iter_t *it, const char *key) {
    ERROR_IF_NULL(manager);
    UNUSED(it);

    bson_t *bNew= bson_new();
    bson_copy_to_excluding_noinit(manager->b, bNew, key, NULL);

    return true;
}

bool bench_bson_execute_benchmark(bench_bson_mgr *manager, const char *benchType) {
    ERROR_IF_NULL(manager);
    UNUSED(benchType);

    assert(bench_bson_insert_int32(manager, 0, "Test1", 41));
    assert(bench_bson_insert_int32(manager, 0, "Test2", 42));
    assert(bench_bson_insert_int32(manager, 0, "Test3", 43));
    assert(bench_bson_insert_int32(manager, 0, "Test4", 44));
    assert(bench_bson_insert_int32(manager, 0, "Test5", 45));
    assert(bench_bson_insert_int32(manager, 0, "Test6", 46));

    bson_iter_t it;
    bson_iter_init(&it, manager->b);
/*
    if(!bench_bson_find_int32(manager, &it, "Test3", 0))
        return BENCH_BSON_ERROR_WRITE(manager->error, "Failed to find int32 value.", 0);

    if(!bench_bson_change_val_int32(manager, &it, 0, 21))
        return BENCH_BSON_ERROR_WRITE(manager->error, "Failed to change int32 value.", 0);

    if(!bench_bson_convert_entry_int64(manager, &it, "Test4"))
        return BENCH_BSON_ERROR_WRITE(manager->error, "Failed to convert to int64 value.", 0);

    if(!bench_bson_convert_entry_int32(manager, &it, "Test4"))
        return bench_bson_error_write(manager->error, "Failed to convert to int32 entry.", 0);

    if(!bench_bson_delete_int32(manager, &it, "Test4"))
        return bench_bson_error_write(manager->error, "Failed to delete int32 entry.", 0);
*/
    return true;
}
