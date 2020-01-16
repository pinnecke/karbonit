#include <gtest/gtest.h>
#include <printf.h>

#include <karbonit/karbonit.h>

static void test_get(const char *json_in, const char *path_str, const char *expected)
{
        error_abort_disable();

        rec doc;
        dot path;
        const char *result;
        str_buf buf;

        rec_from_json(&doc, json_in, KEY_NOKEY, NULL, OPTIMIZE);
        dot_from_string(&path, path_str);
        str_buf_create(&buf);

        result = func_rec_get(&doc, &path, &buf);
        printf("\n"
               "json in.: '%s'\n"
               "path in.: '%s'\n"
               "result..: '%s'\n"
               "expected: '%s'\n",
               json_in, path_str, result, expected);
        ASSERT_TRUE(strcmp(result, expected) == 0);

        str_buf_drop(&buf);
        dot_drop(&path);
        rec_drop(&doc);

        error_abort_enable();
}

TEST(FunGetTest, GetFirstElementDocument)
{
        const char *json_in = "[1, 2]";
        test_get(json_in, "0", "1");
}

TEST(FunGetTest, GetSecondElementDocument)
{
        const char *json_in = "[1, 2]";
        test_get(json_in, "1", "2");
}

TEST(FunGetTest, GetThirdElementDocument)
{
        const char *json_in = "[1, 2]";
        test_get(json_in, "3", "undef");
}

TEST(FunGetTest, GetEntireDocument)
{
        const char *json_in = "[1, 2]";
        test_get(json_in, "", "[1, 2]");
}

TEST(FunGetTest, GetNestedElements)
{
        const char *json_in = "[1, {\"x\":1, \"y\":[1, 2, 3, 4], \"z\":{\"a\":null}}, 2]";
        test_get(json_in, "", "[1, {\"x\":1, \"y\":[1, 2, 3, 4], \"z\":{\"a\":null}}, 2]");
        test_get(json_in, "0", "1");
        test_get(json_in, "1", "{\"x\":1, \"y\":[1, 2, 3, 4], \"z\":{\"a\":null}}");
        test_get(json_in, "1.x", "1");
        test_get(json_in, "1.y", "[1, 2, 3, 4]");
        test_get(json_in, "1.y.0", "1");
        test_get(json_in, "1.y.1", "2");
        test_get(json_in, "1.y.2", "3");
        test_get(json_in, "1.y.3", "4");
        test_get(json_in, "1.y.4", "undef");
        test_get(json_in, "1.z", "{\"a\":null}");
        test_get(json_in, "1.z.a", "null");
        test_get(json_in, "1.z.b", "undef");
        test_get(json_in, "2", "2");
        test_get(json_in, "3", "undef");
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}