// bin/examples-from-json

#include <stdio.h>
#include <karbonit/karbonit.h>

int main (void)
{
    rec record;
    const char *as_json;

    rec_from_json(&record, "{\"msg\": \"Hello from JSON\"}", KEY_NOKEY, NULL, OPTIMIZE);

    str_buf buffer;
    str_buf_create(&buffer);
    as_json = rec_to_json(&buffer, &record);

    printf ("%s\n", as_json);

    rec_drop(&record);
    str_buf_drop(&buffer);

    return 0;
}