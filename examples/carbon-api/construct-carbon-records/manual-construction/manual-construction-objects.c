// bin/examples-manual-construction-objects

#include <stdio.h>
#include <jakson/jakson.h>

int main (void)
{
    rec_new context;
    rec record;
    insert *ins, *nested_ins, *prop_ins;
    obj_state state, as_prop_state;
    str_buf buffer;
    const char *as_json;

    ins = rec_create_begin(&context, &record, KEY_NOKEY, KEEP);

    nested_ins = insert_object_begin(&state, ins, 1024);
        insert_prop_string(nested_ins, "key", "value");
        prop_ins = insert_prop_object_begin(&as_prop_state, nested_ins, "as prop", 1024);
        insert_prop_string(prop_ins, "hello", "object!");
        insert_object_end(&as_prop_state);

    insert_object_end(&state);

    rec_create_end(&context);

    str_buf_create(&buffer);
    as_json = rec_to_json(&buffer, &record);

    printf ("%s\n", as_json);

    rec_drop(&record);
    str_buf_clear(&buffer);

    return 0;
}