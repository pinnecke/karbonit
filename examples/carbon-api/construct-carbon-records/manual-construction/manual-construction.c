// bin/examples-manual-construction

#include <stdio.h>
#include <jakson/jakson.h>

int main (void)
{
    rec_new context;
    rec record;
    str_buf buffer;
    const char *as_json;

    rec_create_begin(&context, &record, KEY_NOKEY, KEEP);
    rec_create_end(&context);

    str_buf_create(&buffer);
    as_json = rec_to_json(&buffer, &record);

    printf ("%s\n", as_json);

    rec_drop(&record);
    str_buf_drop(&buffer);

    return 0;
}