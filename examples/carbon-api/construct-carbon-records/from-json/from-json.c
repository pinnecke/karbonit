// bin/examples-from-json

#include <stdio.h>
#include <jakson/jakson.h>

int main (void)
{
    rec record;
    char *as_json;

    carbon_from_json(&record, "{\"msg\": \"Hello from JSON\"}", KEY_NOKEY, NULL);

    as_json = carbon_to_json_compact_dup(&record);

    printf ("%s\n", as_json);

    carbon_drop(&record);
    free(as_json);

    return 0;
}