#include <gtest/gtest.h>
#include <printf.h>

#include <jakson/jakson.h>

// use
// $ cmake -DBUILD_TYPE=Release -DUSE_AMALGAMATION=ON . && make

#ifdef SAMPLE_SMALL_LIST
#define MIN_LIST_SIZE 1
#define MAX_LIST_SIZE 200
#define N_STEP_SIZE 1
#else
#define MIN_LIST_SIZE 2500
#define MAX_LIST_SIZE 210000
#define N_STEP_SIZE 2500
#endif

TEST(CarbonTest, BENCH_COLUMN)
{
        printf("num_entries,oppspsec\n");

        for (u64 n = MIN_LIST_SIZE; n <= MAX_LIST_SIZE; n += N_STEP_SIZE) {
                rec record;
                rec_new record_new;
                col_state state;
                str_buf path;
                find find;

                u64 ops = 0;
                u64 max_values = n;

                insert *ins = rec_create_begin(&record_new, &record, KEY_NOKEY, OPTIMIZE);
                insert *col_ins = insert_column_begin(&state, ins, COLUMN_U64, max_values);

                for (u64 i = 0; i < max_values; i++) {
                        insert_u64(col_ins, i);
                }
                insert_column_end(&state);
                rec_create_end(&record_new);

                str_buf_create(&path);
                u64 start = wallclock();

                const u64 measureTime = 2000; /* sample 2 sec */
                while (wallclock() - start < measureTime) {
                        i64 rand_val = (rand() % max_values) - 1;
                        i64 needle_idx = JAK_MAX(0, JAK_MIN((i64) max_values - 1, rand_val));

                        str_buf_clear(&path);
                        str_buf_add(&path, "0.");
                        str_buf_add_i64(&path, needle_idx);

                        find_begin(&find, str_buf_cstr(&path), &record);
                        assert(find_has_result(&find));
                        find_end(&find);

                        ops++;
                }
                str_buf_drop(&path);

                printf("%" PRIu64 ",%f\n", n, (float) ops / (float) (measureTime / 1000));
                fflush(stdout);

                rec_drop(&record);
        }
}

TEST(CarbonTest, BENCH_ARRAY)
{
        printf("num_entries,oppspsec\n");

        for (u64 n = MIN_LIST_SIZE; n <= MAX_LIST_SIZE; n += N_STEP_SIZE) {
                rec record;
                rec_new record_new;
                arr_state state;
                str_buf path;
                find find;

                u64 ops = 0;
                u64 max_values = n;

                insert *ins = rec_create_begin(&record_new, &record, KEY_NOKEY, OPTIMIZE);
                insert *arrins = insert_array_begin(&state, ins, max_values);

                for (u64 i = 0; i < max_values; i++) {
                        insert_u64(arrins, i);
                }

                insert_array_end(&state);
                rec_create_end(&record_new);

                str_buf_create(&path);
                u64 start = wallclock();

                const u64 measureTime = 2000; /* sample 2 sec */
                while (wallclock() - start < measureTime) {
                        i64 rand_val = (rand() % max_values) - 1;
                        i64 needle_idx = JAK_MAX(0, JAK_MIN((i64) max_values - 1, rand_val));

                        str_buf_clear(&path);
                        str_buf_add(&path, "0.");
                        str_buf_add_i64(&path, needle_idx);

                        find_begin(&find, str_buf_cstr(&path), &record);
                        assert(find_has_result(&find));
                        find_end(&find);

                        ops++;
                }
                str_buf_drop(&path);

                printf("%" PRIu64 ",%f\n", n, (float) ops / (float) (measureTime / 1000));
                fflush(stdout);

                rec_drop(&record);
        }
}

TEST(CarbonTest, CarbonFromJsonShortenedDotPath)
{
        rec doc;
        find find;
        field_e result_type;

        const char *json_in = "{\"x\":\"y\"}";
        rec_from_json(&doc, json_in, KEY_NOKEY, NULL);

        /* without shortened dot path rule, the json object as given is embedded in an record container (aka array)
         * such that the object must be referenced by its index in the record container (i.e., 0) */
        find_begin(&find, "0.x", &doc);
        ASSERT_TRUE(find_has_result(&find));
        find_result_type(&result_type, &find);
        ASSERT_EQ(result_type, FIELD_STRING);
        find_end(&find);

        /* with shortened dot path rule, the json object can be referenced without providing its index in the record */
        find_begin(&find, "x", &doc);
        ASSERT_TRUE(find_has_result(&find));
        find_result_type(&result_type, &find);
        ASSERT_EQ(result_type, FIELD_STRING);
        find_end(&find);

        rec_drop(&doc);

        json_in = "[{\"x\":\"y\"},{\"x\":[{\"z\":42}]}]";
        rec_from_json(&doc, json_in, KEY_NOKEY, NULL);

        /* The shortened dot path rule does not apply here since the user input is an array  */
        find_begin(&find, "0.x", &doc);
        ASSERT_TRUE(find_has_result(&find));
        find_result_type(&result_type, &find);
        ASSERT_EQ(result_type, FIELD_STRING);
        find_end(&find);

        find_begin(&find, "1.x", &doc);
        ASSERT_TRUE(find_has_result(&find));
        find_result_type(&result_type, &find);
        ASSERT_EQ(result_type, FIELD_ARRAY_UNSORTED_MULTISET);
        find_end(&find);

        find_begin(&find, "x", &doc);
        ASSERT_FALSE(find_has_result(&find));
        find_end(&find);

        /* The shortened dot path rule does also never apply outside the record container  */
        find_begin(&find, "1.x.0.z", &doc);
        ASSERT_TRUE(find_has_result(&find));
        find_end(&find);

        find_begin(&find, "1.x.z", &doc);
        ASSERT_FALSE(find_has_result(&find));
        find_end(&find);

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
        ASSERT_TRUE(find_begin(&find, "0", &doc));
        result = find_result_to_str(&sb1, &find);
        ASSERT_TRUE(strcmp(result, "8") == 0);
        find_end(&find);
        rec_drop(&doc);

        rec_from_json(&doc, "-8", KEY_NOKEY, NULL);
        ASSERT_TRUE(find_begin(&find, "0", &doc));
        result = find_result_to_str(&sb1, &find);
        ASSERT_TRUE(strcmp(result, "-8") == 0);
        find_end(&find);
        rec_drop(&doc);

        rec_from_json(&doc, "\"A\"", KEY_NOKEY, NULL);
        ASSERT_TRUE(find_begin(&find, "0", &doc));
        result = find_result_to_str(&sb1, &find);
        ASSERT_TRUE(strcmp(result, "\"A\"") == 0);
        find_end(&find);
        rec_drop(&doc);

        rec_from_json(&doc, "32.4", KEY_NOKEY, NULL);
        ASSERT_TRUE(find_begin(&find, "0", &doc));
        result = find_result_to_str(&sb1, &find);
        ASSERT_TRUE(strcmp(result, "32.40") == 0);
        find_end(&find);
        rec_drop(&doc);

        rec_from_json(&doc, "null", KEY_NOKEY, NULL);
        ASSERT_TRUE(find_begin(&find, "0", &doc));
        result = find_result_to_str(&sb1, &find);
        ASSERT_TRUE(strcmp(result, "null") == 0);
        find_end(&find);
        rec_drop(&doc);

        rec_from_json(&doc, "true", KEY_NOKEY, NULL);
        rec_hexdump_print(stderr, &doc);
        ASSERT_TRUE(find_begin(&find, "0", &doc));
        result = find_result_to_str(&sb1, &find);
        ASSERT_TRUE(strcmp(result, "true") == 0);
        find_end(&find);
        rec_drop(&doc);

        rec_from_json(&doc, "false", KEY_NOKEY, NULL);
        ASSERT_TRUE(find_begin(&find, "0", &doc));
        result = find_result_to_str(&sb1, &find);
        ASSERT_TRUE(strcmp(result, "false") == 0);
        find_end(&find);
        rec_drop(&doc);

        rec_from_json(&doc, "[1, 2, 3, null]", KEY_NOKEY, NULL);
        ASSERT_TRUE(find_begin(&find, "0", &doc));
        result = find_result_to_str(&sb1, &find);
        ASSERT_TRUE(strcmp(result, "1") == 0);
        find_end(&find);
        rec_drop(&doc);

        rec_from_json(&doc, "[1, 2, 3, null]", KEY_NOKEY, NULL);
        ASSERT_TRUE(find_begin(&find, "1", &doc));
        result = find_result_to_str(&sb1, &find);
        ASSERT_TRUE(strcmp(result, "2") == 0);
        find_end(&find);
        rec_drop(&doc);

        rec_from_json(&doc, "[1, 2, 3, null]", KEY_NOKEY, NULL);
        ASSERT_TRUE(find_begin(&find, "2", &doc));
        result = find_result_to_str(&sb1, &find);
        ASSERT_TRUE(strcmp(result, "3") == 0);
        find_end(&find);
        rec_drop(&doc);

        rec_from_json(&doc, "[1, 2, 3, null]", KEY_NOKEY, NULL);
        ASSERT_TRUE(find_begin(&find, "3", &doc));
        result = find_result_to_str(&sb1, &find);
        ASSERT_TRUE(strcmp(result, "null") == 0);
        find_end(&find);
        rec_drop(&doc);

        rec_from_json(&doc, "[1, 2, 3, null]", KEY_NOKEY, NULL);
        ASSERT_TRUE(find_begin(&find, "4", &doc));
        result = find_result_to_str(&sb1, &find);
        ASSERT_TRUE(strcmp(result, "undef") == 0);
        find_end(&find);
        rec_drop(&doc);

        rec_from_json(&doc, "[\"A\", \"B\", \"C\", null]", KEY_NOKEY, NULL);
        ASSERT_TRUE(find_begin(&find, "0", &doc));
        result = find_result_to_str(&sb1, &find);
        ASSERT_TRUE(strcmp(result, "\"A\"") == 0);
        find_end(&find);
        rec_drop(&doc);

        rec_from_json(&doc, "[\"A\", \"B\", \"C\", null]", KEY_NOKEY, NULL);
        ASSERT_TRUE(find_begin(&find, "1", &doc));
        result = find_result_to_str(&sb1, &find);
        ASSERT_TRUE(strcmp(result, "\"B\"") == 0);
        find_end(&find);
        rec_drop(&doc);

        rec_from_json(&doc, "[\"A\", \"B\", \"C\", null]", KEY_NOKEY, NULL);
        ASSERT_TRUE(find_begin(&find, "2", &doc));
        result = find_result_to_str(&sb1, &find);
        ASSERT_TRUE(strcmp(result, "\"C\"") == 0);
        find_end(&find);
        rec_drop(&doc);

        rec_from_json(&doc, "[\"A\", \"B\", \"C\", null]", KEY_NOKEY, NULL);
        ASSERT_TRUE(find_begin(&find, "3", &doc));
        result = find_result_to_str(&sb1, &find);
        ASSERT_TRUE(strcmp(result, "null") == 0);
        find_end(&find);
        rec_drop(&doc);

        rec_from_json(&doc, "\"Hello, World!\"", KEY_NOKEY, NULL);
        ASSERT_TRUE(find_begin(&find, "0", &doc));
        result = find_result_to_str(&sb1, &find);
        ASSERT_TRUE(strcmp(result, "\"Hello, World!\"") == 0);
        find_end(&find);
        rec_drop(&doc);

        rec_from_json(&doc, "{}", KEY_NOKEY, NULL);
        ASSERT_TRUE(find_begin(&find, "0", &doc));
        result = find_result_to_str(&sb1, &find);
        ASSERT_TRUE(strcmp(result, "{}") == 0);
        find_end(&find);
        rec_drop(&doc);

        rec_from_json(&doc, "[]", KEY_NOKEY, NULL);
        ASSERT_TRUE(find_begin(&find, "0", &doc));
        result = find_result_to_str(&sb1, &find);
        ASSERT_TRUE(strcmp(result, "undef") == 0);
        find_end(&find);
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

        ASSERT_TRUE(find_begin(&find, "m", &doc));
        result = find_result_to_str(&sb1, &find);

        ASSERT_TRUE(strcmp(result, "{\"n\":8, \"o\":-8, \"p\":\"A\", \"q\":32.40, \"r\":null, \"s\":true, \"t\":false, \"u\":[1, 2, 3, null], \"v\":[\"A\", \"B\", null], \"w\":\"Hello, World!\", \"x\":{\"a\":null}, \"y\":[], \"z\":{}}") == 0);
        find_end(&find);

        ASSERT_TRUE(find_begin(&find, "m.n", &doc));
        result = find_result_to_str(&sb1, &find);
        ASSERT_TRUE(strcmp(result, "8") == 0);
        find_end(&find);

        ASSERT_TRUE(find_begin(&find, "m.o", &doc));
        result = find_result_to_str(&sb1, &find);
        ASSERT_TRUE(strcmp(result, "-8") == 0);
        find_end(&find);

        ASSERT_TRUE(find_begin(&find, "m.p", &doc));
        result = find_result_to_str(&sb1, &find);
        ASSERT_TRUE(strcmp(result, "\"A\"") == 0);
        find_end(&find);

        ASSERT_TRUE(find_begin(&find, "m.q", &doc));
        result = find_result_to_str(&sb1, &find);
        ASSERT_TRUE(strcmp(result, "32.40") == 0);
        find_end(&find);

        ASSERT_TRUE(find_begin(&find, "m.r", &doc));
        result = find_result_to_str(&sb1, &find);
        ASSERT_TRUE(strcmp(result, "null") == 0);
        find_end(&find);

        ASSERT_TRUE(find_begin(&find, "m.s", &doc));
        result = find_result_to_str(&sb1, &find);
        ASSERT_TRUE(strcmp(result, "true") == 0);
        find_end(&find);

        ASSERT_TRUE(find_begin(&find, "m.t", &doc));
        result = find_result_to_str(&sb1, &find);
        ASSERT_TRUE(strcmp(result, "false") == 0);
        find_end(&find);

        ASSERT_TRUE(find_begin(&find, "m.u", &doc));
        result = find_result_to_str(&sb1, &find);
        ASSERT_TRUE(strcmp(result, "[1, 2, 3, null]") == 0);
        find_end(&find);

        ASSERT_TRUE(find_begin(&find, "m.u.0", &doc));
        result = find_result_to_str(&sb1, &find);
        ASSERT_TRUE(strcmp(result, "1") == 0);
        find_end(&find);

        ASSERT_TRUE(find_begin(&find, "m.u.1", &doc));
        result = find_result_to_str(&sb1, &find);
        ASSERT_TRUE(strcmp(result, "2") == 0);
        find_end(&find);

        ASSERT_TRUE(find_begin(&find, "m.u.2", &doc));
        result = find_result_to_str(&sb1, &find);
        ASSERT_TRUE(strcmp(result, "3") == 0);
        find_end(&find);

        ASSERT_TRUE(find_begin(&find, "m.u.3", &doc));
        result = find_result_to_str(&sb1, &find);
        ASSERT_TRUE(strcmp(result, "null") == 0);
        find_end(&find);

        ASSERT_TRUE(find_begin(&find, "m.u.4", &doc));
        result = find_result_to_str(&sb1, &find);
        ASSERT_TRUE(strcmp(result, "undef") == 0);
        find_end(&find);

        ASSERT_TRUE(find_begin(&find, "m.v", &doc));
        result = find_result_to_str(&sb1, &find);
        ASSERT_TRUE(strcmp(result, "[\"A\", \"B\", null]") == 0);
        find_end(&find);

        ASSERT_TRUE(find_begin(&find, "m.w", &doc));
        result = find_result_to_str(&sb1, &find);
        ASSERT_TRUE(strcmp(result, "\"Hello, World!\"") == 0);
        find_end(&find);

        ASSERT_TRUE(find_begin(&find, "m.x", &doc));
        result = find_result_to_str(&sb1, &find);
        ASSERT_TRUE(strcmp(result, "{\"a\":null}") == 0);
        find_end(&find);

        ASSERT_TRUE(find_begin(&find, "m.x.a", &doc));
        result = find_result_to_str(&sb1, &find);
        ASSERT_TRUE(strcmp(result, "null") == 0);
        find_end(&find);

        ASSERT_TRUE(find_begin(&find, "m.y", &doc));
        result = find_result_to_str(&sb1, &find);
        ASSERT_TRUE(strcmp(result, "[]") == 0);
        find_end(&find);

        ASSERT_TRUE(find_begin(&find, "m.z", &doc));
        result = find_result_to_str(&sb1, &find);
        ASSERT_TRUE(strcmp(result, "{}") == 0);
        find_end(&find);

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

        find_begin(&find, "x", &doc);
        printf("x\t\t\t->\t%s\n", find_result_to_str(&result, &find));
        find_end(&find);

        find_begin(&find, "x.y", &doc);
        printf("x.y\t\t\t->\t%s\n", find_result_to_str(&result, &find));
        find_end(&find);

        find_begin(&find, "x.z", &doc);
        printf("x.z\t\t\t->\t%s\n", find_result_to_str(&result, &find));
        find_end(&find);

        find_begin(&find, "x.y.z", &doc);
        printf("x.y.z\t\t->\t%s\n", find_result_to_str(&result, &find));
        find_end(&find);

        find_begin(&find, "x.y.0.z", &doc);
        printf("x.y.0.z\t\t->\t%s\n", find_result_to_str(&result, &find));
        find_end(&find);

        find_begin(&find, "x.y.1.z", &doc);
        printf("x.y.0.z\t\t->\t%s\n", find_result_to_str(&result, &find));
        find_end(&find);

        str_buf_drop(&result);
        rec_drop(&doc);
}

TEST(CarbonTest, ParseBooleanArray) {
        rec doc;
        find find;
        field_e type;
        const char *json = "[{\"col\":[true, null, false]}]";

        rec_from_json(&doc, json, KEY_NOKEY, NULL);

        ASSERT_TRUE(find_begin(&find, "0.col", &doc));
        ASSERT_TRUE(find_has_result(&find));
        find_result_type(&type, &find);
        ASSERT_EQ(type, FIELD_COLUMN_BOOLEAN_UNSORTED_MULTISET);
        find_end(&find);

        ASSERT_TRUE(find_begin(&find, "0.col.0", &doc));
        ASSERT_TRUE(find_has_result(&find));
        find_result_type(&type, &find);
        ASSERT_EQ(type, FIELD_TRUE);
        find_end(&find);

        ASSERT_TRUE(find_begin(&find, "0.col.1", &doc));
        ASSERT_TRUE(find_has_result(&find));
        find_result_type(&type, &find);
        ASSERT_EQ(type, FIELD_NULL);
        find_end(&find);

        ASSERT_TRUE(find_begin(&find, "0.col.2", &doc));
        ASSERT_TRUE(find_has_result(&find));
        find_result_type(&type, &find);
        ASSERT_EQ(type, FIELD_FALSE);
        find_end(&find);

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