// bin/examples-read-record-contents

#include <stdio.h>
#include <carbon.h>

int main (void)
{
        carbon record;
        err err;
        struct carbon_array it;
        //struct carbon_strbuf str;

        carbon_from_json(&record, "[\"Hello\", \"Number\", 23]", CARBON_KEY_NOKEY, NULL, &err);

        carbon_read_begin(&it, &record);
        while (carbon_array_next(&it)) {
                //carbon_array_it_
        }

        carbon_drop(&record);

        return 0;
}