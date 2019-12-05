// bin/examples-manual-construction-non-empty

#include <stdio.h>
#include <jakson/jakson.h>

int main (void)
{
    rec_new context;
    rec record;
    insert *ins;
    const char *as_json;
    str_buf buffer;

    ins = rec_create_begin(&context, &record, KEY_NOKEY, KEEP);

    insert_string(ins, "Hello, Carbon!");
    insert_unsigned(ins, 42);
    insert_null(ins);

    rec_create_end(&context);

    str_buf_create(&buffer);
    as_json = rec_to_json(&buffer, &record);

    printf ("%s\n", as_json);

    rec_drop(&record);
    str_buf_drop(&buffer);

    return 0;
}