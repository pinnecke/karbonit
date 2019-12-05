#include <gtest/gtest.h>
#include <printf.h>

#include <jakson/jakson.h>

TEST(TestConverterFormatter, TestFastPrinter) {

        rec doc;
        str_buf buf;

rec_from_json(&doc, "2", KEY_NOKEY, NULL);

        str_buf_create(&buf);


        timestamp start_1 = wallclock();
        rec_to_json_compact(&buf, &doc);
        timestamp end_1 = wallclock();

        timestamp start_2 = wallclock();
        carbon_json_from_carbon(&buf, &doc, NULL);
        timestamp end_2 = wallclock();

        timestamp start_3 = wallclock();
        json_from_record(&buf, &doc);
        timestamp end_3 = wallclock();

        // cmake -DBUILD_TYPE=RELEASE -DUSE_AMALGAMATION=ON .
        // alt: 1005ms, MIT TRAVERS (v1): 1108ms
        printf("%s\n\n\nalt: %llums, MIT TRAVERS: %llums MIT TRAVERS (v2): %llums \n", str_buf_cstr(&buf), (end_1 - start_1), (end_2 - start_2), (end_3 - start_3));

        str_buf_drop(&buf);

        rec_drop(&doc);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}