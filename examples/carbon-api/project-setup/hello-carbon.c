// bin/examples-hello-carbon

#include <stdio.h>
#include <karbonit/karbonit.h>

int main (void)
{
    rec_new context;
    rec record;
    insert *ins;
    str_buf buffer;
    const char *as_json;

    ins = rec_create_begin(&context, &record, KEY_NOKEY, KEEP);
    insert_string(ins, "Hello");
    insert_string(ins, "Carbon!");
    rec_create_end(&context);

    str_buf_create(&buffer);
    as_json = rec_to_json(&buffer, &record);

    printf ("%s\n", as_json);

    rec_drop(&record);
    str_buf_clear(&buffer);

    return 0;
}