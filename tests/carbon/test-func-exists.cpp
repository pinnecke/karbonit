#include <gtest/gtest.h>
#include <printf.h>

#include <jakson/jakson.h>

static void test_exists(const char *json_in, const char *path_str, bool expected)
{
        rec doc;
        rec_from_json(&doc, json_in, KEY_NOKEY, NULL);

        dot path;
        dot_from_string(&path, path_str);
        ASSERT_EQ(func_exists(&doc, &path), expected);
        dot_drop(&path);

        rec_drop(&doc);
}

TEST(FunExistsTest, EntireDocumentExists)
{
        const char *json_in = "[1, 2, 3]";

        test_exists(json_in, "", true);
}

TEST(FunExistsTest, FindElementInArrayOkay)
{
        const char *json_in = "[1, 2, 3, \"X\", {\"a\": 1, \"b\": 2}]";

        test_exists(json_in, "0", true);
        test_exists(json_in, "1", true);
        test_exists(json_in, "2", true);
        test_exists(json_in, "3", true);
        test_exists(json_in, "4", true);
        test_exists(json_in, "4.a", true);
        test_exists(json_in, "4.b", true);
}

TEST(FunExistsTest, FindElementInObjectOkay)
{
        const char *json_in = "{ \"x\": 1, \"y\": 2, \"z\": {\"a\": 1, \"b\": [1,2,3]}}";

        test_exists(json_in, "x", true);
        test_exists(json_in, "y", true);
        test_exists(json_in, "z", true);
        test_exists(json_in, "z.a", true);
        test_exists(json_in, "z.b", true);
        test_exists(json_in, "z.b.0", true);
        test_exists(json_in, "z.b.1", true);
        test_exists(json_in, "z.b.2", true);
}

TEST(FunExistsTest, FindElementInMixedOkay)
{
        const char *json_in = "[1, 2, 3, \"X\", {\"a\": 1, \"b\": 2}, { \"x\": 1, \"y\": 2, \"z\": {\"a\": 1, \"b\": [1,2,3]}}]";

        test_exists(json_in, "0", true);
        test_exists(json_in, "1", true);
        test_exists(json_in, "2", true);
        test_exists(json_in, "3", true);
        test_exists(json_in, "4", true);
        test_exists(json_in, "4.a", true);
        test_exists(json_in, "4.b", true);
        test_exists(json_in, "5.x", true);
        test_exists(json_in, "5.y", true);
        test_exists(json_in, "5.z", true);
        test_exists(json_in, "5.z.a", true);
        test_exists(json_in, "5.z.b", true);
        test_exists(json_in, "5.z.b.0", true);
        test_exists(json_in, "5.z.b.1", true);
        test_exists(json_in, "5.z.b.2", true);
}

TEST(FunExistsTest, NullNotExistsTest)
{
        const char *json_in = "[1, null, 2]";

        test_exists(json_in, "0", true);
        test_exists(json_in, "1", false);
        test_exists(json_in, "2", true);
}

TEST(FunExistsTest, UndefineddNotExistsTest)
{
        const char *json_in = "[1, 2]";

        test_exists(json_in, "3", false);
        test_exists(json_in, "x", false);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}