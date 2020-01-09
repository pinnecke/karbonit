// bin/examples-read-record-contents

#include <stdio.h>
#include <jakson/jakson.h>

int main (void)
{
        rec record;
        arr_it it;
        //str_buf str;

        rec_from_json(&record, "[\"Hello\", \"Number\", 23]", KEY_NOKEY, NULL, OPTIMIZE);

        rec_read(&it, &record);
        while (arr_it_next(&it)) {
                //arr_it_
        }

        rec_drop(&record);

        return 0;
}