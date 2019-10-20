
#include "bench_format_handler.h"

bool bench_format_handler_create_error(bench_error *error)
{
    ERROR_IF_NULL(error)

    error->offset = 0;
    error->line = 0;
    error->file = NULL;
    error->msg = NULL;

    return true;
}

bool bench_format_handler_create_carbon_handler(bench_format_handler *handler, bench_error *error, const char* filePath)
{
    ERROR_IF_NULL(handler)
    ERROR_IF_NULL(error)
    UNUSED(filePath)

    bench_format_handler_create_error(error);

    bench_carbon_error *carbonError = malloc(sizeof(*carbonError));
    bench_carbon_mgr *manager = malloc(sizeof(*manager));

    /*if(filePath) {
        bench_carbon_mgr_create_from_file(manager, carbonError, filePath);
    } else {*/
        bench_carbon_mgr_create_empty(manager, carbonError, error);
    //}

    handler->manager = manager;
    handler->format_name = BENCH_FORMAT_CARBON;
    handler->error = error;

    return true;
}

bool bench_format_handler_create_bson_handler(bench_format_handler *handler, bench_error *error, const char* filePath)
{
    ERROR_IF_NULL(handler)
    ERROR_IF_NULL(error)
    UNUSED(filePath)

    //error = malloc(sizeof(*error));
    bench_bson_error *bsonError = malloc(sizeof(*bsonError));
    bench_bson_mgr *manager = malloc(sizeof(*manager));

    bench_format_handler_create_error(error);

    //if(filePath == NULL) {

        bench_bson_mgr_create_empty(manager, bsonError, error);
    //} else {
    //    bench_bson_mgr_create_from_file(&manager, filePath);
    //}
    handler->manager = manager;
    handler->error = error;
    handler->format_name = BENCH_FORMAT_BSON;

    return true;
}

bool bench_format_handler_create_ubjson_handler(bench_format_handler *handler, bench_error *error, const char* filePath)
{
    ERROR_IF_NULL(handler)
    ERROR_IF_NULL(error)
    UNUSED(filePath)

    bench_ubjson_error *ubjsonError = malloc(sizeof(*ubjsonError));
    bench_ubjson_mgr *manager = malloc(sizeof(*manager));

    bench_format_handler_create_error(error);
    /*if(filePath == NULL) {
        bench_ubjson_mgr_create_from_file(manager, ubjsonError, filePath);
    } else {*/
        bench_ubjson_mgr_create_empty(manager, ubjsonError, error);
    //}
    handler->manager = manager;
    handler->format_name = BENCH_FORMAT_UBJSON;
    handler->error = error;

    return true;
}

bool bench_format_handler_destroy(bench_format_handler *handler)
{
    if(handler == NULL)
        return false;

    if(strcmp(handler->format_name, BENCH_FORMAT_CARBON) == 0) {
        if(!bench_carbon_mgr_destroy((bench_carbon_mgr*) handler->manager))
            return false;
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
    ERROR_IF_NULL(handler)
    if(strcmp(handler->format_name, BENCH_FORMAT_CARBON) == 0) {
        return bench_carbon_get_doc(str, (bench_carbon_mgr*) handler->manager);
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
        return bench_carbon_insert_int32((bench_carbon_mgr*) handler->manager, 0, 0, key, val);
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
        return bench_carbon_execute_benchmark((bench_carbon_mgr*) handler->manager, benchType);
    } else if(strcmp(handler->format_name, BENCH_FORMAT_BSON) == 0) {
        return bench_bson_execute_benchmark((bench_bson_mgr*) handler->manager, benchType);
    } else if(strcmp(handler->format_name, BENCH_FORMAT_UBJSON) == 0) {
        return bench_ubjson_execute_benchmark((bench_ubjson_mgr*) handler->manager, benchType);
    } else {
        return false;
    }
}