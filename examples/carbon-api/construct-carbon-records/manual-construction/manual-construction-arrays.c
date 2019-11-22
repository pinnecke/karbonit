// bin/examples-manual-construction-arrays

#include <stdio.h>
#include <jakson/jakson.h>

int main (void)
{
    rec_new context;
    rec record;
    insert *ins, *nested_ins;
    insert_array_state state;
    char *as_json;

    ins = carbon_create_begin(&context, &record, CARBON_KEY_NOKEY, CARBON_KEEP);

    insert_string(ins, "Hello");
    nested_ins = insert_array_begin(&state, ins, 1024);
        insert_string(nested_ins, "you");
        insert_string(nested_ins, "nested") ;
    insert_array_end(&state);
    insert_string(ins, "array!");

    carbon_create_end(&context);

    as_json = carbon_to_json_compact_dup(&record);

    printf ("%s\n", as_json);

    carbon_drop(&record);
    free(as_json);

    return 0;
}