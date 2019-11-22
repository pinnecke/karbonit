// bin/examples-read-record-contents

#include <stdio.h>
#include <jakson/jakson.h>

int main (void)
{
        rec record;
        arr_it it;
        //str_buf str;

        carbon_from_json(&record, "[\"Hello\", \"Number\", 23]", CARBON_KEY_NOKEY, NULL);

        carbon_read_begin(&it, &record);
        while (arr_it_next(&it)) {
                //arr_it_
        }

        carbon_drop(&record);

        return 0;
}