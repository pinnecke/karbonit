// bin/examples-read-record-contents

#include <stdio.h>
#include <jakson/jakson.h>

int main (void)
{
        rec record;
        carbon_array it;
        //string_buffer str;

        carbon_from_json(&record, "[\"Hello\", \"Number\", 23]", CARBON_KEY_NOKEY, NULL);

        carbon_read_begin(&it, &record);
        while (carbon_array_next(&it)) {
                //carbon_array_
        }

        carbon_drop(&record);

        return 0;
}