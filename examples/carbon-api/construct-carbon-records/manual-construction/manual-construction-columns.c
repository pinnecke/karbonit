// bin/examples-manual-construction-columns

#include <stdio.h>
#include <jakson/jakson.h>

int main (void)
{
    rec_new context;
    rec record;
    insert *ins, *nested_ins;
    insert_column_state state;
    char *as_json;

    ins = carbon_create_begin(&context, &record, CARBON_KEY_NOKEY, CARBON_KEEP);

    nested_ins = insert_column_begin(&state, ins, COLUMN_U32, 1024);
        insert_u32(nested_ins, 23);
        insert_null(nested_ins);
        insert_u32(nested_ins, 42);
    insert_column_end(&state);

    carbon_create_end(&context);

    as_json = carbon_to_json_compact_dup(&record);

    printf ("%s\n", as_json);

    carbon_drop(&record);
    free(as_json);

    return 0;
}