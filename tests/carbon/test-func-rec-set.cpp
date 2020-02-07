#include <gtest/gtest.h>
#include <printf.h>

#include <karbonit/karbonit.h>

static void test_set_ex(const char *json_before, const char *path, field_e type_expected_before,
                        const char *container_path, field_e type_expected_container_before,
                        const char *new_val, field_e type_expected_after, field_e type_expected_container_after,
                        const char *json_after_expected, bool use_patching)
{
        const char *json_after_is, *json_immut_test;

        rec doc, import, rev, *ptr;
        dot dot_path, container_dot_path;
        str_buf buf;
        find find, container_find;
        field_e type_before, type_after, container_before, container_after;

        rec_from_json(&doc, json_before, KEY_NOKEY, NULL, OPTIMIZE);
        rec_from_json(&import, new_val, KEY_NOKEY, NULL, KEEP);
        dot_from_string(&dot_path, path);
        dot_from_string(&container_dot_path, container_path);
        str_buf_create(&buf);

        ASSERT_TRUE(find_from_dot(&find, &dot_path, &doc));
        find_result_type(&type_before, &find);

        ASSERT_TRUE(find_from_dot(&container_find, &container_dot_path, &doc));
        find_result_type(&container_before, &container_find);

        printf("\n** %s ***********************************************************************************\n\n"
              "*** doc dump before ***\n", use_patching ? "PATCHING" : "REVISION");
        rec_hexdump_print(stdout, &doc);
        printf("\n*** value to insert ***\n");
        rec_hexdump_print(stdout, &import);

        ptr = func_rec_set(&rev, &doc, &dot_path, &import, use_patching);

        ASSERT_TRUE(use_patching ? ptr == &doc : ptr == &rev);

        if (!use_patching) {
                /* in case of revision instead of patching, the original document must not be changed */
                json_immut_test = json_from_record(&buf, &doc);
                bool eq = strcmp(json_immut_test, json_before) == 0;
                if (!eq) {
                        printf("ERROR: ** original record has been changed **\n"
                               "expected original contents: '%s'\n"
                               "but found new contents....: '%s'\n", json_before, json_immut_test);
                }
                ASSERT_TRUE(eq);
        }

        printf("*** doc/rev dump after ***\n");
        rec_hexdump_print(stdout, ptr);

        json_after_is = json_from_record(&buf, ptr);

        ASSERT_TRUE(find_from_dot(&find, &dot_path, ptr));
        find_result_type(&type_after, &find);

        ASSERT_TRUE(find_from_dot(&container_find, &container_dot_path, ptr));
        find_result_type(&container_after, &container_find);

        printf("\n"
               "json before..........: '%s'\n"
               "path.................: '%s'\n"
               "new value............: '%s'\n"
               "json after (is) .....: '%s'\n"
               "json after (expected): '%s'\n"
               "type before..........: '%s'\n"
               "type after...........: '%s'\n"
               "type expected before.: '%s'\n"
               "type expected after..: '%s'\n"
               "container expected before.: '%s'\n"
               "container is before.: '%s'\n"
               "container expected after.: '%s'\n"
               "container is after.: '%s'\n"
               "\n",
               json_before,
               path,
               new_val,
               json_after_is,
               json_after_expected,
               field_str(type_before),
               field_str(type_after),
               field_str(type_expected_before),
               field_str(type_expected_after),
               field_str(type_expected_container_before),
               field_str(container_before),
               field_str(type_expected_container_after),
               field_str(container_after)
               );


        ASSERT_EQ(type_before, type_expected_before);
        ASSERT_EQ(type_after, type_expected_after);
        ASSERT_EQ(container_before, type_expected_container_before);
        ASSERT_EQ(container_after, type_expected_container_after);

        ASSERT_TRUE(strcmp(json_after_is, json_after_expected) == 0);

        dot_drop(&dot_path);
        dot_drop(&container_dot_path);
        str_buf_drop(&buf);
        rec_drop(&import);
        if (!use_patching) {
                rec_drop(&rev);
        }
        rec_drop(&doc);
}

static void test_set(const char *json_before, const char *path, field_e type_expected_before,
                     const char *container_path, field_e type_expected_container_before,
                     const char *new_val, field_e type_expected_after, field_e type_expected_container_after,
                     const char *json_after_expected)
{
        test_set_ex(json_before, path, type_expected_before, container_path, type_expected_container_before,
                new_val, type_expected_after, type_expected_container_after, json_after_expected, true);
        test_set_ex(json_before, path, type_expected_before, container_path, type_expected_container_before,
                    new_val, type_expected_after, type_expected_container_after, json_after_expected, false);
}


TEST(FunSetTest, SetAnyNonColumnFieldToFieldSmallerOrEq1B) {
        /* - This test deals with field update that are in-place updates WITHOUT enlarging the document.
         * - Any field can be updated as long as this field is NOT CONTAINED in an column (no column-rewrite)
         * - The path to the element to be updated MUST exist */

        // --- EQ ------------------------------------------------------------------------------------------------------

        test_set("true", "0", FIELD_TRUE, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "false", FIELD_FALSE, FIELD_ARRAY_UNSORTED_MULTISET,
                 "false");

        test_set("true", "0", FIELD_TRUE, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "null", FIELD_NULL, FIELD_ARRAY_UNSORTED_MULTISET,
                 "null");

        test_set("false", "0", FIELD_FALSE, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "true", FIELD_TRUE, FIELD_ARRAY_UNSORTED_MULTISET,
                 "true");

        test_set("false", "0", FIELD_FALSE, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "null", FIELD_NULL, FIELD_ARRAY_UNSORTED_MULTISET,
                 "null");

        test_set("null", "0", FIELD_NULL, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "true", FIELD_TRUE, FIELD_ARRAY_UNSORTED_MULTISET,
                 "true");

        test_set("null", "0", FIELD_NULL, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "null", FIELD_NULL, FIELD_ARRAY_UNSORTED_MULTISET,
                 "null");

        test_set("1", "0", FIELD_NUMBER_U8, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "5", FIELD_NUMBER_U8, FIELD_ARRAY_UNSORTED_MULTISET,
                 "5");

        test_set("1", "0", FIELD_NUMBER_U8, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "-5", FIELD_NUMBER_I8, FIELD_ARRAY_UNSORTED_MULTISET,
                 "-5");

        test_set("-5", "0", FIELD_NUMBER_I8, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "-100", FIELD_NUMBER_I8, FIELD_ARRAY_UNSORTED_MULTISET,
                 "-100");

        test_set("-5", "0", FIELD_NUMBER_I8, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "1", FIELD_NUMBER_U8, FIELD_ARRAY_UNSORTED_MULTISET,
                 "1");

        test_set("-5", "0", FIELD_NUMBER_I8, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "true", FIELD_TRUE, FIELD_ARRAY_UNSORTED_MULTISET,
                 "true");

        test_set("-5", "0", FIELD_NUMBER_I8, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "false", FIELD_FALSE, FIELD_ARRAY_UNSORTED_MULTISET,
                 "false");

        test_set("-5", "0", FIELD_NUMBER_I8, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "null", FIELD_NULL, FIELD_ARRAY_UNSORTED_MULTISET,
                 "null");
}

TEST(FunSetTest, SetAnyNonColumnFieldToFieldSmallerOrEq2B) {
        /* - This test deals with field update that are in-place updates WITHOUT enlarging the document.
         * - Any field can be updated as long as this field is NOT CONTAINED in an column (no column-rewrite)
         * - The path to the element to be updated MUST exist */

        // --- EQ ------------------------------------------------------------------------------------------------------

        test_set("255", "0", FIELD_NUMBER_U16, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "500", FIELD_NUMBER_U16, FIELD_ARRAY_UNSORTED_MULTISET,
                 "500");

        test_set("255", "0", FIELD_NUMBER_U16, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "-500", FIELD_NUMBER_I16, FIELD_ARRAY_UNSORTED_MULTISET,
                 "-500");

        test_set("-500", "0", FIELD_NUMBER_I16, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "-800", FIELD_NUMBER_I16, FIELD_ARRAY_UNSORTED_MULTISET,
                 "-800");

        test_set("-500", "0", FIELD_NUMBER_I16, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "255", FIELD_NUMBER_U16, FIELD_ARRAY_UNSORTED_MULTISET,
                 "255");

        test_set("255", "0", FIELD_NUMBER_U16, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "100", FIELD_NUMBER_U8, FIELD_ARRAY_UNSORTED_MULTISET,
                 "100");

        test_set("255", "0", FIELD_NUMBER_U16, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "-100", FIELD_NUMBER_I8, FIELD_ARRAY_UNSORTED_MULTISET,
                 "-100");

        test_set("255", "0", FIELD_NUMBER_U16, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "true", FIELD_TRUE, FIELD_ARRAY_UNSORTED_MULTISET,
                 "true");

        test_set("255", "0", FIELD_NUMBER_U16, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "false", FIELD_FALSE, FIELD_ARRAY_UNSORTED_MULTISET,
                 "false");

        test_set("255", "0", FIELD_NUMBER_U16, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "null", FIELD_NULL, FIELD_ARRAY_UNSORTED_MULTISET,
                 "null");

}

TEST(FunSetTest, SetAnyNonColumnFieldToFieldSmallerOrEq4B) {
        /* - This test deals with field update that are in-place updates WITHOUT enlarging the document.
         * - Any field can be updated as long as this field is NOT CONTAINED in an column (no column-rewrite)
         * - The path to the element to be updated MUST exist */

        // --- EQ ------------------------------------------------------------------------------------------------------

        test_set("320000", "0", FIELD_NUMBER_U32, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "350000", FIELD_NUMBER_U32, FIELD_ARRAY_UNSORTED_MULTISET,
                 "350000");

        test_set("320000", "0", FIELD_NUMBER_U32, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "-320000", FIELD_NUMBER_I32, FIELD_ARRAY_UNSORTED_MULTISET,
                 "-320000");

        test_set("-320000", "0", FIELD_NUMBER_I32, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "-320000", FIELD_NUMBER_I32, FIELD_ARRAY_UNSORTED_MULTISET,
                 "-320000");

        test_set("-320000", "0", FIELD_NUMBER_I32, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "320000", FIELD_NUMBER_U32, FIELD_ARRAY_UNSORTED_MULTISET,
                 "320000");

        test_set("320000", "0", FIELD_NUMBER_U32, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "255", FIELD_NUMBER_U16, FIELD_ARRAY_UNSORTED_MULTISET,
                 "255");

        test_set("320000", "0", FIELD_NUMBER_U32, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "-255", FIELD_NUMBER_I16, FIELD_ARRAY_UNSORTED_MULTISET,
                 "-255");

        test_set("320000", "0", FIELD_NUMBER_U32, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "100", FIELD_NUMBER_U8, FIELD_ARRAY_UNSORTED_MULTISET,
                 "100");

        test_set("320000", "0", FIELD_NUMBER_U32, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "-100", FIELD_NUMBER_I8, FIELD_ARRAY_UNSORTED_MULTISET,
                 "-100");

        test_set("320000", "0", FIELD_NUMBER_U32, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "true", FIELD_TRUE, FIELD_ARRAY_UNSORTED_MULTISET,
                 "true");

        test_set("320000", "0", FIELD_NUMBER_U32, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "false", FIELD_FALSE, FIELD_ARRAY_UNSORTED_MULTISET,
                 "false");

        test_set("320000", "0", FIELD_NUMBER_U32, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "null", FIELD_NULL, FIELD_ARRAY_UNSORTED_MULTISET,
                 "null");



        test_set("3.14", "0", FIELD_NUMBER_FLOAT, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "-3.14", FIELD_NUMBER_FLOAT, FIELD_ARRAY_UNSORTED_MULTISET,
                 "-3.14");

        test_set("3.14", "0", FIELD_NUMBER_FLOAT, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "255", FIELD_NUMBER_U16, FIELD_ARRAY_UNSORTED_MULTISET,
                 "255");

        test_set("3.14", "0", FIELD_NUMBER_FLOAT, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "-255", FIELD_NUMBER_I16, FIELD_ARRAY_UNSORTED_MULTISET,
                 "-255");

        test_set("3.14", "0", FIELD_NUMBER_FLOAT, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "100", FIELD_NUMBER_U8, FIELD_ARRAY_UNSORTED_MULTISET,
                 "100");

        test_set("3.14", "0", FIELD_NUMBER_FLOAT, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "-100", FIELD_NUMBER_I8, FIELD_ARRAY_UNSORTED_MULTISET,
                 "-100");

        test_set("3.14", "0", FIELD_NUMBER_FLOAT, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "true", FIELD_TRUE, FIELD_ARRAY_UNSORTED_MULTISET,
                 "true");

        test_set("3.14", "0", FIELD_NUMBER_FLOAT, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "false", FIELD_FALSE, FIELD_ARRAY_UNSORTED_MULTISET,
                 "false");

        test_set("3.14", "0", FIELD_NUMBER_FLOAT, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "null", FIELD_NULL, FIELD_ARRAY_UNSORTED_MULTISET,
                 "null");
}

TEST(FunSetTest, SetAnyNonColumnFieldToFieldSmallerOrEq8B) {
        /* - This test deals with field update that are in-place updates WITHOUT enlarging the document.
         * - Any field can be updated as long as this field is NOT CONTAINED in an column (no column-rewrite)
         * - The path to the element to be updated MUST exist */

        // --- EQ ------------------------------------------------------------------------------------------------------

        test_set("3200000000", "0", FIELD_NUMBER_U64, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "6200000000", FIELD_NUMBER_U64, FIELD_ARRAY_UNSORTED_MULTISET,
                 "6200000000");

        test_set("3200000000", "0", FIELD_NUMBER_U64, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "-3200000000", FIELD_NUMBER_I64, FIELD_ARRAY_UNSORTED_MULTISET,
                 "-3200000000");

        test_set("-3200000000", "0", FIELD_NUMBER_I64, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "3.14", FIELD_NUMBER_FLOAT, FIELD_ARRAY_UNSORTED_MULTISET,
                 "3.14");

        test_set("-3200000000", "0", FIELD_NUMBER_I64, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "-6200000000", FIELD_NUMBER_I64, FIELD_ARRAY_UNSORTED_MULTISET,
                 "-6200000000");

        test_set("-3200000000", "0", FIELD_NUMBER_I64, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "3200000000", FIELD_NUMBER_U64, FIELD_ARRAY_UNSORTED_MULTISET,
                 "3200000000");

        test_set("3200000000", "0", FIELD_NUMBER_U64, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "350000", FIELD_NUMBER_U32, FIELD_ARRAY_UNSORTED_MULTISET,
                 "350000");

        test_set("3200000000", "0", FIELD_NUMBER_U64, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "-320000", FIELD_NUMBER_I32, FIELD_ARRAY_UNSORTED_MULTISET,
                 "-320000");

        test_set("-3200000000", "0", FIELD_NUMBER_I64, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "-320000", FIELD_NUMBER_I32, FIELD_ARRAY_UNSORTED_MULTISET,
                 "-320000");

        test_set("-3200000000", "0", FIELD_NUMBER_I64, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "320000", FIELD_NUMBER_U32, FIELD_ARRAY_UNSORTED_MULTISET,
                 "320000");

        test_set("3200000000", "0", FIELD_NUMBER_U64, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "255", FIELD_NUMBER_U16, FIELD_ARRAY_UNSORTED_MULTISET,
                 "255");

        test_set("3200000000", "0", FIELD_NUMBER_U64, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "-255", FIELD_NUMBER_I16, FIELD_ARRAY_UNSORTED_MULTISET,
                 "-255");

        test_set("3200000000", "0", FIELD_NUMBER_U64, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "100", FIELD_NUMBER_U8, FIELD_ARRAY_UNSORTED_MULTISET,
                 "100");

        test_set("3200000000", "0", FIELD_NUMBER_U64, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "-100", FIELD_NUMBER_I8, FIELD_ARRAY_UNSORTED_MULTISET,
                 "-100");

        test_set("3200000000", "0", FIELD_NUMBER_U64, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "true", FIELD_TRUE, FIELD_ARRAY_UNSORTED_MULTISET,
                 "true");

        test_set("3200000000", "0", FIELD_NUMBER_U64, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "false", FIELD_FALSE, FIELD_ARRAY_UNSORTED_MULTISET,
                 "false");

        test_set("3200000000", "0", FIELD_NUMBER_U64, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "null", FIELD_NULL, FIELD_ARRAY_UNSORTED_MULTISET,
                 "null");
}

TEST(FunSetTest, SetAnyNonColumnFieldToFieldSmallerOrEqNB) {
        /* - This test deals with field update that are in-place updates WITHOUT enlarging the document.
         * - Any field can be updated as long as this field is NOT CONTAINED in an column (no column-rewrite)
         * - The path to the element to be updated MUST exist */

        // --- EQ ------------------------------------------------------------------------------------------------------

        test_set("\"Hello\"", "0", FIELD_STRING, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "\"World\"", FIELD_STRING, FIELD_ARRAY_UNSORTED_MULTISET,
                 "\"World\"");

        test_set("\"Hello World\"", "0", FIELD_STRING, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "\"\"", FIELD_STRING, FIELD_ARRAY_UNSORTED_MULTISET,
                 "\"\"");

        test_set("\"Hello World\"", "0", FIELD_STRING, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "3.14", FIELD_NUMBER_FLOAT, FIELD_ARRAY_UNSORTED_MULTISET,
                 "3.14");

        test_set("\"Hello World\"", "0", FIELD_STRING, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "-6200000000", FIELD_NUMBER_I64, FIELD_ARRAY_UNSORTED_MULTISET,
                 "-6200000000");

        test_set("\"Hello World\"", "0", FIELD_STRING, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "3200000000", FIELD_NUMBER_U64, FIELD_ARRAY_UNSORTED_MULTISET,
                 "3200000000");

        test_set("\"Hello World\"", "0", FIELD_STRING, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "350000", FIELD_NUMBER_U32, FIELD_ARRAY_UNSORTED_MULTISET,
                 "350000");

        test_set("\"Hello World\"", "0", FIELD_STRING, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "-320000", FIELD_NUMBER_I32, FIELD_ARRAY_UNSORTED_MULTISET,
                 "-320000");

        test_set("\"Hello World\"", "0", FIELD_STRING, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "-320000", FIELD_NUMBER_I32, FIELD_ARRAY_UNSORTED_MULTISET,
                 "-320000");

        test_set("\"Hello World\"", "0", FIELD_STRING, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "320000", FIELD_NUMBER_U32, FIELD_ARRAY_UNSORTED_MULTISET,
                 "320000");

        test_set("\"Hello World\"", "0", FIELD_STRING, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "255", FIELD_NUMBER_U16, FIELD_ARRAY_UNSORTED_MULTISET,
                 "255");

        test_set("\"Hello World\"", "0", FIELD_STRING, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "-255", FIELD_NUMBER_I16, FIELD_ARRAY_UNSORTED_MULTISET,
                 "-255");

        test_set("\"Hello World\"", "0", FIELD_STRING, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "100", FIELD_NUMBER_U8, FIELD_ARRAY_UNSORTED_MULTISET,
                 "100");

        test_set("\"Hello World\"", "0", FIELD_STRING, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "-100", FIELD_NUMBER_I8, FIELD_ARRAY_UNSORTED_MULTISET,
                 "-100");

        test_set("\"Hello World\"", "0", FIELD_STRING, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "true", FIELD_TRUE, FIELD_ARRAY_UNSORTED_MULTISET,
                 "true");

        test_set("\"Hello World\"", "0", FIELD_STRING, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "false", FIELD_FALSE, FIELD_ARRAY_UNSORTED_MULTISET,
                 "false");

        test_set("\"Hello World\"", "0", FIELD_STRING, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "null", FIELD_NULL, FIELD_ARRAY_UNSORTED_MULTISET,
                 "null");





        test_set("[[1, 2, 3, 4, 5, 6, 7, 8, 9, 10], \"_\"]", "0", FIELD_ARRAY_UNSORTED_MULTISET, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "\"World\"", FIELD_STRING, FIELD_ARRAY_UNSORTED_MULTISET,
                 "[\"World\", \"_\"]");

        test_set("[[1, 2, 3, 4, 5, 6, 7, 8, 9, 10], \"_\"]", "0", FIELD_ARRAY_UNSORTED_MULTISET, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "\"\"", FIELD_STRING, FIELD_ARRAY_UNSORTED_MULTISET,
                 "[\"\", \"_\"]");

        test_set("[[1, 2, 3, 4, 5, 6, 7, 8, 9, 10], \"_\"]", "0", FIELD_ARRAY_UNSORTED_MULTISET, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "3.14", FIELD_NUMBER_FLOAT, FIELD_ARRAY_UNSORTED_MULTISET,
                 "[3.14, \"_\"]");

        test_set("[[1, 2, 3, 4, 5, 6, 7, 8, 9, 10], \"_\"]", "0", FIELD_ARRAY_UNSORTED_MULTISET, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "-6200000000", FIELD_NUMBER_I64, FIELD_ARRAY_UNSORTED_MULTISET,
                 "[-6200000000, \"_\"]");

        test_set("[[1, 2, 3, 4, 5, 6, 7, 8, 9, 10], \"_\"]", "0", FIELD_ARRAY_UNSORTED_MULTISET, "", FIELD_ARRAY_UNSORTED_MULTISET,
                "3200000000", FIELD_NUMBER_U64, FIELD_ARRAY_UNSORTED_MULTISET,
                "[3200000000, \"_\"]");

        test_set("[[1, 2, 3, 4, 5, 6, 7, 8, 9, 10], \"_\"]", "0", FIELD_ARRAY_UNSORTED_MULTISET, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "350000", FIELD_NUMBER_U32, FIELD_ARRAY_UNSORTED_MULTISET,
                 "[350000, \"_\"]");

        test_set("[[1, 2, 3, 4, 5, 6, 7, 8, 9, 10], \"_\"]", "0", FIELD_ARRAY_UNSORTED_MULTISET, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "-320000", FIELD_NUMBER_I32, FIELD_ARRAY_UNSORTED_MULTISET,
                 "[-320000, \"_\"]");

        test_set("[[1, 2, 3, 4, 5, 6, 7, 8, 9, 10], \"_\"]", "0", FIELD_ARRAY_UNSORTED_MULTISET, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "-320000", FIELD_NUMBER_I32, FIELD_ARRAY_UNSORTED_MULTISET,
                 "[-320000, \"_\"]");

        test_set("[[1, 2, 3, 4, 5, 6, 7, 8, 9, 10], \"_\"]", "0", FIELD_ARRAY_UNSORTED_MULTISET, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "320000", FIELD_NUMBER_U32, FIELD_ARRAY_UNSORTED_MULTISET,
                 "[320000, \"_\"]");

        test_set("[[1, 2, 3, 4, 5, 6, 7, 8, 9, 10], \"_\"]", "0", FIELD_ARRAY_UNSORTED_MULTISET, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "255", FIELD_NUMBER_U16, FIELD_ARRAY_UNSORTED_MULTISET,
                 "[255, \"_\"]");

        test_set("[[1, 2, 3, 4, 5, 6, 7, 8, 9, 10], \"_\"]", "0", FIELD_ARRAY_UNSORTED_MULTISET, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "-255", FIELD_NUMBER_I16, FIELD_ARRAY_UNSORTED_MULTISET,
                 "[-255, \"_\"]");

        test_set("[[1, 2, 3, 4, 5, 6, 7, 8, 9, 10], \"_\"]", "0", FIELD_ARRAY_UNSORTED_MULTISET, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "100", FIELD_NUMBER_U8, FIELD_ARRAY_UNSORTED_MULTISET,
                 "[100, \"_\"]");

        test_set("[[1, 2, 3, 4, 5, 6, 7, 8, 9, 10], \"_\"]", "0", FIELD_ARRAY_UNSORTED_MULTISET, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "-100", FIELD_NUMBER_I8, FIELD_ARRAY_UNSORTED_MULTISET,
                 "[-100, \"_\"]");

        test_set("[[1, 2, 3, 4, 5, 6, 7, 8, 9, 10], \"_\"]", "0", FIELD_ARRAY_UNSORTED_MULTISET, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "true", FIELD_TRUE, FIELD_ARRAY_UNSORTED_MULTISET,
                 "[true, \"_\"]");

        test_set("[[1, 2, 3, 4, 5, 6, 7, 8, 9, 10], \"_\"]", "0", FIELD_ARRAY_UNSORTED_MULTISET, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "false", FIELD_FALSE, FIELD_ARRAY_UNSORTED_MULTISET,
                 "[false, \"_\"]");

        test_set("[[1, 2, 3, 4, 5, 6, 7, 8, 9, 10], \"_\"]", "0", FIELD_ARRAY_UNSORTED_MULTISET, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "null", FIELD_NULL, FIELD_ARRAY_UNSORTED_MULTISET,
                 "[null, \"_\"]");





        test_set("{\"x\":\"Hello World\"}", "0", FIELD_OBJECT_UNSORTED_MULTIMAP, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "\"World\"", FIELD_STRING, FIELD_ARRAY_UNSORTED_MULTISET,
                 "\"World\"");

        test_set("{\"x\":\"Hello World\"}", "0", FIELD_OBJECT_UNSORTED_MULTIMAP, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "\"\"", FIELD_STRING, FIELD_ARRAY_UNSORTED_MULTISET,
                 "\"\"");

        test_set("{\"x\":\"Hello World\"}", "0", FIELD_OBJECT_UNSORTED_MULTIMAP, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "3.14", FIELD_NUMBER_FLOAT, FIELD_ARRAY_UNSORTED_MULTISET,
                 "3.14");

        test_set("{\"x\":\"Hello World\"}", "0", FIELD_OBJECT_UNSORTED_MULTIMAP, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "-6200000000", FIELD_NUMBER_I64, FIELD_ARRAY_UNSORTED_MULTISET,
                 "-6200000000");

        test_set("{\"x\":\"Hello World\"}", "0", FIELD_OBJECT_UNSORTED_MULTIMAP, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "3200000000", FIELD_NUMBER_U64, FIELD_ARRAY_UNSORTED_MULTISET,
                 "3200000000");

        test_set("{\"x\":\"Hello World\"}", "0", FIELD_OBJECT_UNSORTED_MULTIMAP, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "350000", FIELD_NUMBER_U32, FIELD_ARRAY_UNSORTED_MULTISET,
                 "350000");

        test_set("{\"x\":\"Hello World\"}", "0", FIELD_OBJECT_UNSORTED_MULTIMAP, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "-320000", FIELD_NUMBER_I32, FIELD_ARRAY_UNSORTED_MULTISET,
                 "-320000");

        test_set("{\"x\":\"Hello World\"}", "0", FIELD_OBJECT_UNSORTED_MULTIMAP, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "-320000", FIELD_NUMBER_I32, FIELD_ARRAY_UNSORTED_MULTISET,
                 "-320000");

        test_set("{\"x\":\"Hello World\"}", "0", FIELD_OBJECT_UNSORTED_MULTIMAP, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "320000", FIELD_NUMBER_U32, FIELD_ARRAY_UNSORTED_MULTISET,
                 "320000");

        test_set("{\"x\":\"Hello World\"}", "0", FIELD_OBJECT_UNSORTED_MULTIMAP, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "255", FIELD_NUMBER_U16, FIELD_ARRAY_UNSORTED_MULTISET,
                 "255");

        test_set("{\"x\":\"Hello World\"}", "0", FIELD_OBJECT_UNSORTED_MULTIMAP, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "-255", FIELD_NUMBER_I16, FIELD_ARRAY_UNSORTED_MULTISET,
                 "-255");

        test_set("{\"x\":\"Hello World\"}", "0", FIELD_OBJECT_UNSORTED_MULTIMAP, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "100", FIELD_NUMBER_U8, FIELD_ARRAY_UNSORTED_MULTISET,
                 "100");

        test_set("{\"x\":\"Hello World\"}", "0", FIELD_OBJECT_UNSORTED_MULTIMAP, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "-100", FIELD_NUMBER_I8, FIELD_ARRAY_UNSORTED_MULTISET,
                 "-100");

        test_set("{\"x\":\"Hello World\"}", "0", FIELD_OBJECT_UNSORTED_MULTIMAP, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "true", FIELD_TRUE, FIELD_ARRAY_UNSORTED_MULTISET,
                 "true");

        test_set("{\"x\":\"Hello World\"}", "0", FIELD_OBJECT_UNSORTED_MULTIMAP, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "false", FIELD_FALSE, FIELD_ARRAY_UNSORTED_MULTISET,
                 "false");

        test_set("{\"x\":\"Hello World\"}", "0", FIELD_OBJECT_UNSORTED_MULTIMAP, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "null", FIELD_NULL, FIELD_ARRAY_UNSORTED_MULTISET,
                 "null");
}

TEST(FunSetTest, SetAnyNonColumnFieldToFieldSmallerOrEqNMB) {
        /* - This test deals with field update that are in-place updates WITHOUT enlarging the document.
         * - Any field can be updated as long as this field is NOT CONTAINED in an column (no column-rewrite)
         * - The path to the element to be updated MUST exist */

        test_set("[{\"x\":\"Hello World\"}, 1, 2, 3]", "0", FIELD_OBJECT_UNSORTED_MULTIMAP, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "[[1, 2, 3, 4], \"_\"]", FIELD_ARRAY_UNSORTED_MULTISET, FIELD_ARRAY_UNSORTED_MULTISET,
                 "[[[1, 2, 3, 4], \"_\"], 1, 2, 3]");
}

TEST(FunSetTest, SetAnyNonColumnFieldToFieldLargerEq)
{
        /* - This test deals with field update that are in-place updates WITH enlarging the document.
         * - Any field can be updated as long as this field is NOT CONTAINED in an column (no column-rewrite)
         * - The path to the element to be updated MUST exist */

        test_set("true", "0", FIELD_TRUE, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "\"Hello\"", FIELD_STRING, FIELD_ARRAY_UNSORTED_MULTISET,
                 "\"Hello\"");

        test_set("true", "0", FIELD_TRUE, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "\"Hello\"", FIELD_STRING, FIELD_ARRAY_UNSORTED_MULTISET,
                 "\"Hello\"");

        test_set("true", "0", FIELD_TRUE, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "[\"Hello\", \"World\"]", FIELD_ARRAY_UNSORTED_MULTISET, FIELD_ARRAY_UNSORTED_MULTISET,
                 "[\"Hello\", \"World\"]");

        test_set("[true, false, {\"replace\":\"me\"}, null]", "1", FIELD_FALSE, "", FIELD_ARRAY_UNSORTED_MULTISET,
                 "[\"Hello\", \"World\"]", FIELD_ARRAY_UNSORTED_MULTISET, FIELD_ARRAY_UNSORTED_MULTISET,
                 "[true, [\"Hello\", \"World\"], {\"replace\":\"me\"}, null]");

        test_set("{\"x\":[1, 2, 3]}", "0.x", FIELD_COLUMN_U8_UNSORTED_MULTISET, "0", FIELD_OBJECT_UNSORTED_MULTIMAP,
                 "\"Hello\"", FIELD_STRING, FIELD_OBJECT_UNSORTED_MULTIMAP,
                 "{\"x\":\"Hello\"}");
}

TEST(FunSetTest, SetColumnFieldToFieldSmallerOrEq)
{
        /* - This test deals with field update that are in-place updates WITHOUT enlarging the document.
         * - Any field can be updated as long as this field IS CONTAINED in an column (aka column-rewrite)
         * - The path to the element to be updated MUST exist */

        test_set("{\"x\":[1, 2, 3]}", "0.x.1", FIELD_NUMBER_U8, "0.x", FIELD_COLUMN_U8_UNSORTED_MULTISET,
                 "42", FIELD_NUMBER_U8, FIELD_COLUMN_U8_UNSORTED_MULTISET,
                 "{\"x\":[1, 42, 3]}");

        test_set("{\"x\":[1, 2, 3]}", "0.x.1", FIELD_NUMBER_U8, "0.x", FIELD_COLUMN_U8_UNSORTED_MULTISET,
                 "true", FIELD_TRUE, FIELD_COLUMN_U8_UNSORTED_MULTISET,
                 "{\"x\":[1, true, 3]}");

        test_set("{\"x\":[1, 2, 3]}", "0.x.1", FIELD_NUMBER_U8, "0.x", FIELD_COLUMN_U8_UNSORTED_MULTISET,
                 "null", FIELD_NULL, FIELD_COLUMN_U8_UNSORTED_MULTISET,
                 "{\"x\":[1, null, 3]}");

        test_set("{\"x\":[50000, 50001, 50002]}", "0.x.1", FIELD_NUMBER_U16, "0.x", FIELD_COLUMN_U16_UNSORTED_MULTISET,
                 "42", FIELD_NUMBER_U16, FIELD_COLUMN_U16_UNSORTED_MULTISET,
                 "{\"x\":[50000, 0, 50002]}");
}

TEST(FunSetTest, ColumnTestInsertArrayLeadsToRewriteAsColumn)
{
        test_set("{\"x\":[1, 2, 3]}", "0.x.1", FIELD_NUMBER_U8, "0.x", FIELD_COLUMN_U8_UNSORTED_MULTISET,
                 "[2, 3]", FIELD_ARRAY_UNSORTED_MULTISET, FIELD_ARRAY_UNSORTED_MULTISET,
                 "{\"x\":[1, [2, 3], 3]}");

        test_set("{\"x\":[1, 2, 3]}", "0.x.1", FIELD_NUMBER_U8, "0.x", FIELD_COLUMN_U8_UNSORTED_MULTISET,
                 "[2]", FIELD_ARRAY_UNSORTED_MULTISET, FIELD_ARRAY_UNSORTED_MULTISET,
                 "{\"x\":[1, [2], 3]}");
}

TEST(FunSetTest, ColumnTestInsertSingleElementNullWorksAllTheTime)
{
        test_set("{\"x\":[1, 2, 3]}", "0.x.1", FIELD_NUMBER_U8, "0.x", FIELD_COLUMN_U8_UNSORTED_MULTISET,
                 "null", FIELD_NULL, FIELD_COLUMN_U8_UNSORTED_MULTISET,
                 "{\"x\":[1, null, 2]}");

        test_set("{\"x\":[-1, 2, 3]}", "0.x.1", FIELD_NUMBER_I8, "0.x", FIELD_COLUMN_I8_UNSORTED_MULTISET,
                 "null", FIELD_NULL, FIELD_COLUMN_I8_UNSORTED_MULTISET,
                 "{\"x\":[-1, null, 2]}");

        test_set("{\"x\":[65000, 2, 3]}", "0.x.1", FIELD_NUMBER_U16, "0.x", FIELD_COLUMN_U16_UNSORTED_MULTISET,
                 "null", FIELD_NULL, FIELD_COLUMN_U16_UNSORTED_MULTISET,
                 "{\"x\":[65000, null, 2]}");

        test_set("{\"x\":[-32000, 2, 3]}", "0.x.1", FIELD_NUMBER_I16, "0.x", FIELD_COLUMN_I16_UNSORTED_MULTISET,
                 "null", FIELD_NULL, FIELD_COLUMN_I16_UNSORTED_MULTISET,
                 "{\"x\":[-32000, null, 2]}");

        test_set("{\"x\":[4000000000, 2, 3]}", "0.x.1", FIELD_NUMBER_U32, "0.x", FIELD_COLUMN_U32_UNSORTED_MULTISET,
                 "null", FIELD_NULL, FIELD_COLUMN_U32_UNSORTED_MULTISET,
                 "{\"x\":[4000000000, null, 2]}");

        test_set("{\"x\":[-2000000000, 2, 3]}", "0.x.1", FIELD_NUMBER_I32, "0.x", FIELD_COLUMN_I32_UNSORTED_MULTISET,
                 "null", FIELD_NULL, FIELD_COLUMN_I32_UNSORTED_MULTISET,
                 "{\"x\":[-2000000000, null, 2]}");

        test_set("{\"x\":[18000000000000000000, 2, 3]}", "0.x.1", FIELD_NUMBER_U64, "0.x", FIELD_COLUMN_U32_UNSORTED_MULTISET,
                 "null", FIELD_NULL, FIELD_COLUMN_U32_UNSORTED_MULTISET,
                 "{\"x\":[18000000000000000000, null, 2]}");

        test_set("{\"x\":[-9000000000000000000, 2, 3]}", "0.x.1", FIELD_NUMBER_I64, "0.x", FIELD_COLUMN_I32_UNSORTED_MULTISET,
                 "null", FIELD_NULL, FIELD_COLUMN_I64_UNSORTED_MULTISET,
                 "{\"x\":[-9000000000000000000, null, 2]}");

        test_set("{\"x\":[true, false, true]}", "0.x.1", FIELD_NUMBER_U8, "0.x", FIELD_COLUMN_BOOLEAN_UNSORTED_MULTISET,
                 "null", FIELD_NULL, FIELD_COLUMN_BOOLEAN_UNSORTED_MULTISET,
                 "{\"x\":[true, null, true]}");

        test_set("{\"x\":[23.0, 42.4, -3.14]}", "0.x.1", FIELD_NUMBER_U8, "0.x", FIELD_COLUMN_FLOAT_UNSORTED_MULTISET,
                 "null", FIELD_NULL, FIELD_COLUMN_FLOAT_UNSORTED_MULTISET,
                 "{\"x\":[23.0, null, -3.14]}");
}

TEST(FunSetTest, ColumnTestInsertSingleTrueWorkForBooleanColumn)
{
        test_set("{\"x\":[false, false, false]}", "0.x.1", FIELD_FALSE, "0.x", FIELD_COLUMN_BOOLEAN_UNSORTED_MULTISET,
                 "true", FIELD_TRUE, FIELD_COLUMN_BOOLEAN_UNSORTED_MULTISET,
                 "{\"x\":[false, true, false]}");
}

TEST(FunSetTest, ColumnTestInsertSingleFalseWorkForBooleanColumn)
{
        test_set("{\"x\":[true, true, true]}", "0.x.1", FIELD_TRUE, "0.x", FIELD_COLUMN_BOOLEAN_UNSORTED_MULTISET,
                 "false", FIELD_FALSE, FIELD_COLUMN_BOOLEAN_UNSORTED_MULTISET,
                 "{\"x\":[true, false, true]}");
}

TEST(FunSetTest, ColumnTestInsertSingleBooleanIntoNumberColumnsLeadsToArrayRewrite)
{
        test_set("{\"x\":[23.0, 42.4, -3.14]}", "0.x.1", FIELD_NUMBER_FLOAT, "0.x", FIELD_COLUMN_FLOAT_UNSORTED_MULTISET,
                 "true", FIELD_TRUE, FIELD_ARRAY_UNSORTED_MULTISET,
                 "{\"x\":[23.0, true, -3.14]}");

        test_set("{\"x\":[23.0, 42.4, -3.14]}", "0.x.1", FIELD_NUMBER_FLOAT, "0.x", FIELD_COLUMN_FLOAT_UNSORTED_MULTISET,
                 "false", FIELD_FALSE, FIELD_ARRAY_UNSORTED_MULTISET,
                 "{\"x\":[23.0, false, -3.14]}");




        test_set("{\"x\":[1, 2, 3]}", "0.x.1", FIELD_NUMBER_U8, "0.x", FIELD_COLUMN_U8_UNSORTED_MULTISET,
                 "true", FIELD_TRUE, FIELD_ARRAY_UNSORTED_MULTISET,
                 "{\"x\":[1, true, 2]}");

        test_set("{\"x\":[-1, 2, 3]}", "0.x.1", FIELD_NUMBER_I8, "0.x", FIELD_COLUMN_I8_UNSORTED_MULTISET,
                 "true", FIELD_TRUE, FIELD_ARRAY_UNSORTED_MULTISET,
                 "{\"x\":[-1, true, 2]}");

        test_set("{\"x\":[65000, 2, 3]}", "0.x.1", FIELD_NUMBER_U16, "0.x", FIELD_COLUMN_U16_UNSORTED_MULTISET,
                 "true", FIELD_TRUE, FIELD_ARRAY_UNSORTED_MULTISET,
                 "{\"x\":[65000, true, 2]}");

        test_set("{\"x\":[-32000, 2, 3]}", "0.x.1", FIELD_NUMBER_I16, "0.x", FIELD_COLUMN_I16_UNSORTED_MULTISET,
                 "true", FIELD_TRUE, FIELD_ARRAY_UNSORTED_MULTISET,
                 "{\"x\":[-32000, true, 2]}");

        test_set("{\"x\":[4000000000, 2, 3]}", "0.x.1", FIELD_NUMBER_U32, "0.x", FIELD_COLUMN_U32_UNSORTED_MULTISET,
                 "true", FIELD_TRUE, FIELD_ARRAY_UNSORTED_MULTISET,
                 "{\"x\":[4000000000, true, 2]}");

        test_set("{\"x\":[-2000000000, 2, 3]}", "0.x.1", FIELD_NUMBER_I32, "0.x", FIELD_COLUMN_I32_UNSORTED_MULTISET,
                 "true", FIELD_TRUE, FIELD_ARRAY_UNSORTED_MULTISET,
                 "{\"x\":[-2000000000, true, 2]}");

        test_set("{\"x\":[18000000000000000000, 2, 3]}", "0.x.1", FIELD_NUMBER_U64, "0.x", FIELD_COLUMN_U32_UNSORTED_MULTISET,
                 "true", FIELD_TRUE, FIELD_ARRAY_UNSORTED_MULTISET,
                 "{\"x\":[18000000000000000000, true, 2]}");

        test_set("{\"x\":[-9000000000000000000, 2, 3]}", "0.x.1", FIELD_NUMBER_I64, "0.x", FIELD_COLUMN_I32_UNSORTED_MULTISET,
                 "true", FIELD_TRUE, FIELD_ARRAY_UNSORTED_MULTISET,
                 "{\"x\":[-9000000000000000000, true, 2]}");

        test_set("{\"x\":[true, false, true]}", "0.x.1", FIELD_NUMBER_U8, "0.x", FIELD_COLUMN_BOOLEAN_UNSORTED_MULTISET,
                 "true", FIELD_TRUE, FIELD_ARRAY_UNSORTED_MULTISET,
                 "{\"x\":[true, true, true]}");



        test_set("{\"x\":[1, 2, 3]}", "0.x.1", FIELD_NUMBER_U8, "0.x", FIELD_COLUMN_U8_UNSORTED_MULTISET,
                 "false", FIELD_FALSE, FIELD_ARRAY_UNSORTED_MULTISET,
                 "{\"x\":[1, false, 2]}");

        test_set("{\"x\":[-1, 2, 3]}", "0.x.1", FIELD_NUMBER_I8, "0.x", FIELD_COLUMN_I8_UNSORTED_MULTISET,
                 "false", FIELD_FALSE, FIELD_ARRAY_UNSORTED_MULTISET,
                 "{\"x\":[-1, false, 2]}");

        test_set("{\"x\":[65000, 2, 3]}", "0.x.1", FIELD_NUMBER_U16, "0.x", FIELD_COLUMN_U16_UNSORTED_MULTISET,
                 "false", FIELD_FALSE, FIELD_ARRAY_UNSORTED_MULTISET,
                 "{\"x\":[65000, false, 2]}");

        test_set("{\"x\":[-32000, 2, 3]}", "0.x.1", FIELD_NUMBER_I16, "0.x", FIELD_COLUMN_I16_UNSORTED_MULTISET,
                 "false", FIELD_FALSE, FIELD_ARRAY_UNSORTED_MULTISET,
                 "{\"x\":[-32000, false, 2]}");

        test_set("{\"x\":[4000000000, 2, 3]}", "0.x.1", FIELD_NUMBER_U32, "0.x", FIELD_COLUMN_U32_UNSORTED_MULTISET,
                 "false", FIELD_FALSE, FIELD_ARRAY_UNSORTED_MULTISET,
                 "{\"x\":[4000000000, false, 2]}");

        test_set("{\"x\":[-2000000000, 2, 3]}", "0.x.1", FIELD_NUMBER_I32, "0.x", FIELD_COLUMN_I32_UNSORTED_MULTISET,
                 "false", FIELD_FALSE, FIELD_ARRAY_UNSORTED_MULTISET,
                 "{\"x\":[-2000000000, false, 2]}");

        test_set("{\"x\":[18000000000000000000, 2, 3]}", "0.x.1", FIELD_NUMBER_U64, "0.x", FIELD_COLUMN_U32_UNSORTED_MULTISET,
                 "false", FIELD_FALSE, FIELD_ARRAY_UNSORTED_MULTISET,
                 "{\"x\":[18000000000000000000, false, 2]}");

        test_set("{\"x\":[-9000000000000000000, 2, 3]}", "0.x.1", FIELD_NUMBER_I64, "0.x", FIELD_COLUMN_I32_UNSORTED_MULTISET,
                 "false", FIELD_FALSE, FIELD_ARRAY_UNSORTED_MULTISET,
                 "{\"x\":[-9000000000000000000, false, 2]}");
}

TEST(FunSetTest, ColumnTestInsertUnsupportedColumnTypeIntoColumnLeadsToArrayRewrite)
{
        test_set("{\"x\":[1, 2, 3]}", "0.x.1", FIELD_NUMBER_U8, "0.x", FIELD_COLUMN_U8_UNSORTED_MULTISET,
                 "\"String\"", FIELD_STRING, FIELD_ARRAY_UNSORTED_MULTISET,
                 "{\"x\":[1, \"String\", 2]}");

        test_set("{\"x\":[1, 2, 3]}", "0.x.1", FIELD_NUMBER_U8, "0.x", FIELD_COLUMN_U8_UNSORTED_MULTISET,
                 "[4, 5, 6]", FIELD_ARRAY_UNSORTED_MULTISET, FIELD_ARRAY_UNSORTED_MULTISET,
                 "{\"x\":[1, [4, 5, 6], 2]}");

        test_set("{\"x\":[1, 2, 3]}", "0.x.1", FIELD_NUMBER_U8, "0.x", FIELD_COLUMN_U8_UNSORTED_MULTISET,
                 "{\"k\":\"v\"}", FIELD_OBJECT_UNSORTED_MULTIMAP, FIELD_ARRAY_UNSORTED_MULTISET,
                 "{\"x\":[1, {\"k\":\"v\"}, 2]}");
}

TEST(FunSetTest, ColumnTestInsertCompatibleValueIntoColumnDoesNotAffectColumnTypeBoolean)
{
        test_set("{\"x\":[true, false, true]}", "0.x.1", FIELD_FALSE, "0.x", FIELD_COLUMN_BOOLEAN_UNSORTED_MULTISET,
                 "true", FIELD_TRUE, FIELD_COLUMN_BOOLEAN_UNSORTED_MULTISET,
                 "{\"x\":[true, true, true]}");

        test_set("{\"x\":[false, true, false]}", "0.x.1", FIELD_FALSE, "0.x", FIELD_COLUMN_BOOLEAN_UNSORTED_MULTISET,
                 "false", FIELD_FALSE, FIELD_COLUMN_BOOLEAN_UNSORTED_MULTISET,
                 "{\"x\":[false, true, false]}");

}

TEST(FunSetTest, ColumnTestInsertCompatibleValueIntoColumnDoesNotAffectColumnTypeFloat)
{
        test_set("{\"x\":[23.0, 42.4, -3.14]}", "0.x.1", FIELD_NUMBER_FLOAT, "0.x", FIELD_COLUMN_FLOAT_UNSORTED_MULTISET,
                 "-100.0", FIELD_NUMBER_FLOAT, FIELD_COLUMN_FLOAT_UNSORTED_MULTISET,
                 "{\"x\":[23.0, -100.0, -3.14]}");


        test_set("{\"x\":[23.0, 42.4, -3.14]}", "0.x.1", FIELD_NUMBER_FLOAT, "0.x", FIELD_COLUMN_FLOAT_UNSORTED_MULTISET,
                 "1", FIELD_NUMBER_U8, FIELD_COLUMN_FLOAT_UNSORTED_MULTISET,
                 "{\"x\":[23.0, 1.0, -3.14]}");

        test_set("{\"x\":[23.0, 42.4, -3.14]}", "0.x.1", FIELD_NUMBER_FLOAT, "0.x", FIELD_COLUMN_FLOAT_UNSORTED_MULTISET,
                 "-1", FIELD_NUMBER_U8, FIELD_COLUMN_FLOAT_UNSORTED_MULTISET,
                 "{\"x\":[23.0, -1.0, -3.14]}");

        test_set("{\"x\":[23.0, 42.4, -3.14]}", "0.x.1", FIELD_NUMBER_FLOAT, "0.x", FIELD_COLUMN_FLOAT_UNSORTED_MULTISET,
                 "65000", FIELD_NUMBER_U16, FIELD_COLUMN_FLOAT_UNSORTED_MULTISET,
                 "{\"x\":[23.0, 65000.0, -3.14]}");

        test_set("{\"x\":[23.0, 42.4, -3.14]}", "0.x.1", FIELD_NUMBER_FLOAT, "0.x", FIELD_COLUMN_FLOAT_UNSORTED_MULTISET,
                 "-32000", FIELD_NUMBER_I16, FIELD_COLUMN_FLOAT_UNSORTED_MULTISET,
                 "{\"x\":[23.0, -32000.0, -3.14]}");

        test_set("{\"x\":[23.0, 42.4, -3.14]}", "0.x.1", FIELD_NUMBER_FLOAT, "0.x", FIELD_COLUMN_FLOAT_UNSORTED_MULTISET,
                 "4000000000", FIELD_NUMBER_U32, FIELD_COLUMN_FLOAT_UNSORTED_MULTISET,
                 "{\"x\":[23.0, 4000000000.0, -3.14]}");

        test_set("{\"x\":[23.0, 42.4, -3.14]}", "0.x.1", FIELD_NUMBER_FLOAT, "0.x", FIELD_COLUMN_FLOAT_UNSORTED_MULTISET,
                 "-2000000000", FIELD_NUMBER_I32, FIELD_COLUMN_FLOAT_UNSORTED_MULTISET,
                 "{\"x\":[23.0, -2000000000.0, -3.14]}");

        test_set("{\"x\":[23.0, 42.4, -3.14]}", "0.x.1", FIELD_NUMBER_FLOAT, "0.x", FIELD_COLUMN_FLOAT_UNSORTED_MULTISET,
                 "18000000000000000000", FIELD_NUMBER_U64, FIELD_COLUMN_FLOAT_UNSORTED_MULTISET,
                 "{\"x\":[23.0, 18000000000000000000.0, -3.14]}");

        test_set("{\"x\":[23.0, 42.4, -3.14]}", "0.x.1", FIELD_NUMBER_FLOAT, "0.x", FIELD_COLUMN_FLOAT_UNSORTED_MULTISET,
                 "-9000000000000000000", FIELD_NUMBER_I64, FIELD_COLUMN_FLOAT_UNSORTED_MULTISET,
                 "{\"x\":[23.0, -9000000000000000000.0, -3.14]}");
}

TEST(FunSetTest, ColumnTestInsertCompatibleValueIntoColumnDoesNotAffectColumnTypeU8)
{
        test_set("{\"x\":[1, 2, 3]}", "0.x.1", FIELD_NUMBER_U8, "0.x", FIELD_COLUMN_U8_UNSORTED_MULTISET,
                 "100", FIELD_NUMBER_U8, FIELD_COLUMN_U8_UNSORTED_MULTISET,
                 "{\"x\":[1, 100, 3]}");
}

TEST(FunSetTest, ColumnTestInsertCompatibleValueIntoColumnDoesNotAffectColumnTypeU16)
{
        test_set("{\"x\":[65000, 65001, 65002]}", "0.x.1", FIELD_NUMBER_U16, "0.x", FIELD_COLUMN_U16_UNSORTED_MULTISET,
                 "1", FIELD_NUMBER_U8, FIELD_COLUMN_U16_UNSORTED_MULTISET,
                 "{\"x\":[65000, 65001, 65002]}");

        test_set("{\"x\":[65000, 65001, 65002]}", "0.x.1", FIELD_NUMBER_U16, "0.x", FIELD_COLUMN_U16_UNSORTED_MULTISET,
                 "65003", FIELD_NUMBER_U16, FIELD_COLUMN_U16_UNSORTED_MULTISET,
                 "{\"x\":[65000, 65003, 65002]}");
}

TEST(FunSetTest, ColumnTestInsertCompatibleValueIntoColumnDoesNotAffectColumnTypeU32)
{
        test_set("{\"x\":[4000000000, 4000000001, 4000000002]}", "0.x.1", FIELD_NUMBER_U32, "0.x", FIELD_COLUMN_U32_UNSORTED_MULTISET,
                 "1", FIELD_NUMBER_U8, FIELD_COLUMN_U32_UNSORTED_MULTISET,
                 "{\"x\":[4000000000, 1, 4000000002]}");

        test_set("{\"x\":[4000000000, 4000000001, 4000000002]}", "0.x.1", FIELD_NUMBER_U32, "0.x", FIELD_COLUMN_U32_UNSORTED_MULTISET,
                 "65000", FIELD_NUMBER_U16, FIELD_COLUMN_U32_UNSORTED_MULTISET,
                 "{\"x\":[4000000000, 65000, 4000000002]}");

        test_set("{\"x\":[4000000000, 4000000001, 4000000002]}", "0.x.1", FIELD_NUMBER_U32, "0.x", FIELD_COLUMN_U32_UNSORTED_MULTISET,
                 "4000000003", FIELD_NUMBER_U32, FIELD_COLUMN_U32_UNSORTED_MULTISET,
                 "{\"x\":[4000000000, 4000000003, 4000000002]}");
}

TEST(FunSetTest, ColumnTestInsertCompatibleValueIntoColumnDoesNotAffectColumnTypeU64)
{
        test_set("{\"x\":[18000000000000000000, 18000000000000000001, 18000000000000000002]}", "0.x.1", FIELD_NUMBER_U64, "0.x", FIELD_COLUMN_U32_UNSORTED_MULTISET,
                 "1", FIELD_NUMBER_U8, FIELD_COLUMN_U64_UNSORTED_MULTISET,
                 "{\"x\":[18000000000000000000, 1, 18000000000000000002]}");

        test_set("{\"x\":[18000000000000000000, 18000000000000000001, 18000000000000000002]}", "0.x.1", FIELD_NUMBER_U64, "0.x", FIELD_COLUMN_U32_UNSORTED_MULTISET,
                 "65000", FIELD_NUMBER_U16, FIELD_COLUMN_U64_UNSORTED_MULTISET,
                 "{\"x\":[18000000000000000000, 65000, 18000000000000000002]}");

        test_set("{\"x\":[18000000000000000000, 18000000000000000001, 18000000000000000002]}", "0.x.1", FIELD_NUMBER_U64, "0.x", FIELD_COLUMN_U64_UNSORTED_MULTISET,
                 "4000000000", FIELD_NUMBER_U32, FIELD_COLUMN_U64_UNSORTED_MULTISET,
                 "{\"x\":[18000000000000000000, 4000000000, 18000000000000000002]}");

        test_set("{\"x\":[18000000000000000000, 18000000000000000001, 18000000000000000002]}", "0.x.1", FIELD_NUMBER_U64, "0.x", FIELD_COLUMN_U64_UNSORTED_MULTISET,
                 "18000000000000000003", FIELD_NUMBER_U64, FIELD_COLUMN_U64_UNSORTED_MULTISET,
                 "{\"x\":[18000000000000000000, 18000000000000000003, 18000000000000000002]}");
}






TEST(FunSetTest, ColumnTestInsertCompatibleValueIntoColumnDoesNotAffectColumnTypeI8)
{
        test_set("{\"x\":[-1, -2, -3]}", "0.x.1", FIELD_NUMBER_I8, "0.x", FIELD_COLUMN_I8_UNSORTED_MULTISET,
                 "-100", FIELD_NUMBER_I8, FIELD_COLUMN_I8_UNSORTED_MULTISET,
                 "{\"x\":[-1, -100, -3]}");
}

TEST(FunSetTest, ColumnTestInsertCompatibleValueIntoColumnDoesNotAffectColumnTypeI16)
{
        test_set("{\"x\":[-30000, -30001, -30002]}", "0.x.1", FIELD_NUMBER_I16, "0.x", FIELD_COLUMN_I16_UNSORTED_MULTISET,
                 "-1", FIELD_NUMBER_I8, FIELD_COLUMN_U16_UNSORTED_MULTISET,
                 "{\"x\":[-30000, 1, -30002]}");

        test_set("{\"x\":[-30000, -30001, -30002]}", "0.x.1", FIELD_NUMBER_I16, "0.x", FIELD_COLUMN_I16_UNSORTED_MULTISET,
                 "-30003", FIELD_NUMBER_I16, FIELD_COLUMN_I16_UNSORTED_MULTISET,
                 "{\"x\":[-30000, -30003, -30002]}");
}

TEST(FunSetTest, ColumnTestInsertCompatibleValueIntoColumnDoesNotAffectColumnTypeI32)
{
        test_set("{\"x\":[-2000000000, -2000000001, -2000000002]}", "0.x.1", FIELD_NUMBER_I32, "0.x", FIELD_COLUMN_I32_UNSORTED_MULTISET,
                 "-1", FIELD_NUMBER_I8, FIELD_COLUMN_I32_UNSORTED_MULTISET,
                 "{\"x\":[-2000000000, -1, -2000000002]}");

        test_set("{\"x\":[-2000000000, -2000000001, -2000000002]}", "0.x.1", FIELD_NUMBER_I32, "0.x", FIELD_COLUMN_I32_UNSORTED_MULTISET,
                 "-30000", FIELD_NUMBER_I16, FIELD_COLUMN_I32_UNSORTED_MULTISET,
                 "{\"x\":[-2000000000, -30000, -2000000002]}");

        test_set("{\"x\":[-2000000000, -2000000001, -2000000002]}", "0.x.1", FIELD_NUMBER_I32, "0.x", FIELD_COLUMN_I32_UNSORTED_MULTISET,
                 "-2000000003", FIELD_NUMBER_I32, FIELD_COLUMN_I32_UNSORTED_MULTISET,
                 "{\"x\":[-2000000000, -2000000003, -2000000002]}");
}

TEST(FunSetTest, ColumnTestInsertCompatibleValueIntoColumnDoesNotAffectColumnTypeI64)
{
        test_set("{\"x\":[-9000000000000000000, -9000000000000000001, -9000000000000000002]}", "0.x.1", FIELD_NUMBER_U64, "0.x", FIELD_COLUMN_U32_UNSORTED_MULTISET,
                 "-1", FIELD_NUMBER_U8, FIELD_COLUMN_U64_UNSORTED_MULTISET,
                 "{\"x\":[-9000000000000000000, -1, -9000000000000000002]}");

        test_set("{\"x\":[-9000000000000000000, -9000000000000000001, -9000000000000000002]}", "0.x.1", FIELD_NUMBER_U64, "0.x", FIELD_COLUMN_U32_UNSORTED_MULTISET,
                 "-30000", FIELD_NUMBER_U16, FIELD_COLUMN_U64_UNSORTED_MULTISET,
                 "{\"x\":[-9000000000000000000, -30000, -9000000000000000002]}");

        test_set("{\"x\":[-9000000000000000000, -9000000000000000001, -9000000000000000002]}", "0.x.1", FIELD_NUMBER_U64, "0.x", FIELD_COLUMN_U64_UNSORTED_MULTISET,
                 "-2000000000", FIELD_NUMBER_U32, FIELD_COLUMN_U64_UNSORTED_MULTISET,
                 "{\"x\":[-9000000000000000000, -2000000000, -9000000000000000002]}");

        test_set("{\"x\":[-9000000000000000000, -9000000000000000001, -9000000000000000002]}", "0.x.1", FIELD_NUMBER_U64, "0.x", FIELD_COLUMN_U64_UNSORTED_MULTISET,
                 "-9000000000000000003", FIELD_NUMBER_U64, FIELD_COLUMN_U64_UNSORTED_MULTISET,
                 "{\"x\":[-9000000000000000000, -9000000000000000003, -9000000000000000002]}");
}

TEST(FunSetTest, ColumnTestInsertEmbeddableValueIntoColumn)
{
        /* u8 is embedded in i16 */
        test_set("{\"x\":[-30000, -30001, -30002]}", "0.x.1", FIELD_NUMBER_I16, "0.x", FIELD_COLUMN_I16_UNSORTED_MULTISET,
                 "125", FIELD_NUMBER_U8, FIELD_COLUMN_I16_UNSORTED_MULTISET,
                 "{\"x\":[-30000, 125, -30002]}");

        /* u16 is embedded in i32 */
        test_set("{\"x\":[-2000000000, -2000000001, -2000000002]}", "0.x.1", FIELD_NUMBER_I32, "0.x", FIELD_COLUMN_I32_UNSORTED_MULTISET,
                 "65000", FIELD_NUMBER_U16, FIELD_COLUMN_I32_UNSORTED_MULTISET,
                 "{\"x\":[-2000000000, 65000, -2000000002]}");

        /* u32 is embedded in i64 */
        test_set("{\"x\":[-9000000000000000000, -9000000000000000001, -9000000000000000002]}", "0.x.1", FIELD_NUMBER_I64, "0.x", FIELD_COLUMN_I64_UNSORTED_MULTISET,
                 "4000000000", FIELD_NUMBER_U32, FIELD_COLUMN_I64_UNSORTED_MULTISET,
                 "{\"x\":[-9000000000000000000, 4000000000, -9000000000000000002]}");
}

TEST(FunSetTest, ColumnTestInsertValueIntoColumnForcesEnlargement)
{
        test_set("{\"x\":[0, 1, 2]}", "0.x.1", FIELD_NUMBER_U8, "0.x", FIELD_COLUMN_U8_UNSORTED_MULTISET,
                 "65000", FIELD_NUMBER_U16, FIELD_COLUMN_U16_UNSORTED_MULTISET,
                 "{\"x\":[0, 65000, 2]}");

        test_set("{\"x\":[65000, 65001, 65002]}", "0.x.1", FIELD_NUMBER_U16, "0.x", FIELD_COLUMN_U16_UNSORTED_MULTISET,
                 "4000000000", FIELD_NUMBER_U32, FIELD_COLUMN_U32_UNSORTED_MULTISET,
                 "{\"x\":[65000, 4000000000, 65002]}");

        test_set("{\"x\":[4000000000, 4000000001, 4000000002]}", "0.x.1", FIELD_NUMBER_U32, "0.x", FIELD_COLUMN_U32_UNSORTED_MULTISET,
                 "18000000000000000000", FIELD_NUMBER_U64, FIELD_COLUMN_U64_UNSORTED_MULTISET,
                 "{\"x\":[4000000000, 18000000000000000000, 4000000002]}");

        test_set("{\"x\":[0, -1, -2]}", "0.x.1", FIELD_NUMBER_I8, "0.x", FIELD_COLUMN_I8_UNSORTED_MULTISET,
                 "-30000", FIELD_NUMBER_I16, FIELD_COLUMN_I16_UNSORTED_MULTISET,
                 "{\"x\":[0, -30000, -2]}");

        test_set("{\"x\":[-30000, -30001, -30002]}", "0.x.1", FIELD_NUMBER_I16, "0.x", FIELD_COLUMN_I16_UNSORTED_MULTISET,
                 "-2000000000", FIELD_NUMBER_I32, FIELD_COLUMN_I32_UNSORTED_MULTISET,
                 "{\"x\":[-30000, -2000000000, -30002]}");

        test_set("{\"x\":[-2000000000, -2000000001, -2000000002]}", "0.x.1", FIELD_NUMBER_I32, "0.x", FIELD_COLUMN_I32_UNSORTED_MULTISET,
                 "-9000000000000000000", FIELD_NUMBER_I64, FIELD_COLUMN_I64_UNSORTED_MULTISET,
                 "{\"x\":[-2000000000, -9000000000000000000, -2000000002]}");
}

TEST(FunSetTest, ColumnTestInsertValueIntoColumnForcesEnlargementOverMultipleSteps)
{
        test_set("{\"x\":[0, 1, 2]}", "0.x.1", FIELD_NUMBER_U8, "0.x", FIELD_COLUMN_U8_UNSORTED_MULTISET,
                 "65000", FIELD_NUMBER_U16, FIELD_COLUMN_U16_UNSORTED_MULTISET,
                 "{\"x\":[0, 65000, 2]}");

        test_set("{\"x\":[0, 1, 2]}", "0.x.1", FIELD_NUMBER_U8, "0.x", FIELD_COLUMN_U8_UNSORTED_MULTISET,
                 "4000000000", FIELD_NUMBER_U32, FIELD_COLUMN_U32_UNSORTED_MULTISET,
                 "{\"x\":[65000, 4000000000, 65002]}");

        test_set("{\"x\":[0, 1, 2]}", "0.x.1", FIELD_NUMBER_U8, "0.x", FIELD_COLUMN_U8_UNSORTED_MULTISET,
                 "18000000000000000000", FIELD_NUMBER_U64, FIELD_COLUMN_U64_UNSORTED_MULTISET,
                 "{\"x\":[4000000000, 18000000000000000000, 4000000002]}");

        test_set("{\"x\":[0, 1, 2]}", "0.x.1", FIELD_NUMBER_U8, "0.x", FIELD_COLUMN_U8_UNSORTED_MULTISET,
                 "-2000000000", FIELD_NUMBER_I32, FIELD_COLUMN_I32_UNSORTED_MULTISET,
                 "{\"x\":[-30000, -2000000000, -30002]}");

        test_set("{\"x\":[0, 1, 2]}", "0.x.1", FIELD_NUMBER_U8, "0.x", FIELD_COLUMN_U8_UNSORTED_MULTISET,
                 "-9000000000000000000", FIELD_NUMBER_I64, FIELD_COLUMN_I64_UNSORTED_MULTISET,
                 "{\"x\":[-2000000000, -9000000000000000000, -2000000002]}");
        

        test_set("{\"x\":[65000, 65001, 65002]}", "0.x.1", FIELD_NUMBER_U16, "0.x", FIELD_COLUMN_U16_UNSORTED_MULTISET,
                 "4000000000", FIELD_NUMBER_U32, FIELD_COLUMN_U32_UNSORTED_MULTISET,
                 "{\"x\":[65000, 4000000000, 65002]}");

        test_set("{\"x\":[65000, 65001, 65002]}", "0.x.1", FIELD_NUMBER_U16, "0.x", FIELD_COLUMN_U16_UNSORTED_MULTISET,
                 "18000000000000000000", FIELD_NUMBER_U64, FIELD_COLUMN_U64_UNSORTED_MULTISET,
                 "{\"x\":[4000000000, 18000000000000000000, 4000000002]}");

        test_set("{\"x\":[65000, 65001, 65002]}", "0.x.1", FIELD_NUMBER_U16, "0.x", FIELD_COLUMN_U16_UNSORTED_MULTISET,
                 "-2000000000", FIELD_NUMBER_I32, FIELD_COLUMN_I32_UNSORTED_MULTISET,
                 "{\"x\":[-30000, -2000000000, -30002]}");

        test_set("{\"x\":[65000, 65001, 65002]}", "0.x.1", FIELD_NUMBER_U16, "0.x", FIELD_COLUMN_U16_UNSORTED_MULTISET,
                 "-9000000000000000000", FIELD_NUMBER_I64, FIELD_COLUMN_I64_UNSORTED_MULTISET,
                 "{\"x\":[-2000000000, -9000000000000000000, -2000000002]}");


        test_set("{\"x\":[4000000000, 4000000001, 4000000002]}", "0.x.1", FIELD_NUMBER_U32, "0.x", FIELD_COLUMN_U32_UNSORTED_MULTISET,
                 "4000000000", FIELD_NUMBER_U32, FIELD_COLUMN_U32_UNSORTED_MULTISET,
                 "{\"x\":[65000, 4000000000, 65002]}");

        test_set("{\"x\":[4000000000, 4000000001, 4000000002]}", "0.x.1", FIELD_NUMBER_U32, "0.x", FIELD_COLUMN_U32_UNSORTED_MULTISET,
                 "18000000000000000000", FIELD_NUMBER_U64, FIELD_COLUMN_U64_UNSORTED_MULTISET,
                 "{\"x\":[4000000000, 18000000000000000000, 4000000002]}");

        test_set("{\"x\":[4000000000, 4000000001, 4000000002]}", "0.x.1", FIELD_NUMBER_U32, "0.x", FIELD_COLUMN_U32_UNSORTED_MULTISET,
                 "-2000000000", FIELD_NUMBER_I32, FIELD_COLUMN_I32_UNSORTED_MULTISET,
                 "{\"x\":[-30000, -2000000000, -30002]}");

        test_set("{\"x\":[4000000000, 4000000001, 4000000002]}", "0.x.1", FIELD_NUMBER_U32, "0.x", FIELD_COLUMN_U32_UNSORTED_MULTISET,
                 "-9000000000000000000", FIELD_NUMBER_I64, FIELD_COLUMN_I64_UNSORTED_MULTISET,
                 "{\"x\":[-2000000000, -9000000000000000000, -2000000002]}");


        test_set("{\"x\":[0, -1, -2]}", "0.x.1", FIELD_NUMBER_I8, "0.x", FIELD_COLUMN_I8_UNSORTED_MULTISET,
                 "4000000000", FIELD_NUMBER_U32, FIELD_COLUMN_U32_UNSORTED_MULTISET,
                 "{\"x\":[65000, 4000000000, 65002]}");

        test_set("{\"x\":[0, -1, -2]}", "0.x.1", FIELD_NUMBER_I8, "0.x", FIELD_COLUMN_I8_UNSORTED_MULTISET,
                 "18000000000000000000", FIELD_NUMBER_U64, FIELD_COLUMN_U64_UNSORTED_MULTISET,
                 "{\"x\":[4000000000, 18000000000000000000, 4000000002]}");

        test_set("{\"x\":[0, -1, -2]}", "0.x.1", FIELD_NUMBER_I8, "0.x", FIELD_COLUMN_I8_UNSORTED_MULTISET,
                 "-2000000000", FIELD_NUMBER_I32, FIELD_COLUMN_I32_UNSORTED_MULTISET,
                 "{\"x\":[-30000, -2000000000, -30002]}");

        test_set("{\"x\":[0, -1, -2]}", "0.x.1", FIELD_NUMBER_I8, "0.x", FIELD_COLUMN_I8_UNSORTED_MULTISET,
                 "-9000000000000000000", FIELD_NUMBER_I64, FIELD_COLUMN_I64_UNSORTED_MULTISET,
                 "{\"x\":[-2000000000, -9000000000000000000, -2000000002]}");


        test_set("{\"x\":[-30000, -30001, -30002]}", "0.x.1", FIELD_NUMBER_I16, "0.x", FIELD_COLUMN_I16_UNSORTED_MULTISET,
                 "4000000000", FIELD_NUMBER_U32, FIELD_COLUMN_U32_UNSORTED_MULTISET,
                 "{\"x\":[65000, 4000000000, 65002]}");

        test_set("{\"x\":[-30000, -30001, -30002]}", "0.x.1", FIELD_NUMBER_I16, "0.x", FIELD_COLUMN_I16_UNSORTED_MULTISET,
                 "18000000000000000000", FIELD_NUMBER_U64, FIELD_COLUMN_U64_UNSORTED_MULTISET,
                 "{\"x\":[4000000000, 18000000000000000000, 4000000002]}");

        test_set("{\"x\":[-30000, -30001, -30002]}", "0.x.1", FIELD_NUMBER_I16, "0.x", FIELD_COLUMN_I16_UNSORTED_MULTISET,
                 "-2000000000", FIELD_NUMBER_I32, FIELD_COLUMN_I32_UNSORTED_MULTISET,
                 "{\"x\":[-30000, -2000000000, -30002]}");

        test_set("{\"x\":[-30000, -30001, -30002]}", "0.x.1", FIELD_NUMBER_I16, "0.x", FIELD_COLUMN_I16_UNSORTED_MULTISET,
                 "-9000000000000000000", FIELD_NUMBER_I64, FIELD_COLUMN_I64_UNSORTED_MULTISET,
                 "{\"x\":[-2000000000, -9000000000000000000, -2000000002]}");



        test_set("{\"x\":[-2000000000, -2000000001, -2000000002]}", "0.x.1", FIELD_NUMBER_I32, "0.x", FIELD_COLUMN_I32_UNSORTED_MULTISET,
                 "4000000000", FIELD_NUMBER_U32, FIELD_COLUMN_U32_UNSORTED_MULTISET,
                 "{\"x\":[65000, 4000000000, 65002]}");

        test_set("{\"x\":[-2000000000, -2000000001, -2000000002]}", "0.x.1", FIELD_NUMBER_I32, "0.x", FIELD_COLUMN_I32_UNSORTED_MULTISET,
                 "18000000000000000000", FIELD_NUMBER_U64, FIELD_COLUMN_U64_UNSORTED_MULTISET,
                 "{\"x\":[4000000000, 18000000000000000000, 4000000002]}");

        test_set("{\"x\":[-2000000000, -2000000001, -2000000002]}", "0.x.1", FIELD_NUMBER_I32, "0.x", FIELD_COLUMN_I32_UNSORTED_MULTISET,
                 "-2000000000", FIELD_NUMBER_I32, FIELD_COLUMN_I32_UNSORTED_MULTISET,
                 "{\"x\":[-30000, -2000000000, -30002]}");

        test_set("{\"x\":[-2000000000, -2000000001, -2000000002]}", "0.x.1", FIELD_NUMBER_I32, "0.x", FIELD_COLUMN_I32_UNSORTED_MULTISET,
                 "-9000000000000000000", FIELD_NUMBER_I64, FIELD_COLUMN_I64_UNSORTED_MULTISET,
                 "{\"x\":[-2000000000, -9000000000000000000, -2000000002]}");
}

TEST(FunSetTest, ColumnTestInsertValueIntoColumnForcesExpanding)
{
        test_set("{\"x\":[0, -1, -2]}", "0.x.1", FIELD_NUMBER_I8, "0.x", FIELD_COLUMN_I8_UNSORTED_MULTISET,
                 "255", FIELD_NUMBER_U8, FIELD_COLUMN_I16_UNSORTED_MULTISET,
                 "{\"x\":[0, 255, -2]}");

        test_set("{\"x\":[-30000, -30001, -30002]}", "0.x.1", FIELD_NUMBER_I16, "0.x", FIELD_COLUMN_I16_UNSORTED_MULTISET,
                 "65000", FIELD_NUMBER_U16, FIELD_COLUMN_I32_UNSORTED_MULTISET,
                 "{\"x\":[-30000, 65000, -30002]}");

        test_set("{\"x\":[-2000000000, -2000000001, -2000000002]}", "0.x.1", FIELD_NUMBER_I32, "0.x", FIELD_COLUMN_I32_UNSORTED_MULTISET,
                 "4000000000", FIELD_NUMBER_U32, FIELD_COLUMN_I64_UNSORTED_MULTISET,
                 "{\"x\":[-2000000000, 4000000000, -2000000002]}");
}

TEST(FunSetTest, ColumnTestInsertValueIntoColumnForcesExpandingMultipleSteps)
{
        test_set("{\"x\":[0, -1, -2]}", "0.x.1", FIELD_NUMBER_I8, "0.x", FIELD_COLUMN_I8_UNSORTED_MULTISET,
                 "65000", FIELD_NUMBER_U16, FIELD_COLUMN_I32_UNSORTED_MULTISET,
                 "{\"x\":[0, 65000, -2]}");

        test_set("{\"x\":[0, -1, -2]}", "0.x.1", FIELD_NUMBER_I8, "0.x", FIELD_COLUMN_I8_UNSORTED_MULTISET,
                 "4000000000", FIELD_NUMBER_U32, FIELD_COLUMN_I64_UNSORTED_MULTISET,
                 "{\"x\":[0, 4000000000, -2]}");
}

TEST(FunSetTest, ColumnTestInsertValueIntoColumnForcesDomainMatch)
{
        test_set("{\"x\":[0, 1, 2]}", "0.x.1", FIELD_NUMBER_U8, "0.x", FIELD_COLUMN_U8_UNSORTED_MULTISET,
                 "42.23", FIELD_NUMBER_FLOAT, FIELD_COLUMN_FLOAT_UNSORTED_MULTISET,
                 "{\"x\":[0.0, 42.23, 2.0]}");

        test_set("{\"x\":[-100, -101, -102]}", "0.x.1", FIELD_NUMBER_I8, "0.x", FIELD_COLUMN_I8_UNSORTED_MULTISET,
                 "42.23", FIELD_NUMBER_FLOAT, FIELD_COLUMN_FLOAT_UNSORTED_MULTISET,
                 "{\"x\":[-100.0, 42.23, -102.0]}");

        test_set("{\"x\":[65000, 65001, 65002]}", "0.x.1", FIELD_NUMBER_U16, "0.x", FIELD_COLUMN_U16_UNSORTED_MULTISET,
                 "42.23", FIELD_NUMBER_FLOAT, FIELD_COLUMN_FLOAT_UNSORTED_MULTISET,
                 "{\"x\":[65000.0, 42.23, 65002.0]}");

        test_set("{\"x\":[-30000, -30001, -30002]}", "0.x.1", FIELD_NUMBER_I16, "0.x", FIELD_COLUMN_I16_UNSORTED_MULTISET,
                 "42.23", FIELD_NUMBER_FLOAT, FIELD_COLUMN_FLOAT_UNSORTED_MULTISET,
                 "{\"x\":[-30000.0, 42.23, -30002.0]}");

        test_set("{\"x\":[4000000000, 4000000001, 4000000002]}", "0.x.1", FIELD_NUMBER_U32, "0.x", FIELD_COLUMN_U32_UNSORTED_MULTISET,
                 "42.23", FIELD_NUMBER_FLOAT, FIELD_COLUMN_FLOAT_UNSORTED_MULTISET,
                 "{\"x\":[4000000000.0, 42.23, 4000000002.0]}");

        test_set("{\"x\":[-2000000000, -2000000001, -2000000002]}", "0.x.1", FIELD_NUMBER_I32, "0.x", FIELD_COLUMN_I32_UNSORTED_MULTISET,
                 "42.23", FIELD_NUMBER_FLOAT, FIELD_COLUMN_FLOAT_UNSORTED_MULTISET,
                 "{\"x\":[-2000000000.0, 42.23, -2000000002.0]}");

        test_set("{\"x\":[18000000000000000000, 18000000000000000001, 18000000000000000002]}", "0.x.1", FIELD_NUMBER_U64, "0.x", FIELD_COLUMN_U64_UNSORTED_MULTISET,
                 "42.23", FIELD_NUMBER_FLOAT, FIELD_COLUMN_FLOAT_UNSORTED_MULTISET,
                 "{\"x\":[18000000000000000000.0, 42.23, 18000000000000000002.0]}");

        test_set("{\"x\":[-9000000000000000000, -9000000000000000001, -9000000000000000002]}", "0.x.1", FIELD_NUMBER_I64, "0.x", FIELD_COLUMN_I64_UNSORTED_MULTISET,
                 "42.23", FIELD_NUMBER_FLOAT, FIELD_COLUMN_FLOAT_UNSORTED_MULTISET,
                 "{\"x\":[-9000000000000000000.0, 42.23, -9000000000000000002.0]}");
}

TEST(FunSetTest, ColumnTestInsertValueIntoColumnForcesRewriteToArray)
{
        test_set("{\"x\":[-9000000000000000000, -9000000000000000001, -9000000000000000002]}", "0.x.1", FIELD_NUMBER_I64, "0.x", FIELD_COLUMN_I64_UNSORTED_MULTISET,
                 "18000000000000000000", FIELD_NUMBER_U64, FIELD_ARRAY_UNSORTED_MULTISET,
                 "{\"x\":[-9000000000000000000, 18000000000000000000, -9000000000000000002]}");

        test_set("{\"x\":[1, 2, 3]}", "0.x.1", FIELD_NUMBER_U8, "0.x", FIELD_COLUMN_U8_UNSORTED_MULTISET,
                 "true", FIELD_TRUE, FIELD_ARRAY_UNSORTED_MULTISET,
                 "{\"x\":[1, true, 3]}");
}

TEST(FunSetTest, ColumnForceRewriteToArray)
{
        test_set("{\"x\":[1, 2, null, 4]}", "0.x.1", FIELD_NUMBER_U8, "0.x", FIELD_COLUMN_U8_UNSORTED_MULTISET,
                 "[5, 6, null]", FIELD_ARRAY_UNSORTED_MULTISET, FIELD_ARRAY_UNSORTED_MULTISET,
                 "{\"x\":[1, [5, 6, null], null, 4]}");
}


TEST(FunSetTest, SetColumnFieldToFieldLargerEq)
{
        /* - This test deals with field update that are in-place updates WITH enlarging the document.
         * - Any field can be updated as long as this field IS CONTAINED in an column (aka column-rewrite)
         * - The path to the element to be updated MUST exist */
}

TEST(FunSetTest, ForceColumnArrayReWrite)
{
        /* This test deals with updating a value inside a column to a value that cannot be stored inside a column.
         * The effect is that the column must be rewritten to an array */

}


TEST(FunSetTest, SetFieldWhichPathNotExists)
{

}











int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}