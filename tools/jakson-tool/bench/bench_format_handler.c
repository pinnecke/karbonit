
#include "bench_format_handler.h"

bool bench_format_handler_create_carbon(bench_format_handler *handler, bench_carbon_mgr *manager,
                                        bench_error *error, const char* filePath)
{
    // TODO: Implement function
    ERROR_IF_NULL(handler);
    ERROR_IF_NULL(manager);

    UNUSED(error);
    UNUSED(filePath);

    return false;
}

bool bench_format_handler_create_bson_handler(bench_format_handler *handler, bench_error *error, const char* filePath)
{
    //JAK_ERROR_IF_NULL(handler);
    UNUSED(error);
    UNUSED(filePath);

    handler->format_name = BENCH_FORMAT_BSON;
    error = malloc(sizeof(*error));
    error->msg = NULL;
    error->code = 0;
    handler->error = error;

    bench_bson_error *bsonError = malloc(sizeof(*bsonError));
    bench_bson_mgr *manager = malloc(sizeof(*manager));
    //if(filePath == NULL) {
        bench_bson_error_create(bsonError, error);
        bench_bson_mgr_create_empty(manager, bsonError);
    //} else {
    //    bench_bson_mgr_create_from_file(&manager, filePath);
    //}
    handler->manager = manager;

    return true;
}

bool bench_format_handler_create_ubjson_handler(bench_format_handler *handler, bench_error *error, const char* filePath)
{
    ERROR_IF_NULL(handler);
    ERROR_IF_NULL(error);
    UNUSED(filePath);

    handler->format_name = BENCH_FORMAT_UBJSON;
    error = malloc(sizeof(*error));
    error->msg = NULL;
    error->code = 0;
    handler->error = error;

    bench_ubjson_error *ubjsonError = malloc(sizeof(*ubjsonError));
    bench_ubjson_mgr *manager = malloc(sizeof(*manager));

    bench_ubjson_error_create(ubjsonError, error);
    bench_ubjson_mgr_create_empty(manager, ubjsonError);

    handler->manager = manager;

    return true;
}

bool bench_format_handler_destroy(bench_format_handler *handler)
{
    if(handler == NULL)
        return false;

    if(strcmp(handler->format_name, BENCH_FORMAT_CARBON) == 0) {
        // TODO: Implement
    } else if(strcmp(handler->format_name, BENCH_FORMAT_BSON) == 0) {
        if(!bench_bson_mgr_destroy((bench_bson_mgr*) handler->manager))
            return false;
    } else if(strcmp(handler->format_name, BENCH_FORMAT_UBJSON) == 0) {
        if(!bench_ubjson_mgr_destroy((bench_ubjson_mgr*) handler->manager))
            return false;
    } else {
        return false;
    }

    free(handler->manager);
    free(handler->error);
    return true;
}

bool bench_format_handler_get_doc(char *str, bench_format_handler *handler) {
    ERROR_IF_NULL(handler);
    if(strcmp(handler->format_name, BENCH_FORMAT_CARBON) == 0) {
        // TODO: Implement
        return false;
    } else if(strcmp(handler->format_name, BENCH_FORMAT_BSON) == 0) {
        return bench_bson_get_doc(str, (bench_bson_mgr*) handler->manager);
    } else if(strcmp(handler->format_name, BENCH_FORMAT_UBJSON) == 0) {
        return bench_ubjson_get_doc(str, handler->manager);
    } else {
        return false;
    }
}

bool bench_format_handler_insert_int32(bench_format_handler *handler, char *key, int32_t val)
{
    if(strcmp(handler->format_name, BENCH_FORMAT_CARBON) == 0) {
        // TODO: Implement
        return false;
    } else if(strcmp(handler->format_name, BENCH_FORMAT_BSON) == 0) {
        return bench_bson_insert_int32((bench_bson_mgr*) handler->manager, 0, key, val);
    } else if(strcmp(handler->format_name, BENCH_FORMAT_UBJSON) == 0) {
        return bench_ubjson_insert_int32((bench_ubjson_mgr*) handler->manager, key, val);
    } else {
        return false;
    }
}

bool bench_format_handler_find_int32(bench_format_handler *handler, char *key, int32_t val)
{
    if(strcmp(handler->format_name, BENCH_FORMAT_CARBON) == 0) {
        // TODO: Implement
        return false;
    } else if(strcmp(handler->format_name, BENCH_FORMAT_BSON) == 0) {
        return bench_bson_find_int32((bench_bson_mgr*) handler->manager, 0, key, val);
    } else if(strcmp(handler->format_name, BENCH_FORMAT_UBJSON) == 0) {
        return bench_ubjson_find_int32((bench_ubjson_mgr*) handler->manager, 0, key, val);
    } else {
        return false;
    }
}

bool bench_format_handler_change_val_int32(bench_format_handler *handler, char *key, int32_t newVal)
{
    if(strcmp(handler->format_name, BENCH_FORMAT_CARBON) == 0) {
        // TODO: Implement
        return false;
    } else if(strcmp(handler->format_name, BENCH_FORMAT_BSON) == 0) {
        return bench_bson_change_val_int32((bench_bson_mgr*) handler->manager, 0, key, newVal);
    } else if(strcmp(handler->format_name, BENCH_FORMAT_UBJSON) == 0) {
        return bench_ubjson_change_val_int32((bench_ubjson_mgr*) handler->manager, 0, key, newVal);
    } else {
        return false;
    }
}

bool bench_format_handler_convert_entry_int32(bench_format_handler *handler, char *key)
{
    if(strcmp(handler->format_name, BENCH_FORMAT_CARBON) == 0) {
        // TODO: Implement
        return false;
    } else if(strcmp(handler->format_name, BENCH_FORMAT_BSON) == 0) {
        return bench_bson_convert_entry_int32((bench_bson_mgr*) handler->manager, 0, key);
    } else if(strcmp(handler->format_name, BENCH_FORMAT_UBJSON) == 0) {
        return bench_ubjson_convert_entry_int32((bench_ubjson_mgr*) handler->manager, 0, key);
    } else {
        return false;
    }
}

bool bench_format_handler_convert_entry_int64(bench_format_handler *handler, char *key)
{
    if(strcmp(handler->format_name, BENCH_FORMAT_CARBON) == 0) {
        // TODO: Implement
        return false;
    } else if(strcmp(handler->format_name, BENCH_FORMAT_BSON) == 0) {
        return bench_bson_convert_entry_int64((bench_bson_mgr*) handler->manager, 0, key);
    } else if(strcmp(handler->format_name, BENCH_FORMAT_UBJSON) == 0) {
        return bench_ubjson_convert_entry_int64((bench_ubjson_mgr*) handler->manager, 0, key);
    } else {
        return false;
    }
}

bool bench_format_handler_delete_int32(bench_format_handler *handler, char *key)
{
    if(strcmp(handler->format_name, BENCH_FORMAT_CARBON) == 0) {
        // TODO: Implement
        return false;
    } else if(strcmp(handler->format_name, BENCH_FORMAT_BSON) == 0) {
        return bench_bson_delete_int32((bench_bson_mgr*) handler->manager, 0, key);
    } else if(strcmp(handler->format_name, BENCH_FORMAT_UBJSON) == 0) {
        return bench_ubjson_delete_int32((bench_ubjson_mgr*) handler->manager, 0, key);
    } else {
        return false;
    }
}

bool bench_format_handler_execute_benchmark(bench_format_handler *handler, const char *benchType)
{
    ERROR_IF_NULL(handler)
    ERROR_IF_NULL(benchType)
    if(strcmp(handler->format_name, BENCH_FORMAT_CARBON) == 0) {
        // TODO: Implement
        return false;
    } else if(strcmp(handler->format_name, BENCH_FORMAT_BSON) == 0) {
        return bench_bson_execute_benchmark((bench_bson_mgr*) handler->manager, benchType);
    } else if(strcmp(handler->format_name, BENCH_FORMAT_UBJSON) == 0) {
        return bench_ubjson_execute_benchmark((bench_ubjson_mgr*) handler->manager, benchType);
    } else {
        return false;
    }
}