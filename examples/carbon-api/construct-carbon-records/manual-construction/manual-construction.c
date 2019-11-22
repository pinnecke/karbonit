// bin/examples-manual-construction

#include <stdio.h>
#include <jakson/jakson.h>

int main (void)
{
    rec_new context;
    rec record;
    char *as_json;

    carbon_create_begin(&context, &record, KEY_NOKEY, CARBON_KEEP);
    carbon_create_end(&context);

    as_json = carbon_to_json_compact_dup(&record);

    printf ("%s\n", as_json);

    carbon_drop(&record);
    free(as_json);

    return 0;
}