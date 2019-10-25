
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

    if(!FN_IS_OK(carbon_create_empty(doc, CARBON_LIST_UNSORTED_MULTISET, CARBON_KEY_NOKEY)))
        BENCH_CARBON_ERROR_WRITE(carbonError, "Failed to create CARBON doc", 0);

    manager->doc = doc;
    manager->error = carbonError;

    return true;
}

bool bench_carbon_mgr_destroy(bench_carbon_mgr *manager)
{
    ERROR_IF_NULL(manager)
    CHECK_SUCCESS(carbon_drop(manager->doc));
    CHECK_SUCCESS(bench_carbon_error_destroy(manager->error));

    return true;
}

bool bench_carbon_get_doc(char *str, bench_carbon_mgr *manager)
{
    ERROR_IF_NULL(str)
    ERROR_IF_NULL(manager)

    strcpy(str, carbon_to_json_compact_dup(manager->doc));

    return true;
}

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
    carbon_patch_find_begin(&find, _key, manager->doc);
    carbon_field_type_e type;
    carbon_find_result_type(&type, &find);

    if(type != CARBON_FIELD_NUMBER_I32) {
        BENCH_CARBON_ERROR_WRITE(manager->error, strcat("Could not find int32 key ", _key), 0);
        return false;
    }

    //it = FN_GET_PTR(carbon_object_it, carbon_find_result_object(&find));

    return true;
}

bool bench_carbon_change_val_int32(bench_carbon_mgr *manager, carbon_object_it *it, char *key, int32_t newVal)
{
    ERROR_IF_NULL(manager)
    if(!it) {
        ERROR_IF_NULL(key)
        carbon_find find;
        // Empty dot path searches everywhere?
        carbon_patch_find_begin(&find, key, manager->doc);
        if (FN_IS_OK(carbon_find_has_result(&find))) {
            carbon_field_type_e type;
            carbon_find_result_type(&type, &find);
            if (FN_IS_OK(carbon_find_result_array(&find))) {
                // TODO : if array
            } else if (type == CARBON_FIELD_NUMBER_I32) {
                it = FN_GET_PTR(carbon_object_it, carbon_find_result_object(&find));
            } else {
                // TODO : Error handling
                return false;
            }
        } else {
            carbon_find_end(&find);
            return false;
        }
        carbon_find_end(&find);
    }

    int32_t *val = 0;
    if (!carbon_object_it_i32_value(val, it)) {
        // TODO : Error handling
        return false;
    }
    *val = newVal;

    return true;
}

bool bench_carbon_execute_benchmark(bench_carbon_mgr *manager, const char *benchType)
{
    ERROR_IF_NULL(manager)
    UNUSED(benchType)
    carbon_array_it arr_it;
    //carbon_object_it obj_it;
    //carbon_field_type_e type;
    //carbon *rev_doc;
    carbon_insert ins, obj_ins;
    carbon_insert_object_state state;
    //carbon_revise revise;

    //carbon_create_empty(&doc, CARBON_LIST_UNSORTED_MULTISET, CARBON_KEY_NOKEY);
    //carbon_revise_begin(&revise, &rev_doc, manager->doc);
    carbon_patch_begin(&arr_it, manager->doc);
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
/*
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
*/
    carbon_insert_object_end(&state);
    //carbon_revise_end(&revise);
    //manager->doc = rev_doc;
    return true;
}
