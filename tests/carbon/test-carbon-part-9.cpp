#include <gtest/gtest.h>
#include <printf.h>

#include <jakson/jakson.h>


TEST(CarbonTest, CarbonFromJsonShortenedDotPath)
{
        rec doc;
        find find;
        field_e result_type;

        const char *json_in = "{\"x\":\"y\"}";
        rec_from_json(&doc, json_in, KEY_NOKEY, NULL);

        /* without shortened dot path rule, the json object as given is embedded in an record container (aka array)
         * such that the object must be referenced by its index in the record container (i.e., 0) */
        find_begin_from_string(&find, "0.x", &doc);
        ASSERT_TRUE(find_has_result(&find));
        find_result_type(&result_type, &find);
        ASSERT_EQ(result_type, FIELD_STRING);

        /* with shortened dot path rule, the json object can be referenced without providing its index in the record */
        find_begin_from_string(&find, "x", &doc);
        ASSERT_TRUE(find_has_result(&find));
        find_result_type(&result_type, &find);
        ASSERT_EQ(result_type, FIELD_STRING);

        rec_drop(&doc);

        json_in = "[{\"x\":\"y\"},{\"x\":[{\"z\":42}]}]";
        rec_from_json(&doc, json_in, KEY_NOKEY, NULL);

        /* The shortened dot path rule does not apply here since the user input is an array  */
        find_begin_from_string(&find, "0.x", &doc);
        ASSERT_TRUE(find_has_result(&find));
        find_result_type(&result_type, &find);
        ASSERT_EQ(result_type, FIELD_STRING);

        find_begin_from_string(&find, "1.x", &doc);
        ASSERT_TRUE(find_has_result(&find));
        find_result_type(&result_type, &find);
        ASSERT_EQ(result_type, FIELD_ARRAY_UNSORTED_MULTISET);

        find_begin_from_string(&find, "x", &doc);
        ASSERT_FALSE(find_has_result(&find));

        /* The shortened dot path rule does also never apply outside the record container  */
        find_begin_from_string(&find, "1.x.0.z", &doc);
        ASSERT_TRUE(find_has_result(&find));

        find_begin_from_string(&find, "1.x.z", &doc);
        ASSERT_FALSE(find_has_result(&find));

        rec_drop(&doc);
}

TEST(CarbonTest, CarbonFindPrint)
{
        rec doc;
        find find;
        const char *result;

        str_buf sb1;
        str_buf_create(&sb1);

        rec_from_json(&doc, "8", KEY_NOKEY, NULL);
        ASSERT_TRUE(find_begin_from_string(&find, "0", &doc));
        result = find_result_to_str(&sb1, &find);
        ASSERT_TRUE(strcmp(result, "8") == 0);
        rec_drop(&doc);

        rec_from_json(&doc, "-8", KEY_NOKEY, NULL);
        ASSERT_TRUE(find_begin_from_string(&find, "0", &doc));
        result = find_result_to_str(&sb1, &find);
        ASSERT_TRUE(strcmp(result, "-8") == 0);
        rec_drop(&doc);

        rec_from_json(&doc, "\"A\"", KEY_NOKEY, NULL);
        ASSERT_TRUE(find_begin_from_string(&find, "0", &doc));
        result = find_result_to_str(&sb1, &find);
        ASSERT_TRUE(strcmp(result, "\"A\"") == 0);
        rec_drop(&doc);

        rec_from_json(&doc, "32.4", KEY_NOKEY, NULL);
        ASSERT_TRUE(find_begin_from_string(&find, "0", &doc));
        result = find_result_to_str(&sb1, &find);
        ASSERT_TRUE(strcmp(result, "32.40") == 0);
        rec_drop(&doc);

        rec_from_json(&doc, "null", KEY_NOKEY, NULL);
        ASSERT_TRUE(find_begin_from_string(&find, "0", &doc));
        result = find_result_to_str(&sb1, &find);
        ASSERT_TRUE(strcmp(result, "null") == 0);
        rec_drop(&doc);

        rec_from_json(&doc, "true", KEY_NOKEY, NULL);
        rec_hexdump_print(stderr, &doc);
        ASSERT_TRUE(find_begin_from_string(&find, "0", &doc));
        result = find_result_to_str(&sb1, &find);
        ASSERT_TRUE(strcmp(result, "true") == 0);
        rec_drop(&doc);

        rec_from_json(&doc, "false", KEY_NOKEY, NULL);
        ASSERT_TRUE(find_begin_from_string(&find, "0", &doc));
        result = find_result_to_str(&sb1, &find);
        ASSERT_TRUE(strcmp(result, "false") == 0);
        rec_drop(&doc);

        rec_from_json(&doc, "[1, 2, 3, null]", KEY_NOKEY, NULL);
        ASSERT_TRUE(find_begin_from_string(&find, "0", &doc));
        result = find_result_to_str(&sb1, &find);
        ASSERT_TRUE(strcmp(result, "1") == 0);
        rec_drop(&doc);

        rec_from_json(&doc, "[1, 2, 3, null]", KEY_NOKEY, NULL);
        ASSERT_TRUE(find_begin_from_string(&find, "1", &doc));
        result = find_result_to_str(&sb1, &find);
        ASSERT_TRUE(strcmp(result, "2") == 0);
        rec_drop(&doc);

        rec_from_json(&doc, "[1, 2, 3, null]", KEY_NOKEY, NULL);
        ASSERT_TRUE(find_begin_from_string(&find, "2", &doc));
        result = find_result_to_str(&sb1, &find);
        ASSERT_TRUE(strcmp(result, "3") == 0);
        rec_drop(&doc);

        rec_from_json(&doc, "[1, 2, 3, null]", KEY_NOKEY, NULL);
        ASSERT_TRUE(find_begin_from_string(&find, "3", &doc));
        result = find_result_to_str(&sb1, &find);
        ASSERT_TRUE(strcmp(result, "null") == 0);
        rec_drop(&doc);

        rec_from_json(&doc, "[1, 2, 3, null]", KEY_NOKEY, NULL);
        ASSERT_FALSE(find_begin_from_string(&find, "4", &doc));
        result = find_result_to_str(&sb1, &find);
        ASSERT_TRUE(strcmp(result, "undef") == 0);
        rec_drop(&doc);

        rec_from_json(&doc, "[\"A\", \"B\", \"C\", null]", KEY_NOKEY, NULL);
        ASSERT_TRUE(find_begin_from_string(&find, "0", &doc));
        result = find_result_to_str(&sb1, &find);
        ASSERT_TRUE(strcmp(result, "\"A\"") == 0);
        rec_drop(&doc);

        rec_from_json(&doc, "[\"A\", \"B\", \"C\", null]", KEY_NOKEY, NULL);
        ASSERT_TRUE(find_begin_from_string(&find, "1", &doc));
        result = find_result_to_str(&sb1, &find);
        ASSERT_TRUE(strcmp(result, "\"B\"") == 0);
        rec_drop(&doc);

        rec_from_json(&doc, "[\"A\", \"B\", \"C\", null]", KEY_NOKEY, NULL);
        ASSERT_TRUE(find_begin_from_string(&find, "2", &doc));
        result = find_result_to_str(&sb1, &find);
        ASSERT_TRUE(strcmp(result, "\"C\"") == 0);
        rec_drop(&doc);

        rec_from_json(&doc, "[\"A\", \"B\", \"C\", null]", KEY_NOKEY, NULL);
        ASSERT_TRUE(find_begin_from_string(&find, "3", &doc));
        result = find_result_to_str(&sb1, &find);
        ASSERT_TRUE(strcmp(result, "null") == 0);
        rec_drop(&doc);

        rec_from_json(&doc, "\"Hello, World!\"", KEY_NOKEY, NULL);
        ASSERT_TRUE(find_begin_from_string(&find, "0", &doc));
        result = find_result_to_str(&sb1, &find);
        ASSERT_TRUE(strcmp(result, "\"Hello, World!\"") == 0);
        rec_drop(&doc);

        rec_from_json(&doc, "{}", KEY_NOKEY, NULL);
        ASSERT_TRUE(find_begin_from_string(&find, "0", &doc));
        result = find_result_to_str(&sb1, &find);
        ASSERT_TRUE(strcmp(result, "{}") == 0);
        rec_drop(&doc);

        rec_from_json(&doc, "[]", KEY_NOKEY, NULL);
        ASSERT_FALSE(find_begin_from_string(&find, "0", &doc));
        result = find_result_to_str(&sb1, &find);
        ASSERT_TRUE(strcmp(result, "undef") == 0);
        rec_drop(&doc);

        const char *complex = "{\n"
                              "   \"m\":{\n"
                              "         \"n\":8,\n"
                              "         \"o\":-8,\n"
                              "         \"p\":\"A\",\n"
                              "         \"q\":32.4,\n"
                              "         \"r\":null,\n"
                              "         \"s\":true,\n"
                              "         \"t\":false,\n"
                              "         \"u\":[\n"
                              "            1,\n"
                              "            2,\n"
                              "            3,\n"
                              "            null\n"
                              "         ],\n"
                              "         \"v\":[\n"
                              "            \"A\",\n"
                              "            \"B\",\n"
                              "            null\n"
                              "         ],\n"
                              "         \"w\":\"Hello, World!\",\n"
                              "         \"x\":{\n"
                              "            \"a\":null\n"
                              "         },\n"
                              "         \"y\":[\n"
                              "\n"
                              "         ],\n"
                              "         \"z\":{\n"
                              "\n"
                              "         }\n"
                              "      }\n"
                              "}";

        rec_from_json(&doc, complex, KEY_NOKEY, NULL);

        ASSERT_TRUE(find_begin_from_string(&find, "m", &doc));
        result = find_result_to_str(&sb1, &find);

        ASSERT_TRUE(strcmp(result, "{\"n\":8, \"o\":-8, \"p\":\"A\", \"q\":32.40, \"r\":null, \"s\":true, \"t\":false, \"u\":[1, 2, 3, null], \"v\":[\"A\", \"B\", null], \"w\":\"Hello, World!\", \"x\":{\"a\":null}, \"y\":[], \"z\":{}}") == 0);

        ASSERT_TRUE(find_begin_from_string(&find, "m.n", &doc));
        result = find_result_to_str(&sb1, &find);
        ASSERT_TRUE(strcmp(result, "8") == 0);

        ASSERT_TRUE(find_begin_from_string(&find, "m.o", &doc));
        result = find_result_to_str(&sb1, &find);
        ASSERT_TRUE(strcmp(result, "-8") == 0);

        ASSERT_TRUE(find_begin_from_string(&find, "m.p", &doc));
        result = find_result_to_str(&sb1, &find);
        ASSERT_TRUE(strcmp(result, "\"A\"") == 0);

        ASSERT_TRUE(find_begin_from_string(&find, "m.q", &doc));
        result = find_result_to_str(&sb1, &find);
        ASSERT_TRUE(strcmp(result, "32.40") == 0);

        ASSERT_TRUE(find_begin_from_string(&find, "m.r", &doc));
        result = find_result_to_str(&sb1, &find);
        ASSERT_TRUE(strcmp(result, "null") == 0);

        ASSERT_TRUE(find_begin_from_string(&find, "m.s", &doc));
        result = find_result_to_str(&sb1, &find);
        ASSERT_TRUE(strcmp(result, "true") == 0);

        ASSERT_TRUE(find_begin_from_string(&find, "m.t", &doc));
        result = find_result_to_str(&sb1, &find);
        ASSERT_TRUE(strcmp(result, "false") == 0);

        ASSERT_TRUE(find_begin_from_string(&find, "m.u", &doc));
        result = find_result_to_str(&sb1, &find);
        ASSERT_TRUE(strcmp(result, "[1, 2, 3, null]") == 0);

        ASSERT_TRUE(find_begin_from_string(&find, "m.u.0", &doc));
        result = find_result_to_str(&sb1, &find);
        ASSERT_TRUE(strcmp(result, "1") == 0);

        ASSERT_TRUE(find_begin_from_string(&find, "m.u.1", &doc));
        result = find_result_to_str(&sb1, &find);
        ASSERT_TRUE(strcmp(result, "2") == 0);

        ASSERT_TRUE(find_begin_from_string(&find, "m.u.2", &doc));
        result = find_result_to_str(&sb1, &find);
        ASSERT_TRUE(strcmp(result, "3") == 0);

        ASSERT_TRUE(find_begin_from_string(&find, "m.u.3", &doc));
        result = find_result_to_str(&sb1, &find);
        ASSERT_TRUE(strcmp(result, "null") == 0);

        ASSERT_FALSE(find_begin_from_string(&find, "m.u.4", &doc));
        result = find_result_to_str(&sb1, &find);
        ASSERT_TRUE(strcmp(result, "undef") == 0);

        ASSERT_TRUE(find_begin_from_string(&find, "m.v", &doc));
        result = find_result_to_str(&sb1, &find);
        ASSERT_TRUE(strcmp(result, "[\"A\", \"B\", null]") == 0);

        ASSERT_TRUE(find_begin_from_string(&find, "m.w", &doc));
        result = find_result_to_str(&sb1, &find);
        ASSERT_TRUE(strcmp(result, "\"Hello, World!\"") == 0);

        ASSERT_TRUE(find_begin_from_string(&find, "m.x", &doc));
        result = find_result_to_str(&sb1, &find);
        ASSERT_TRUE(strcmp(result, "{\"a\":null}") == 0);

        ASSERT_TRUE(find_begin_from_string(&find, "m.x.a", &doc));
        result = find_result_to_str(&sb1, &find);
        ASSERT_TRUE(strcmp(result, "null") == 0);

        ASSERT_TRUE(find_begin_from_string(&find, "m.y", &doc));
        result = find_result_to_str(&sb1, &find);
        ASSERT_TRUE(strcmp(result, "[]") == 0);

        ASSERT_TRUE(find_begin_from_string(&find, "m.z", &doc));
        result = find_result_to_str(&sb1, &find);
        ASSERT_TRUE(strcmp(result, "{}") == 0);

        str_buf_drop(&sb1);
        rec_drop(&doc);
}

TEST(CarbonTest, CarbonFindPrintExamples)
{
        rec doc;
        find find;
        str_buf result;

        const char *json = "{\"x\":{\"y\":[{\"z\":23}, {\"z\":null}]} }";

        rec_from_json(&doc, json, KEY_NOKEY, NULL);
        str_buf_create(&result);

        printf("input: '%s'\n", json);

        find_begin_from_string(&find, "x", &doc);
        printf("x\t\t\t->\t%s\n", find_result_to_str(&result, &find));

        find_begin_from_string(&find, "x.y", &doc);
        printf("x.y\t\t\t->\t%s\n", find_result_to_str(&result, &find));

        find_begin_from_string(&find, "x.z", &doc);
        printf("x.z\t\t\t->\t%s\n", find_result_to_str(&result, &find));

        find_begin_from_string(&find, "x.y.z", &doc);
        printf("x.y.z\t\t->\t%s\n", find_result_to_str(&result, &find));

        find_begin_from_string(&find, "x.y.0.z", &doc);
        printf("x.y.0.z\t\t->\t%s\n", find_result_to_str(&result, &find));

        find_begin_from_string(&find, "x.y.1.z", &doc);
        printf("x.y.0.z\t\t->\t%s\n", find_result_to_str(&result, &find));

        str_buf_drop(&result);
        rec_drop(&doc);
}

TEST(CarbonTest, ParseBooleanArray) {
        rec doc;
        find find;
        field_e type;
        const char *json = "[{\"col\":[true, null, false]}]";

        rec_from_json(&doc, json, KEY_NOKEY, NULL);

        ASSERT_TRUE(find_begin_from_string(&find, "0.col", &doc));
        ASSERT_TRUE(find_has_result(&find));
        find_result_type(&type, &find);
        ASSERT_EQ(type, FIELD_COLUMN_BOOLEAN_UNSORTED_MULTISET);

        ASSERT_TRUE(find_begin_from_string(&find, "0.col.0", &doc));
        ASSERT_TRUE(find_has_result(&find));
        find_result_type(&type, &find);
        ASSERT_EQ(type, FIELD_TRUE);

        ASSERT_TRUE(find_begin_from_string(&find, "0.col.1", &doc));
        ASSERT_TRUE(find_has_result(&find));
        find_result_type(&type, &find);
        ASSERT_EQ(type, FIELD_NULL);

        ASSERT_TRUE(find_begin_from_string(&find, "0.col.2", &doc));
        ASSERT_TRUE(find_has_result(&find));
        find_result_type(&type, &find);
        ASSERT_EQ(type, FIELD_FALSE);

        rec_drop(&doc);
}

TEST(CarbonTest, PathIndex) {
        pindex index;
        rec doc;

        const char *json = "[\n"
                           "   {\n"
                           "      \"a\":null,\n"
                           "      \"b\":[ 1, 2, 3 ],\n"
                           "      \"c\":{\n"
                           "         \"msg\":\"Hello, World!\"\n"
                           "      }\n"
                           "   },\n"
                           "   {\n"
                           "      \"a\":42,\n"
                           "      \"b\":[ ],\n"
                           "      \"c\":null\n"
                           "   }\n"
                           "]";

//        /* the working directory must be the repository root */
//        int fd = open("tests/carbon/assets/ms-academic-graph.json", O_RDONLY);
//        ASSERT_NE(fd, -1);
//        int json_in_len = lseek(fd, 0, SEEK_END);
//        const char *json = (const char *) mmap(0, json_in_len, PROT_READ, MAP_PRIVATE, fd, 0);

        rec_from_json(&doc, json, KEY_NOKEY, NULL);
        pindex_create(&index, &doc);
        pindex_print(stdout, &index);
        rec_hexdump_print(stdout, &doc);
        pindex_hexdump(stdout, &index);

        rec doc2;
        pindex_to_record(&doc2, &index);
        rec_drop(&doc2);

        ASSERT_TRUE(pindex_indexes_doc(&index, &doc));
        rec_drop(&doc);
}

TEST(CarbonTest, CommitHashStr) {
        str_buf s;
        str_buf_create(&s);

        ASSERT_TRUE(strcmp(commit_to_str(&s, 1), "0000000000000001") == 0);
        ASSERT_TRUE(strcmp(commit_to_str(&s, 42), "000000000000002a") == 0);
        ASSERT_TRUE(strcmp(commit_to_str(&s, 432432532532323), "0001894b8b7dac63") == 0);
        ASSERT_TRUE(strcmp(commit_to_str(&s, 2072006001577230657), "1cc13e7b007d0141") == 0);
        ASSERT_EQ(1U, commit_from_str(commit_to_str(&s, 1)));
        ASSERT_EQ(42U, commit_from_str(commit_to_str(&s, 42)));
        ASSERT_EQ(432432532532323U, commit_from_str(commit_to_str(&s, 432432532532323)));
        ASSERT_EQ(0U, commit_from_str(""));
        ASSERT_EQ(0U, commit_from_str("hello"));
        ASSERT_EQ(0U, commit_from_str("000000000000001"));
        ASSERT_EQ(0U, commit_from_str("000000000000001Z"));
        ASSERT_EQ(0U, commit_from_str(NULL));

        str_buf_drop(&s);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}