// bin/examples-hello-carbon

#include <stdio.h>
#include <jakson/jakson.h>

int main (void)
{
    rec_new context;
    rec record;
    insert *ins;
    char *as_json;

    ins = carbon_create_begin(&context, &record, CARBON_KEY_NOKEY, CARBON_KEEP);
    insert_string(ins, "Hello");
    insert_string(ins, "Carbon!");
    carbon_create_end(&context);

    as_json = carbon_to_json_compact_dup(&record);

    printf ("%s\n", as_json);

    carbon_drop(&record);
    free(as_json);

    return 0;
}