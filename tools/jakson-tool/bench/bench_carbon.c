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
    if(errOffset) {
        error->err->msg = strcat(msg, (const char *) errOffset);
    } else {
        error->err->msg = msg;
    }

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
    manager->doc = doc;

    return true;
}

bool bench_carbon_mgr_create_empty(bench_carbon_mgr *manager, bench_carbon_error *error)
{
    ERROR_IF_NULL(manager)
    ERROR_IF_NULL(error)

    carbon_new context;
    carbon *doc = 0;
    err *err = 0;

    carbon_create_begin(&context, doc, CARBON_KEY_NOKEY, CARBON_KEEP);
    carbon_create_end(&context);
    error_init(err);

    manager->doc = doc;
    manager->err = err;
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