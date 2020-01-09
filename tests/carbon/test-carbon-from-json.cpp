#include <gtest/gtest.h>
#include <printf.h>

#include <jakson/jakson.h>

static void assert_json_out_eq_json_in(const char *json_in, const char *expected)
{
        rec document;
        str_buf str;
        const char *json_out;

        rec_from_json(&document, json_in, KEY_NOKEY, NULL, OPTIMIZE);
        str_buf_create(&str);

        json_out = json_from_record(&str, &document);
        printf("\n"
               "input...: '%s'\n"
               "expected: '%s'\n"
               "is......: '%s\n'",
               json_in, expected, json_out);

        ASSERT_EQ(strcmp(json_out, expected), 0);

        str_buf_drop(&str);
        rec_drop(&document);
}

TEST(FromOtherFormatTest, FromEmptyArrayTest)
{
        assert_json_out_eq_json_in("[]", "[]");
}

TEST(FromOtherFormatTest, FromEmptyObjectTest)
{
        assert_json_out_eq_json_in("{}", "{}");
}

TEST(FromOtherFormatTest, FromEmptyObjectNestedInArrayTest)
{
        assert_json_out_eq_json_in("[{}]", "{}"); /* note, that this is by definition */
}

TEST(FromOtherFormatTest, FromEmptyArrayNestedInObjectTest)
{
        assert_json_out_eq_json_in("{\"_\":[]}", "{\"_\":[]}");
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}