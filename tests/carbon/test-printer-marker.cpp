#include <gtest/gtest.h>

#include <jakson/jakson.h>


TEST(TestConverterFormatter, TestFastPrinter) {

        rec doc;
        str_buf buf;

        rec_from_json(&doc, "[2, 1, { \"message\": \"hello\", \"data\":[1,2,3,4,5,6]}]", KEY_AUTOKEY, NULL);

        str_buf_create(&buf);

        markers_print(&buf, &doc);
        printf("%s\n", str_buf_cstr(&buf));

        str_buf_drop(&buf);

        rec_drop(&doc);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}