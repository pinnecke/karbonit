// bin/examples-manual-construction-columns

#include <stdio.h>
#include <karbonit/karbonit.h>

int main (void)
{
    rec_new context;
    rec record;
    insert *ins, *nested_ins;
    col_state state;
    const char *as_json;
    str_buf buffer;

    ins = rec_create_begin(&context, &record, KEY_NOKEY, KEEP);

    nested_ins = insert_column_begin(&state, ins, COLUMN_U32, 1024);
        insert_u32(nested_ins, 23);
        insert_null(nested_ins);
        insert_u32(nested_ins, 42);
    insert_column_end(&state);

    rec_create_end(&context);

    str_buf_create(&buffer);
    as_json = rec_to_json(&buffer, &record);

    printf ("%s\n", as_json);

    rec_drop(&record);
    str_buf_drop(&buffer);

    return 0;
}