#include <gtest/gtest.h>
#include <printf.h>

#include <jakson/jakson.h>

TEST(TestConverterFormatter, TestFastPrinter) {

        carbon doc;
        struct err err;
        string_buffer buf;
        carbon_from_json(&doc, "[1,2,4,5]", CARBON_KEY_NOKEY, NULL, &err);

        string_buffer_create(&buf);


        timestamp start_2 = wallclock();
        carbon_print_json_from_carbon(&buf, &doc, NULL);
        timestamp end_2 = wallclock();

 timestamp start_1 = wallclock();
        carbon_to_json_compact(&buf, &doc);
        timestamp end_1 = wallclock();

        //const char *out = string_cstr(&buf);
        printf("alt: %llums, neu: %llums \n", (end_1 - start_1), (end_2 - start_2));

        string_buffer_drop(&buf);

        carbon_drop(&doc);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}