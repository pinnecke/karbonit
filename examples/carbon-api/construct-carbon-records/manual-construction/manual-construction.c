// bin/examples-manual-construction

#include <stdio.h>
#include <jakson/jakson.h>

int main (void)
{
    rec_new context;
    rec record;
    char *as_json;

    rec_create_begin(&context, &record, KEY_NOKEY, KEEP);
    rec_create_end(&context);

    as_json = rec_to_json_compact_dup(&record);

    printf ("%s\n", as_json);

    rec_drop(&record);
    free(as_json);

    return 0;
}