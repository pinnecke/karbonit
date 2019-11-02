
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

bool bench_bson_append_doc(bench_bson_mgr *manager, const char *filePath)
{
    ERROR_IF_NULL(manager);
    ERROR_IF_NULL(filePath);

    bson_t *b;
    bson_error_t bError;
    char msg[ERROR_MSG_SIZE];
    static int doc_count = 0;
    char doc_key[32];

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
        BENCH_BSON_ERROR_WRITE(manager->error, msg, 0);
        return false;
    }

    sprintf(doc_key, "%d", doc_count);
    bson_append_document(manager->b, doc_key, -1, b);
    doc_count++;

    return true;
}

bool bench_bson_convert_doc(size_t *conv_size, bench_bson_mgr *manager, const char *filePath)
{
    ERROR_IF_NULL(conv_size);
    ERROR_IF_NULL(manager);
    ERROR_IF_NULL(filePath);

    bson_t *b;
    bson_error_t bError;
    char msg[ERROR_MSG_SIZE];

    FILE *f = fopen(filePath, "rb");
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);
    unsigned char *jsonContent = MALLOC(fsize + 1);
    size_t nread = fread(jsonContent, fsize, 1, f);
    UNUSED(nread);
    fclose(f);
    jsonContent[fsize] = 0;

    b = bson_new_from_json(jsonContent, -1, &bError);
    if(!b) {
        snprintf(msg, sizeof(msg), "Failed to create BSON document from JSON.\nError: %s", bError.message);
        BENCH_BSON_ERROR_WRITE(manager->error, msg, 0);
        return false;
    }

    *conv_size = b->len;

    bson_destroy(b);

    return true;
}

bool bench_bson_mgr_destroy(bench_bson_mgr *manager)
{
    ERROR_IF_NULL(manager)

    bson_destroy(manager->b);
    CHECK_SUCCESS(bench_bson_error_destroy(manager->error));

    return true;
}

bool bench_bson_to_file(bench_bson_mgr *manager, const char *filePath)
{
    ERROR_IF_NULL(manager);
    ERROR_IF_NULL(filePath);

    size_t errOffset;
    FILE *f = fopen(filePath, "w");
    if (f == NULL)
    {
        printf("Error opening file!\n");
        exit(1);
    }

    if (!bson_validate(manager->b, BSON_VALIDATE_NONE, &errOffset)) {
        BENCH_BSON_ERROR_WRITE(manager->error, "The document failed to validate at offset: %u\n", errOffset);
        return false;
    }

    fprintf(f, "%s", bson_as_canonical_extended_json(manager->b, NULL));

    return true;
}

bool bench_bson_get_doc(char* str, bench_bson_mgr *manager)
{
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

bool bench_bson_insert_int8(bench_bson_mgr *manager, uint32_t numOperations)
{
    for(uint32_t i = 0; i < numOperations; i++) {
        int8_t val = random() % INT8_MAX;
        char key[sizeof(uint32_t) + 1];
        sprintf(key, "%d", i);

        // Using int32 function since BSON internally manages the represented size
        if(!bson_append_int32(manager->b, key, -1, val)) {
            BENCH_BSON_ERROR_WRITE(manager->error, "Failed to insert int8 item at position: %d\nAborting.\n", i);
            return false;
        }
    }

    return true;
}

bool bench_bson_insert_int16(bench_bson_mgr *manager, uint32_t numOperations)
{
    for(uint32_t i = 0; i < numOperations; i++) {
        int16_t val = random() % INT16_MAX;
        char key[sizeof(uint32_t) + 1];
        sprintf(key, "%d", i);

        // Using int32 function since BSON internally manages the represented size
        if(!bson_append_int32(manager->b, key, -1, val)) {
            BENCH_BSON_ERROR_WRITE(manager->error, "Failed to insert int16 item at position: %d\nAborting.\n", i);
            return false;
        }
    }

    return true;
}

bool bench_bson_insert_int32(bench_bson_mgr *manager, uint32_t numOperations)
{
    for(uint32_t i = 0; i < numOperations; i++) {
        int32_t val = random() % INT32_MAX;
        char key[sizeof(uint32_t) + 1];
        sprintf(key, "%d", i);

        if(!bson_append_int32(manager->b, key, -1, val)) {
            BENCH_BSON_ERROR_WRITE(manager->error, "Failed to insert int32 item at position: %d\nAborting.\n", i);
            return false;
        }
        manager->numEntries++;
    }

    return true;
}

bool bench_bson_insert_int64(bench_bson_mgr *manager, uint32_t numOperations)
{
    for(uint32_t i = 0; i < numOperations; i++) {
        int64_t val = random() % INT64_MAX;
        char key[sizeof(uint32_t) + 1];
        sprintf(key, "%d", i);

        if(!bson_append_int64(manager->b, key, -1, val)) {
            BENCH_BSON_ERROR_WRITE(manager->error, "Failed to insert int64 item.", i);
            return false;
        }
    }

    return true;
}

bool bench_bson_read(bench_bson_mgr *manager, uint32_t numOperations)
{
    bson_iter_t it;
    char msg[ERROR_MSG_SIZE];
    for(uint32_t i = 0; i < numOperations; i++) {
        bson_iter_init(&it, manager->b);
        char key[sizeof(uint32_t) + 1];
        sprintf(key, "%d", (uint32_t)(random() % numOperations));

        if(!bson_iter_find(&it, key)) {
            sprintf(msg, "Failed to read item:%s\nAborting.\n", key);
            BENCH_BSON_ERROR_WRITE(manager->error, msg , 0);
            return false;
        }
    }
    return true;
}

bool bench_bson_update_int8(bench_bson_mgr *manager, uint32_t numOperations)
{
    bson_iter_t it;

    for(uint32_t i = 0; i < numOperations; i++) {
        bson_iter_init(&it, manager->b);
        int8_t newVal = random() % INT8_MAX;
        char key[sizeof(uint32_t) + 1];
        sprintf(key, "%d", (uint32_t)(random() % numOperations));

        if(!bson_iter_find(&it, key)) {
            BENCH_BSON_ERROR_WRITE(manager->error, "Failed to update int8 item: %d\nAborting.\n", atoi(key));
            return false;
        }
        bson_iter_overwrite_int32(&it, newVal);
    }
    return true;
}

bool bench_bson_update_int16(bench_bson_mgr *manager, uint32_t numOperations)
{
    bson_iter_t it;

    for(uint32_t i = 0; i < numOperations; i++) {
        bson_iter_init(&it, manager->b);
        int16_t newVal = random() % INT16_MAX;
        char key[sizeof(uint32_t) + 1];

        sprintf(key, "%d", (uint32_t)(random() % numOperations));
        if(!bson_iter_find(&it, key)) {
            BENCH_BSON_ERROR_WRITE(manager->error, "Failed to update int16 item: %d\nAborting.\n", atoi(key));
            return false;
        }
        bson_iter_overwrite_int32(&it, newVal);
    }
    return true;
}

bool bench_bson_update_int32(bench_bson_mgr *manager, uint32_t numOperations)
{
    bson_iter_t it;

    for(uint32_t i = 0; i < numOperations; i++) {
        bson_iter_init(&it, manager->b);
        int32_t newVal = random() % INT32_MAX;
        char key[sizeof(uint32_t) + 1];
        sprintf(key, "%d", (uint32_t)(random() % numOperations));

        if(!bson_iter_find(&it, key)) {
            BENCH_BSON_ERROR_WRITE(manager->error, "Failed to update int32 item: %d\nAborting.\n", atoi(key));
            return false;
        }
        bson_iter_overwrite_int32(&it, newVal);
    }
    return true;
}

bool bench_bson_update_int64(bench_bson_mgr *manager, uint32_t numOperations)
{
    bson_iter_t it;

    for(uint32_t i = 0; i < numOperations; i++) {
        bson_iter_init(&it, manager->b);
        int64_t newVal = random() % INT64_MAX;
        char key[sizeof(uint32_t) + 1];
        sprintf(key, "%d", (uint32_t)(random() % numOperations));

        if(!bson_iter_find(&it, key)) {
            BENCH_BSON_ERROR_WRITE(manager->error, "Failed to update int64 item: %d", atoi(key));
            return false;
        }
        bson_iter_overwrite_int64(&it, newVal);
    }
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

bool bench_bson_delete(bench_bson_mgr *manager, uint32_t numOperations) {
    char removedKeys[numOperations][sizeof(uint32_t) + 1];

    for(uint32_t i = 0; i < numOperations; i++) {
        bson_t *bNew= bson_new();
        char key[sizeof(uint32_t) + 1];
        generate_key:
        sprintf(key, "%d", (uint32_t)(random() % manager->numEntries));

        // Check if key has already been removed
        for(uint32_t j = 0; j < i; j++) {
            if(strcmp(key, removedKeys[j]) == 0)
                goto generate_key;
        }

        bson_copy_to_excluding_noinit(manager->b, bNew, key, NULL);
        strcat(removedKeys[i], key);
        bson_destroy(manager->b);
        manager->b = bNew;
    }

    return true;
}

bool bench_bson_execute_benchmark_operation_int8(bench_bson_mgr *manager, bench_operation_type opType, uint32_t numOperations)
{
    if(opType == BENCH_OP_TYPE_INSERT) {
        return bench_bson_insert_int8(manager, numOperations);
    } else if(opType == BENCH_OP_TYPE_READ) {
        return bench_bson_read(manager, numOperations);
    } else if(opType == BENCH_OP_TYPE_UPDATE) {
        return bench_bson_update_int8(manager, numOperations);
    } else if(opType == BENCH_OP_TYPE_DELETE) {
        return bench_bson_delete(manager, numOperations);
    } else {
        return false;
    }
}

bool bench_bson_execute_benchmark_operation_int16(bench_bson_mgr *manager, bench_operation_type opType, uint32_t numOperations)
{
    if(opType == BENCH_OP_TYPE_INSERT) {
        return bench_bson_insert_int16(manager, numOperations);
    } else if(opType == BENCH_OP_TYPE_READ) {
        return bench_bson_read(manager, numOperations);
    } else if(opType == BENCH_OP_TYPE_UPDATE) {
        return bench_bson_update_int16(manager, numOperations);
    } else if(opType == BENCH_OP_TYPE_DELETE) {
        return bench_bson_delete(manager, numOperations);
    } else {
        return false;
    }
}

bool bench_bson_execute_benchmark_operation_int32(bench_bson_mgr *manager, bench_operation_type opType, uint32_t numOperations)
{
    if(opType == BENCH_OP_TYPE_INSERT) {
        return bench_bson_insert_int32(manager, numOperations);
    } else if(opType == BENCH_OP_TYPE_READ) {
        return bench_bson_read(manager, numOperations);
    } else if(opType == BENCH_OP_TYPE_UPDATE) {
        return bench_bson_update_int32(manager, numOperations);
    } else if(opType == BENCH_OP_TYPE_DELETE) {
        return bench_bson_delete(manager, numOperations);
    } else {
        return false;
    }
}

bool bench_bson_execute_benchmark_operation_int64(bench_bson_mgr *manager, bench_operation_type opType, uint32_t numOperations)
{
    if(opType == BENCH_OP_TYPE_INSERT) {
        return bench_bson_insert_int64(manager, numOperations);
    } else if(opType == BENCH_OP_TYPE_READ) {
        return bench_bson_read(manager, numOperations);
    } else if(opType == BENCH_OP_TYPE_UPDATE) {
        return bench_bson_update_int64(manager, numOperations);
    } else if(opType == BENCH_OP_TYPE_DELETE) {
        return bench_bson_delete(manager, numOperations);
    } else {
        return false;
    }
}

bool bench_bson_execute_benchmark_operation(bench_bson_mgr *manager, bench_type type,
        bench_operation_type opType, uint32_t numOperations)
{
    ERROR_IF_NULL(manager);
    ERROR_IF_NULL(type);
    ERROR_IF_NULL(opType);

    if(type == BENCH_TYPE_INT8) {
        return bench_bson_execute_benchmark_operation_int8(manager, opType, numOperations);
    } else if(type == BENCH_TYPE_INT16) {
        return bench_bson_execute_benchmark_operation_int16(manager, opType, numOperations);
    } else if(type == BENCH_TYPE_INT32) {
        return bench_bson_execute_benchmark_operation_int32(manager, opType, numOperations);
    } else if(type == BENCH_TYPE_INT64) {
        return bench_bson_execute_benchmark_operation_int64(manager, opType, numOperations);
    } else {
        return false;
    }
}
