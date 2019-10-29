
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

bool bench_format_handler_write_error(bench_error *error, const char *msg)
{
    ERROR_IF_NULL(error)
    ERROR_IF_NULL(msg)

    error->msg = strdup(msg);

    return true;
}

bool bench_format_handler_create_handler(bench_format_handler *handler, bench_error *error, const char *filePath, const char *formatType)
{
    ERROR_IF_NULL(handler)
    ERROR_IF_NULL(error)
    ERROR_IF_NULL(formatType)

    CHECK_SUCCESS(bench_format_handler_create_error(error));

    if(strcmp(formatType, BENCH_FORMAT_CARBON) == 0) {
        return bench_format_handler_create_carbon_handler(handler, error, filePath);
    } else if(strcmp(formatType, BENCH_FORMAT_BSON) == 0) {
        return bench_format_handler_create_bson_handler(handler, error, filePath);
    } else if(strcmp(formatType, BENCH_FORMAT_UBJSON) == 0) {
        return bench_format_handler_create_ubjson_handler(handler, error, filePath);
    } else {
        bench_format_handler_write_error(error, strcat("Wrong format type: ", formatType));
        return false;
    }
}

bool bench_format_handler_create_carbon_handler(bench_format_handler *handler, bench_error *error, const char* filePath)
{
    ERROR_IF_NULL(handler)
    ERROR_IF_NULL(error)

    bench_carbon_error *carbonError = malloc(sizeof(*carbonError));
    bench_carbon_mgr *manager = malloc(sizeof(*manager));

    if(filePath == NULL) {
        bench_carbon_mgr_create_empty(manager, carbonError, error);
    } else {
        bench_carbon_mgr_create_from_file(manager, carbonError, error, filePath);
    }

    handler->manager = manager;
    handler->format_name = BENCH_FORMAT_CARBON;
    handler->error = error;

    return true;
}

bool bench_format_handler_create_bson_handler(bench_format_handler *handler, bench_error *error, const char* filePath)
{
    ERROR_IF_NULL(handler)
    ERROR_IF_NULL(error)

    bench_bson_error *bsonError = malloc(sizeof(*bsonError));
    bench_bson_mgr *manager = malloc(sizeof(*manager));

    if(filePath == NULL) {
        bench_bson_mgr_create_empty(manager, bsonError, error);
    } else {
        bench_bson_mgr_create_from_file(manager, bsonError, error, filePath);
    }

    handler->manager = manager;
    handler->error = error;
    handler->format_name = BENCH_FORMAT_BSON;

    return true;
}

bool bench_format_handler_create_ubjson_handler(bench_format_handler *handler, bench_error *error, const char* filePath)
{
    ERROR_IF_NULL(handler)
    ERROR_IF_NULL(error)

    bench_ubjson_error *ubjsonError = malloc(sizeof(*ubjsonError));
    bench_ubjson_mgr *manager = malloc(sizeof(*manager));

    if(filePath == NULL) {
        bench_ubjson_mgr_create_empty(manager, ubjsonError, error);
    } else {
        bench_ubjson_mgr_create_from_file(manager, ubjsonError, error, filePath);
    }

    handler->manager = manager;
    handler->format_name = BENCH_FORMAT_UBJSON;
    handler->error = error;

    return true;
}

bool bench_format_handler_append_doc(bench_format_handler *handler, const char *filePath)
{
    ERROR_IF_NULL(handler);
    ERROR_IF_NULL(filePath);

    if(strcmp(handler->format_name, BENCH_FORMAT_CARBON) == 0) {
        return bench_carbon_append_doc((bench_carbon_mgr*) handler->manager, filePath);
    } else if(strcmp(handler->format_name, BENCH_FORMAT_BSON) == 0) {
        return bench_bson_append_doc((bench_bson_mgr*) handler->manager, filePath);
    } else if(strcmp(handler->format_name, BENCH_FORMAT_UBJSON) == 0) {
        return bench_ubjson_append_doc((bench_ubjson_mgr*) handler->manager, filePath);
    }

    return false;
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

    //free(handler->manager);
    //free(handler->error);
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

size_t bench_format_handler_get_doc_size(bench_format_handler *handler)
{
    ERROR_IF_NULL(handler)

    if(strcmp(handler->format_name, BENCH_FORMAT_CARBON) == 0) {
        //return bench_carbon_get_doc_size(str, (bench_carbon_mgr*) handler->manager);
    } else if(strcmp(handler->format_name, BENCH_FORMAT_BSON) == 0) {
        return bench_bson_get_doc_size((bench_bson_mgr*) handler->manager);
    } else if(strcmp(handler->format_name, BENCH_FORMAT_UBJSON) == 0) {
        return bench_ubjson_get_doc_size((bench_ubjson_mgr*) handler->manager);
    }

    return false;
}
bool bench_format_handler_get_process_status(char *buffer)
{
    FILE* status = fopen( "/proc/self/status", "r" );
    char c;

    while((c = fgetc(status)) != EOF)
        strncat(buffer, &c, 1);

    fclose(status);

    return true;
}

bool bench_format_handler_get_file_content(unsigned char *json_content, FILE *file, long file_size) {
    ERROR_IF_NULL(json_content);
    ERROR_IF_NULL(file);
    if(!file_size) {
        fseek(file, 0, SEEK_END);
        file_size = ftell(file);
    }

    fread(json_content, file_size, 1, file);
    json_content[file_size] = 0;

    fclose(file);

    return true;
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