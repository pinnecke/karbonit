#include "bench_carbon.h"
#include "bench_format_handler.h"

bool bench_carbon_error_create(bench_carbon_error *carbonError, bench_error *benchError)
{
    ERROR_IF_NULL(carbonError)
    ERROR_IF_NULL(benchError)

    carbonError->err = benchError;

    return true;
}

bool bench_carbon_error_write(bench_carbon_error *error, char *msg, size_t errOffset)
{
    ERROR_IF_NULL(error)
    ERROR_IF_NULL(msg)
    UNUSED(errOffset)

    if(errOffset)
        strcat(error->err->msg, (const char *) errOffset);

    strcat(error->err->msg, msg);

    return true;
}

bool bench_carbon_mgr_create_from_file(bench_carbon_mgr *manager, bench_carbon_error *error, const char *filePath)
{
    ERROR_IF_NULL(manager)
    ERROR_IF_NULL(error)
    ERROR_IF_NULL(filePath)

    carbon *doc = 0;
    err *err = 0;
    /*
    carbon_find *find;
    carbon_find_create(find)
    */
    FILE *f = fopen(filePath, "rb");
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *jsonContent = MALLOC(fsize + 1);
    size_t nread = fread(jsonContent, fsize, 1, f);
    UNUSED(nread)
    fclose(f);
    jsonContent[fsize] = 0;

    carbon_from_json(doc, jsonContent, CARBON_KEY_NOKEY, NULL, err);

    manager->err = err;
    manager->error = error;
    manager->doc = doc;

    return true;
}

bool bench_carbon_mgr_create_empty(bench_carbon_mgr *manager, bench_carbon_error *error, bench_error *benchError)
{
    ERROR_IF_NULL(manager)
    ERROR_IF_NULL(error)

    //carbon_new context;
    carbon *doc = malloc(sizeof(*doc));
    err *err = malloc(sizeof(*err));

    //carbon_create_begin(&context, doc, CARBON_KEY_NOKEY, CARBON_KEEP);
    carbon_create_empty(doc, CARBON_LIST_UNSORTED_MULTISET, CARBON_KEY_NOKEY);
    bench_carbon_error_create(error, benchError);
    //carbon_create_end(&context);
    error_init(err);

    manager->doc = doc;
    manager->err = err;
    manager->error = error;
    return true;
}

bool bench_carbon_mgr_destroy(bench_carbon_mgr *manager)
{
    ERROR_IF_NULL(manager)
    CHECK_SUCCESS(carbon_drop(manager->doc));
    CHECK_SUCCESS(error_drop(manager->err));

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

    return carbon_insert_prop_i32(ins, key, val);
}

bool bench_carbon_find_int32(bench_carbon_mgr *manager, carbon_object_it *it, char *key, int32_t val)
{
    ERROR_IF_NULL(manager)
    UNUSED(it)
    ERROR_IF_NULL(key)
    UNUSED(val)

    carbon_find find;
    carbon_patch_find_begin(&find, "0.\"Test3\"", manager->doc);
    carbon_field_type_e type;
    carbon_find_result_type(&type, &find);

    if(type != CARBON_FIELD_NUMBER_I32) {
        // TODO : Error handling
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

    if(!bench_carbon_find_int32(manager, 0, "Test3", 0))
        bench_carbon_error_write(manager->error, "Failed to find int32 value.", 0);
    if(!bench_carbon_change_val_int32(manager, 0, "Test3", 21))
        bench_carbon_error_write(manager->error, "Failed to change int32 value.", 0);
/*
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
