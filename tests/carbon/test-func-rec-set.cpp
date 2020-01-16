#include <gtest/gtest.h>
#include <printf.h>

#include <karbonit/karbonit.h>

static void test_set_ex(const char *json_before, const char *path, field_e type_expected_before,
        const char *new_val, field_e type_expected_after, const char *json_after_expected, bool use_patching)
{
        const char *json_after_is, *json_immut_test;

        rec doc, import, rev, *ptr;
        dot dot_path;
        str_buf buf;
        find find;
        field_e type_before, type_after;

        rec_from_json(&doc, json_before, KEY_NOKEY, NULL, OPTIMIZE);
        rec_from_json(&import, new_val, KEY_NOKEY, NULL, KEEP);
        dot_from_string(&dot_path, path);
        str_buf_create(&buf);

        ASSERT_TRUE(find_from_dot(&find, &dot_path, &doc));
        find_result_type(&type_before, &find);

        printf("\n** %s ***********************************************************************************\n\n"
              "*** doc dump before ***\n", use_patching ? "PATCHING" : "REVISION");
        rec_hexdump_print(stdout, &doc);

        ptr = func_rec_set(&rev, &doc, &dot_path, &import, use_patching);

        ASSERT_TRUE(use_patching ? ptr == &doc : ptr == &rev);

        if (!use_patching) {
                /* in case of revision instead of patching, the original document must not be changed */
                json_immut_test = json_from_record(&buf, &doc);
                bool eq = strcmp(json_immut_test, json_before) == 0;
                if (!eq) {
                        printf("ERROR: '%s'\n", json_immut_test);
                }
                ASSERT_TRUE(eq);
        }

        printf("*** doc/rev dump after ***\n");
        rec_hexdump_print(stdout, ptr);

        json_after_is = json_from_record(&buf, ptr);

        ASSERT_TRUE(find_from_dot(&find, &dot_path, ptr));
        find_result_type(&type_after, &find);

        printf("\n"
               "json before..........: '%s'\n"
               "path.................: '%s'\n"
               "new value............: '%s'\n"
               "json after (is) .....: '%s'\n"
               "json after (expected): '%s'\n"
               "type before..........: '%s'\n"
               "type after...........: '%s'\n"
               "type expected before.: '%s'\n"
               "type expected after..: '%s'\n\n",
               json_before,
               path,
               new_val,
               json_after_is,
               json_after_expected,
               field_str(type_before),
               field_str(type_after),
               field_str(type_expected_before),
               field_str(type_expected_after));


        ASSERT_EQ(type_before, type_expected_before);
        ASSERT_EQ(type_after, type_expected_after);
        ASSERT_TRUE(strcmp(json_after_is, json_after_expected) == 0);

        dot_drop(&dot_path);
        str_buf_drop(&buf);
        rec_drop(&import);
        if (!use_patching) {
                rec_drop(&rev);
        }
        rec_drop(&doc);
}

static void test_set(const char *json_before, const char *path, field_e type_expected_before,
                     const char *new_val, field_e type_expected_after, const char *json_after_expected)
{
        test_set_ex(json_before, path, type_expected_before, new_val, type_expected_after, json_after_expected, true);
        test_set_ex(json_before, path, type_expected_before, new_val, type_expected_after, json_after_expected, false);
}

TEST(FunSetTest, SetAnyNonColumnFieldToFieldSmallerOrEqPatch1B) {
        /* - This test deals with field update that are in-place updates WITHOUT enlarging the document.
         * - Any field can be updated as long as this field is NOT CONTAINED in an column (no column-rewrite)
         * - The path to the element to be updated MUST exist
         * - The document itself should be UPDATED and therefore PATCHING is used */

        // --- EQ ------------------------------------------------------------------------------------------------------

        test_set("true", "0", FIELD_TRUE,
                 "false", FIELD_FALSE,
                 "false");

        test_set("true", "0", FIELD_TRUE,
                 "null", FIELD_NULL,
                 "null");

        test_set("false", "0", FIELD_FALSE,
                 "true", FIELD_TRUE,
                 "true");

        test_set("false", "0", FIELD_FALSE,
                 "null", FIELD_NULL,
                 "null");

        test_set("null", "0", FIELD_NULL,
                 "true", FIELD_TRUE,
                 "true");

        test_set("null", "0", FIELD_NULL,
                 "null", FIELD_NULL,
                 "null");

        test_set("1", "0", FIELD_NUMBER_U8,
                 "5", FIELD_NUMBER_U8,
                 "5");

        test_set("1", "0", FIELD_NUMBER_U8,
                 "-5", FIELD_NUMBER_I8,
                 "-5");

        test_set("-5", "0", FIELD_NUMBER_I8,
                 "-100", FIELD_NUMBER_I8,
                 "-100");

        test_set("-5", "0", FIELD_NUMBER_I8,
                 "1", FIELD_NUMBER_U8,
                 "1");

        test_set("-5", "0", FIELD_NUMBER_I8,
                 "true", FIELD_TRUE,
                 "true");

        test_set("-5", "0", FIELD_NUMBER_I8,
                 "false", FIELD_FALSE,
                 "false");

        test_set("-5", "0", FIELD_NUMBER_I8,
                 "null", FIELD_NULL,
                 "null");
}

TEST(FunSetTest, SetAnyNonColumnFieldToFieldSmallerOrEqPatch2B) {
        /* - This test deals with field update that are in-place updates WITHOUT enlarging the document.
         * - Any field can be updated as long as this field is NOT CONTAINED in an column (no column-rewrite)
         * - The path to the element to be updated MUST exist
         * - The document itself should be UPDATED and therefore PATCHING is used */

        // --- EQ ------------------------------------------------------------------------------------------------------

        test_set("255", "0", FIELD_NUMBER_U16,
                 "500", FIELD_NUMBER_U16,
                 "500");

        test_set("255", "0", FIELD_NUMBER_U16,
                 "-500", FIELD_NUMBER_I16,
                 "-500");

        test_set("-500", "0", FIELD_NUMBER_I16,
                 "-800", FIELD_NUMBER_I16,
                 "-800");

        test_set("-500", "0", FIELD_NUMBER_I16,
                 "255", FIELD_NUMBER_U16,
                 "255");

        test_set("255", "0", FIELD_NUMBER_U16,
                 "100", FIELD_NUMBER_U8,
                 "100");

        test_set("255", "0", FIELD_NUMBER_U16,
                 "-100", FIELD_NUMBER_I8,
                 "-100");

        test_set("255", "0", FIELD_NUMBER_U16,
                 "true", FIELD_TRUE,
                 "true");

        test_set("255", "0", FIELD_NUMBER_U16,
                 "false", FIELD_FALSE,
                 "false");

        test_set("255", "0", FIELD_NUMBER_U16,
                 "null", FIELD_NULL,
                 "null");

}

TEST(FunSetTest, SetAnyNonColumnFieldToFieldSmallerOrEqPatch4B) {
        /* - This test deals with field update that are in-place updates WITHOUT enlarging the document.
         * - Any field can be updated as long as this field is NOT CONTAINED in an column (no column-rewrite)
         * - The path to the element to be updated MUST exist
         * - The document itself should be UPDATED and therefore PATCHING is used */

        // --- EQ ------------------------------------------------------------------------------------------------------

        test_set("320000", "0", FIELD_NUMBER_U32,
                 "350000", FIELD_NUMBER_U32,
                 "350000");

        test_set("320000", "0", FIELD_NUMBER_U32,
                 "-320000", FIELD_NUMBER_I32,
                 "-320000");

        test_set("-320000", "0", FIELD_NUMBER_I32,
                 "-320000", FIELD_NUMBER_I32,
                 "-320000");

        test_set("-320000", "0", FIELD_NUMBER_I32,
                 "320000", FIELD_NUMBER_U32,
                 "320000");

        test_set("320000", "0", FIELD_NUMBER_U32,
                 "255", FIELD_NUMBER_U16,
                 "255");

        test_set("320000", "0", FIELD_NUMBER_U32,
                 "-255", FIELD_NUMBER_I16,
                 "-255");

        test_set("320000", "0", FIELD_NUMBER_U32,
                 "100", FIELD_NUMBER_U8,
                 "100");

        test_set("320000", "0", FIELD_NUMBER_U32,
                 "-100", FIELD_NUMBER_I8,
                 "-100");

        test_set("320000", "0", FIELD_NUMBER_U32,
                 "true", FIELD_TRUE,
                 "true");

        test_set("320000", "0", FIELD_NUMBER_U32,
                 "false", FIELD_FALSE,
                 "false");

        test_set("320000", "0", FIELD_NUMBER_U32,
                 "null", FIELD_NULL,
                 "null");



        test_set("3.14", "0", FIELD_NUMBER_FLOAT,
                 "-3.14", FIELD_NUMBER_FLOAT,
                 "-3.14");

        test_set("3.14", "0", FIELD_NUMBER_FLOAT,
                 "255", FIELD_NUMBER_U16,
                 "255");

        test_set("3.14", "0", FIELD_NUMBER_FLOAT,
                 "-255", FIELD_NUMBER_I16,
                 "-255");

        test_set("3.14", "0", FIELD_NUMBER_FLOAT,
                 "100", FIELD_NUMBER_U8,
                 "100");

        test_set("3.14", "0", FIELD_NUMBER_FLOAT,
                 "-100", FIELD_NUMBER_I8,
                 "-100");

        test_set("3.14", "0", FIELD_NUMBER_FLOAT,
                 "true", FIELD_TRUE,
                 "true");

        test_set("3.14", "0", FIELD_NUMBER_FLOAT,
                 "false", FIELD_FALSE,
                 "false");

        test_set("3.14", "0", FIELD_NUMBER_FLOAT,
                 "null", FIELD_NULL,
                 "null");
}

TEST(FunSetTest, SetAnyNonColumnFieldToFieldSmallerOrEqPatch8B) {
        /* - This test deals with field update that are in-place updates WITHOUT enlarging the document.
         * - Any field can be updated as long as this field is NOT CONTAINED in an column (no column-rewrite)
         * - The path to the element to be updated MUST exist
         * - The document itself should be UPDATED and therefore PATCHING is used */

        // --- EQ ------------------------------------------------------------------------------------------------------

        test_set("3200000000", "0", FIELD_NUMBER_U64,
                 "6200000000", FIELD_NUMBER_U64,
                 "6200000000");

        test_set("3200000000", "0", FIELD_NUMBER_U64,
                 "-3200000000", FIELD_NUMBER_I64,
                 "-3200000000");

        test_set("-3200000000", "0", FIELD_NUMBER_I64,
                 "3.14", FIELD_NUMBER_FLOAT,
                 "3.14");

        test_set("-3200000000", "0", FIELD_NUMBER_I64,
                 "-6200000000", FIELD_NUMBER_I64,
                 "-6200000000");

        test_set("-3200000000", "0", FIELD_NUMBER_I64,
                 "3200000000", FIELD_NUMBER_U64,
                 "3200000000");

        test_set("3200000000", "0", FIELD_NUMBER_U64,
                 "350000", FIELD_NUMBER_U32,
                 "350000");

        test_set("3200000000", "0", FIELD_NUMBER_U64,
                 "-320000", FIELD_NUMBER_I32,
                 "-320000");

        test_set("-3200000000", "0", FIELD_NUMBER_I64,
                 "-320000", FIELD_NUMBER_I32,
                 "-320000");

        test_set("-3200000000", "0", FIELD_NUMBER_I64,
                 "320000", FIELD_NUMBER_U32,
                 "320000");

        test_set("3200000000", "0", FIELD_NUMBER_U64,
                 "255", FIELD_NUMBER_U16,
                 "255");

        test_set("3200000000", "0", FIELD_NUMBER_U64,
                 "-255", FIELD_NUMBER_I16,
                 "-255");

        test_set("3200000000", "0", FIELD_NUMBER_U64,
                 "100", FIELD_NUMBER_U8,
                 "100");

        test_set("3200000000", "0", FIELD_NUMBER_U64,
                 "-100", FIELD_NUMBER_I8,
                 "-100");

        test_set("3200000000", "0", FIELD_NUMBER_U64,
                 "true", FIELD_TRUE,
                 "true");

        test_set("3200000000", "0", FIELD_NUMBER_U64,
                 "false", FIELD_FALSE,
                 "false");

        test_set("3200000000", "0", FIELD_NUMBER_U64,
                 "null", FIELD_NULL,
                 "null");
}

TEST(FunSetTest, SetAnyNonColumnFieldToFieldSmallerOrEqPatchNB) {
        /* - This test deals with field update that are in-place updates WITHOUT enlarging the document.
         * - Any field can be updated as long as this field is NOT CONTAINED in an column (no column-rewrite)
         * - The path to the element to be updated MUST exist
         * - The document itself should be UPDATED and therefore PATCHING is used */

        // --- EQ ------------------------------------------------------------------------------------------------------

        test_set("\"Hello\"", "0", FIELD_STRING,
                 "\"World\"", FIELD_STRING,
                 "\"World\"");

        test_set("\"Hello World\"", "0", FIELD_STRING,
                 "\"\"", FIELD_STRING,
                 "\"\"");

        test_set("\"Hello World\"", "0", FIELD_STRING,
                 "3.14", FIELD_NUMBER_FLOAT,
                 "3.14");

        test_set("\"Hello World\"", "0", FIELD_STRING,
                 "-6200000000", FIELD_NUMBER_I64,
                 "-6200000000");

        test_set("\"Hello World\"", "0", FIELD_STRING,
                 "3200000000", FIELD_NUMBER_U64,
                 "3200000000");

        test_set("\"Hello World\"", "0", FIELD_STRING,
                 "350000", FIELD_NUMBER_U32,
                 "350000");

        test_set("\"Hello World\"", "0", FIELD_STRING,
                 "-320000", FIELD_NUMBER_I32,
                 "-320000");

        test_set("\"Hello World\"", "0", FIELD_STRING,
                 "-320000", FIELD_NUMBER_I32,
                 "-320000");

        test_set("\"Hello World\"", "0", FIELD_STRING,
                 "320000", FIELD_NUMBER_U32,
                 "320000");

        test_set("\"Hello World\"", "0", FIELD_STRING,
                 "255", FIELD_NUMBER_U16,
                 "255");

        test_set("\"Hello World\"", "0", FIELD_STRING,
                 "-255", FIELD_NUMBER_I16,
                 "-255");

        test_set("\"Hello World\"", "0", FIELD_STRING,
                 "100", FIELD_NUMBER_U8,
                 "100");

        test_set("\"Hello World\"", "0", FIELD_STRING,
                 "-100", FIELD_NUMBER_I8,
                 "-100");

        test_set("\"Hello World\"", "0", FIELD_STRING,
                 "true", FIELD_TRUE,
                 "true");

        test_set("\"Hello World\"", "0", FIELD_STRING,
                 "false", FIELD_FALSE,
                 "false");

        test_set("\"Hello World\"", "0", FIELD_STRING,
                 "null", FIELD_NULL,
                 "null");





        test_set("[[1, 2, 3, 4, 5, 6, 7, 8, 9, 10], \"_\"]", "0", FIELD_ARRAY_UNSORTED_MULTISET,
                 "\"World\"", FIELD_STRING,
                 "[\"World\", \"_\"]");

        test_set("[[1, 2, 3, 4, 5, 6, 7, 8, 9, 10], \"_\"]", "0", FIELD_ARRAY_UNSORTED_MULTISET,
                 "\"\"", FIELD_STRING,
                 "[\"\", \"_\"]");

        test_set("[[1, 2, 3, 4, 5, 6, 7, 8, 9, 10], \"_\"]", "0", FIELD_ARRAY_UNSORTED_MULTISET,
                 "3.14", FIELD_NUMBER_FLOAT,
                 "[3.14, \"_\"]");

        test_set("[[1, 2, 3, 4, 5, 6, 7, 8, 9, 10], \"_\"]", "0", FIELD_ARRAY_UNSORTED_MULTISET,
                 "-6200000000", FIELD_NUMBER_I64,
                 "[-6200000000, \"_\"]");

        test_set("[[1, 2, 3, 4, 5, 6, 7, 8, 9, 10], \"_\"]", "0", FIELD_ARRAY_UNSORTED_MULTISET,
                "3200000000", FIELD_NUMBER_U64,
                "[3200000000, \"_\"]");

        test_set("[[1, 2, 3, 4, 5, 6, 7, 8, 9, 10], \"_\"]", "0", FIELD_ARRAY_UNSORTED_MULTISET,
                 "350000", FIELD_NUMBER_U32,
                 "[350000, \"_\"]");

        test_set("[[1, 2, 3, 4, 5, 6, 7, 8, 9, 10], \"_\"]", "0", FIELD_ARRAY_UNSORTED_MULTISET,
                 "-320000", FIELD_NUMBER_I32,
                 "[-320000, \"_\"]");

        test_set("[[1, 2, 3, 4, 5, 6, 7, 8, 9, 10], \"_\"]", "0", FIELD_ARRAY_UNSORTED_MULTISET,
                 "-320000", FIELD_NUMBER_I32,
                 "[-320000, \"_\"]");

        test_set("[[1, 2, 3, 4, 5, 6, 7, 8, 9, 10], \"_\"]", "0", FIELD_ARRAY_UNSORTED_MULTISET,
                 "320000", FIELD_NUMBER_U32,
                 "[320000, \"_\"]");

        test_set("[[1, 2, 3, 4, 5, 6, 7, 8, 9, 10], \"_\"]", "0", FIELD_ARRAY_UNSORTED_MULTISET,
                 "255", FIELD_NUMBER_U16,
                 "[255, \"_\"]");

        test_set("[[1, 2, 3, 4, 5, 6, 7, 8, 9, 10], \"_\"]", "0", FIELD_ARRAY_UNSORTED_MULTISET,
                 "-255", FIELD_NUMBER_I16,
                 "[-255, \"_\"]");

        test_set("[[1, 2, 3, 4, 5, 6, 7, 8, 9, 10], \"_\"]", "0", FIELD_ARRAY_UNSORTED_MULTISET,
                 "100", FIELD_NUMBER_U8,
                 "[100, \"_\"]");

        test_set("[[1, 2, 3, 4, 5, 6, 7, 8, 9, 10], \"_\"]", "0", FIELD_ARRAY_UNSORTED_MULTISET,
                 "-100", FIELD_NUMBER_I8,
                 "[-100, \"_\"]");

        test_set("[[1, 2, 3, 4, 5, 6, 7, 8, 9, 10], \"_\"]", "0", FIELD_ARRAY_UNSORTED_MULTISET,
                 "true", FIELD_TRUE,
                 "[true, \"_\"]");

        test_set("[[1, 2, 3, 4, 5, 6, 7, 8, 9, 10], \"_\"]", "0", FIELD_ARRAY_UNSORTED_MULTISET,
                 "false", FIELD_FALSE,
                 "[false, \"_\"]");

        test_set("[[1, 2, 3, 4, 5, 6, 7, 8, 9, 10], \"_\"]", "0", FIELD_ARRAY_UNSORTED_MULTISET,
                 "null", FIELD_NULL,
                 "[null, \"_\"]");





        test_set("{\"x\":\"Hello World\"}", "0", FIELD_OBJECT_UNSORTED_MULTIMAP,
                 "\"World\"", FIELD_STRING,
                 "\"World\"");

        test_set("{\"x\":\"Hello World\"}", "0", FIELD_OBJECT_UNSORTED_MULTIMAP,
                 "\"\"", FIELD_STRING,
                 "\"\"");

        test_set("{\"x\":\"Hello World\"}", "0", FIELD_OBJECT_UNSORTED_MULTIMAP,
                 "3.14", FIELD_NUMBER_FLOAT,
                 "3.14");

        test_set("{\"x\":\"Hello World\"}", "0", FIELD_OBJECT_UNSORTED_MULTIMAP,
                 "-6200000000", FIELD_NUMBER_I64,
                 "-6200000000");

        test_set("{\"x\":\"Hello World\"}", "0", FIELD_OBJECT_UNSORTED_MULTIMAP,
                 "3200000000", FIELD_NUMBER_U64,
                 "3200000000");

        test_set("{\"x\":\"Hello World\"}", "0", FIELD_OBJECT_UNSORTED_MULTIMAP,
                 "350000", FIELD_NUMBER_U32,
                 "350000");

        test_set("{\"x\":\"Hello World\"}", "0", FIELD_OBJECT_UNSORTED_MULTIMAP,
                 "-320000", FIELD_NUMBER_I32,
                 "-320000");

        test_set("{\"x\":\"Hello World\"}", "0", FIELD_OBJECT_UNSORTED_MULTIMAP,
                 "-320000", FIELD_NUMBER_I32,
                 "-320000");

        test_set("{\"x\":\"Hello World\"}", "0", FIELD_OBJECT_UNSORTED_MULTIMAP,
                 "320000", FIELD_NUMBER_U32,
                 "320000");

        test_set("{\"x\":\"Hello World\"}", "0", FIELD_OBJECT_UNSORTED_MULTIMAP,
                 "255", FIELD_NUMBER_U16,
                 "255");

        test_set("{\"x\":\"Hello World\"}", "0", FIELD_OBJECT_UNSORTED_MULTIMAP,
                 "-255", FIELD_NUMBER_I16,
                 "-255");

        test_set("{\"x\":\"Hello World\"}", "0", FIELD_OBJECT_UNSORTED_MULTIMAP,
                 "100", FIELD_NUMBER_U8,
                 "100");

        test_set("{\"x\":\"Hello World\"}", "0", FIELD_OBJECT_UNSORTED_MULTIMAP,
                 "-100", FIELD_NUMBER_I8,
                 "-100");

        test_set("{\"x\":\"Hello World\"}", "0", FIELD_OBJECT_UNSORTED_MULTIMAP,
                 "true", FIELD_TRUE,
                 "true");

        test_set("{\"x\":\"Hello World\"}", "0", FIELD_OBJECT_UNSORTED_MULTIMAP,
                 "false", FIELD_FALSE,
                 "false");

        test_set("{\"x\":\"Hello World\"}", "0", FIELD_OBJECT_UNSORTED_MULTIMAP,
                 "null", FIELD_NULL,
                 "null");
}

TEST(FunSetTest, SetAnyNonColumnFieldToFieldSmallerOrEqPatchNMB) {
        /* - This test deals with field update that are in-place updates WITHOUT enlarging the document.
         * - Any field can be updated as long as this field is NOT CONTAINED in an column (no column-rewrite)
         * - The path to the element to be updated MUST exist
         * - The document itself should be UPDATED and therefore PATCHING is used */

        test_set("[{\"x\":\"Hello World\"}, 1, 2, 3]", "0", FIELD_OBJECT_UNSORTED_MULTIMAP,
                 "[[1, 2, 3, 4], \"_\"]", FIELD_ARRAY_UNSORTED_MULTISET,
                 "[[1, 2, 3, 4], \"_\"]], 1, 2, 3]");
}

TEST(FunSetTest, SetAnyNonColumnFieldToFieldSmallerOrEqNoPatch)
{
        /* - This test deals with field update that are in-place updates WITHOUT enlarging the document.
         * - Any field can be updated as long as this field is NOT CONTAINED in an column (no column-rewrite)
         * - The path to the element to be updated MUST exist
         * - The document itself should NOT be UPDATED and therefore REVISION is used */
}

TEST(FunSetTest, SetAnyNonColumnFieldToFieldLargerEqPatch)
{
        /* - This test deals with field update that are in-place updates WITH enlarging the document.
         * - Any field can be updated as long as this field is NOT CONTAINED in an column (no column-rewrite)
         * - The path to the element to be updated MUST exist
         * - The document itself should be UPDATED and therefore PATCHING is used */
}

TEST(FunSetTest, SetAnyNonColumnFieldToFieldLargerOrEqNoPatch)
{
        /* - This test deals with field update that are in-place updates WITH enlarging the document.
         * - Any field can be updated as long as this field is NOT CONTAINED in an column (no column-rewrite)
         * - The path to the element to be updated MUST exist
         * - The document itself should NOT be UPDATED and therefore REVISION is used */
}

TEST(FunSetTest, SetColumnFieldToFieldSmallerOrEqPatch)
{
        /* - This test deals with field update that are in-place updates WITHOUT enlarging the document.
         * - Any field can be updated as long as this field IS CONTAINED in an column (aka column-rewrite)
         * - The path to the element to be updated MUST exist
         * - The document itself should be UPDATED and therefore PATCHING is used */
}

TEST(FunSetTest, SetColumnFieldToFieldSmallerOrEqNoPatch)
{
        /* - This test deals with field update that are in-place updates WITHOUT enlarging the document.
         * - Any field can be updated as long as this field IS CONTAINED in an column (aka column-rewrite)
         * - The path to the element to be updated MUST exist
         * - The document itself should NOT be UPDATED and therefore REVISION is used */
}

TEST(FunSetTest, SetColumnFieldToFieldLargerEqPatch)
{
        /* - This test deals with field update that are in-place updates WITH enlarging the document.
         * - Any field can be updated as long as this field IS CONTAINED in an column (aka column-rewrite)
         * - The path to the element to be updated MUST exist
         * - The document itself should be UPDATED and therefore PATCHING is used */
}

TEST(FunSetTest, SetColumnFieldToFieldLargerOrEqNoPatch)
{
        /* - This test deals with field update that are in-place updates WITH enlarging the document.
         * - Any field can be updated as long as this field IS CONTAINED in an column (aka column-rewrite)
         * - The path to the element to be updated MUST exist
         * - The document itself should NOT be UPDATED and therefore REVISION is used */
}











int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}