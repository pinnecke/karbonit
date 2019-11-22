// bin/examples-manual-construction-non-empty

#include <stdio.h>
#include <jakson/jakson.h>

int main (void)
{
    rec_new context;
    rec record;
    insert *ins;
    char *as_json;

    ins = carbon_create_begin(&context, &record, KEY_NOKEY, CARBON_KEEP);

    insert_string(ins, "Hello, Carbon!");
    insert_unsigned(ins, 42);
    insert_null(ins);

    carbon_create_end(&context);

    as_json = carbon_to_json_compact_dup(&record);

    printf ("%s\n", as_json);

    carbon_drop(&record);
    free(as_json);

    return 0;
}