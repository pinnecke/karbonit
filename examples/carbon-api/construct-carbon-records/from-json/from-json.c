// bin/examples-from-json

#include <stdio.h>
#include <jakson/jakson.h>

int main (void)
{
    rec record;
    char *as_json;

    rec_from_json(&record, "{\"msg\": \"Hello from JSON\"}", KEY_NOKEY, NULL);

    as_json = rec_to_json_compact_dup(&record);

    printf ("%s\n", as_json);

    rec_drop(&record);
    free(as_json);

    return 0;
}