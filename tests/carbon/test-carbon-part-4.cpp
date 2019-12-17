#include <gtest/gtest.h>
#include <printf.h>

#include <jakson/jakson.h>

TEST(CarbonTest, CarbonRemoveCustomBinaryToEmpty)
{
        rec doc, rev_doc;
        rec_new context;
        rev revise;
        arr_it rev_it;
        str_buf sb;
        bool has_next;
        str_buf_create(&sb);

        // -------------------------------------------------------------------------------------------------------------
        insert *ins = rec_create_begin(&context, &doc, KEY_NOKEY, KEEP);

        const char *data = "ABC";
        insert_binary(ins, data, strlen(data), NULL, "123");

        rec_create_end(&context);
        // -------------------------------------------------------------------------------------------------------------

        char *json_1 = strdup(rec_to_json(&sb, &doc));

        //rec_hexdump_print(stdout, &doc);

        // -------------------------------------------------------------------------------------------------------------
        revise_begin(&revise, &rev_doc, &doc);
        revise_iterator_open(&rev_it, &revise);
        has_next = arr_it_next(&rev_it);
        ASSERT_TRUE(has_next);
        internal_arr_it_remove(&rev_it);
        has_next = arr_it_next(&rev_it);
        ASSERT_FALSE(has_next);
        revise_end(&revise);
        // -------------------------------------------------------------------------------------------------------------

        char *json_2 = strdup(rec_to_json(&sb, &rev_doc));

        // -------------------------------------------------------------------------------------------------------------


        // printf("BEFORE\t'%s'\nAFTER\t'%s'\n", json_1, json_2);

        ASSERT_TRUE(strcmp(json_1, "{\"type\":\"123\", \"encoding\":\"base64\", \"value\":\"A=JDAA\"}") == 0);
        ASSERT_TRUE(strcmp(json_2, "[]") == 0);

        str_buf_drop(&sb);
        rec_drop(&doc);
        rec_drop(&rev_doc);
        free(json_1);
        free(json_2);
}

TEST(CarbonTest, CarbonRemoveFirstCustomBinary)
{
        rec doc, rev_doc;
        rec_new context;
        rev revise;
        arr_it rev_it;
        str_buf sb;
        bool has_next;
        str_buf_create(&sb);

        // -------------------------------------------------------------------------------------------------------------
        insert *ins = rec_create_begin(&context, &doc, KEY_NOKEY, KEEP);

        const char *data1 = "This report, by its very length, defends itself against the risk of being read.";
        insert_binary(ins, data1, strlen(data1), NULL, "my-fancy-format");

        const char *data2 = "{\"key\": \"value\"}";
        insert_binary(ins, data2, strlen(data2), NULL, "application/something-json-like");

        rec_create_end(&context);
        // -------------------------------------------------------------------------------------------------------------

        char *json_1 = strdup(rec_to_json(&sb, &doc));

        // -------------------------------------------------------------------------------------------------------------
        revise_begin(&revise, &rev_doc, &doc);
        revise_iterator_open(&rev_it, &revise);
        has_next = arr_it_next(&rev_it);
        ASSERT_TRUE(has_next);
        internal_arr_it_remove(&rev_it);
        has_next = arr_it_next(&rev_it);
        ASSERT_TRUE(has_next);
        field_e next_type;
        arr_it_field_type(&next_type, &rev_it);
        ASSERT_EQ(next_type, FIELD_BINARY_CUSTOM);
        revise_end(&revise);
        // -------------------------------------------------------------------------------------------------------------

        char *json_2 = strdup(rec_to_json(&sb, &rev_doc));

        // -------------------------------------------------------------------------------------------------------------


        // printf("BEFORE\t'%s'\nAFTER\t'%s'\n", json_1, json_2);

        ASSERT_TRUE(strcmp(json_1, "[{\"type\":\"my-fancy-format\", \"encoding\":\"base64\", \"value\":\"VGhpcyByZXBvcnQsIGJ5IGl0cyB2ZXJ5IGxlbmd0aCwgZGVmZW5kcyBpdHNlbGYgYWdhaW5zdCB0aGUgcmlzayBvZiBiZWluZyByZWFkLgAA\"}, {\"type\":\"application/something-json-like\", \"encoding\":\"base64\", \"value\":\"eyJrZXkiOiAidmFsdWUifQAA\"}]") == 0);
        ASSERT_TRUE(strcmp(json_2, "{\"type\":\"application/something-json-like\", \"encoding\":\"base64\", \"value\":\"eyJrZXkiOiAidmFsdWUifQAA\"}") == 0);

        str_buf_drop(&sb);
        rec_drop(&doc);
        rec_drop(&rev_doc);
        free(json_1);
        free(json_2);
}

TEST(CarbonTest, CarbonRemoveLastCustomBinary)
{
        rec doc, rev_doc;
        rec_new context;
        rev revise;
        arr_it rev_it;
        str_buf sb;
        bool has_next;
        str_buf_create(&sb);

        // -------------------------------------------------------------------------------------------------------------
        insert *ins = rec_create_begin(&context, &doc, KEY_NOKEY, KEEP);

        const char *data1 = "This report, by its very length, defends itself against the risk of being read.";
        insert_binary(ins, data1, strlen(data1), NULL, "my-fancy-format");

        const char *data2 = "{\"key\": \"value\"}";
        insert_binary(ins, data2, strlen(data2), NULL, "application/something-json-like");

        rec_create_end(&context);
        // -------------------------------------------------------------------------------------------------------------

        char *json_1 = strdup(rec_to_json(&sb, &doc));

        // -------------------------------------------------------------------------------------------------------------
        revise_begin(&revise, &rev_doc, &doc);
        revise_iterator_open(&rev_it, &revise);
        has_next = arr_it_next(&rev_it);
        ASSERT_TRUE(has_next);
        has_next = arr_it_next(&rev_it);
        ASSERT_TRUE(has_next);
        internal_arr_it_remove(&rev_it);
        has_next = arr_it_next(&rev_it);
        ASSERT_FALSE(has_next);
        revise_end(&revise);
        // -------------------------------------------------------------------------------------------------------------

        char *json_2 = strdup(rec_to_json(&sb, &rev_doc));

        // -------------------------------------------------------------------------------------------------------------


        // printf("BEFORE\t'%s'\nAFTER\t'%s'\n", json_1, json_2);

        ASSERT_TRUE(strcmp(json_1, "[{\"type\":\"my-fancy-format\", \"encoding\":\"base64\", \"value\":\"VGhpcyByZXBvcnQsIGJ5IGl0cyB2ZXJ5IGxlbmd0aCwgZGVmZW5kcyBpdHNlbGYgYWdhaW5zdCB0aGUgcmlzayBvZiBiZWluZyByZWFkLgAA\"}, {\"type\":\"application/something-json-like\", \"encoding\":\"base64\", \"value\":\"eyJrZXkiOiAidmFsdWUifQAA\"}]") == 0);
        ASSERT_TRUE(strcmp(json_2, "{\"type\":\"my-fancy-format\", \"encoding\":\"base64\", \"value\":\"VGhpcyByZXBvcnQsIGJ5IGl0cyB2ZXJ5IGxlbmd0aCwgZGVmZW5kcyBpdHNlbGYgYWdhaW5zdCB0aGUgcmlzayBvZiBiZWluZyByZWFkLgAA\"}") == 0);

        str_buf_drop(&sb);
        rec_drop(&doc);
        rec_drop(&rev_doc);
        free(json_1);
        free(json_2);
}

TEST(CarbonTest, CarbonRemoveMiddleCustomBinary)
{
        rec doc, rev_doc;
        rec_new context;
        rev revise;
        arr_it rev_it;
        str_buf sb;
        bool has_next;
        str_buf_create(&sb);

        // -------------------------------------------------------------------------------------------------------------
        insert *ins = rec_create_begin(&context, &doc, KEY_NOKEY, KEEP);

        const char *data1 = "This report, by its very length, defends itself against the risk of being read.";
        insert_binary(ins, data1, strlen(data1), NULL, "my-fancy-format");

        const char *data2 = "{\"key\": \"value\"}";
        insert_binary(ins, data2, strlen(data2), NULL, "application/something-json-like");

        const char *data3 = "<html><body><p>The quick brown fox jumps over the lazy dog</p></body></html>";
        insert_binary(ins, data3, strlen(data3), NULL, "my-other-nonstandard-format");

        rec_create_end(&context);
        // -------------------------------------------------------------------------------------------------------------

        char *json_1 = strdup(rec_to_json(&sb, &doc));

        // -------------------------------------------------------------------------------------------------------------
        revise_begin(&revise, &rev_doc, &doc);
        revise_iterator_open(&rev_it, &revise);
        has_next = arr_it_next(&rev_it);
        ASSERT_TRUE(has_next);
        has_next = arr_it_next(&rev_it);
        ASSERT_TRUE(has_next);
        internal_arr_it_remove(&rev_it);
        has_next = arr_it_next(&rev_it);
        ASSERT_TRUE(has_next);
        field_e type;
        arr_it_field_type(&type, &rev_it);
        ASSERT_EQ(type, FIELD_BINARY_CUSTOM);
        has_next = arr_it_next(&rev_it);
        ASSERT_FALSE(has_next);
        revise_end(&revise);
        // -------------------------------------------------------------------------------------------------------------

        char *json_2 = strdup(rec_to_json(&sb, &rev_doc));

        // -------------------------------------------------------------------------------------------------------------

        // printf("BEFORE\t'%s'\nAFTER\t'%s'\n", json_1, json_2);

        ASSERT_TRUE(strcmp(json_1, "[{\"type\":\"my-fancy-format\", \"encoding\":\"base64\", \"value\":\"VGhpcyByZXBvcnQsIGJ5IGl0cyB2ZXJ5IGxlbmd0aCwgZGVmZW5kcyBpdHNlbGYgYWdhaW5zdCB0aGUgcmlzayBvZiBiZWluZyByZWFkLgAA\"}, {\"type\":\"application/something-json-like\", \"encoding\":\"base64\", \"value\":\"eyJrZXkiOiAidmFsdWUifQAA\"}, {\"type\":\"my-other-nonstandard-format\", \"encoding\":\"base64\", \"value\":\"PGh0bWw+PGJvZHk+PHA+VGhlIHF1aWNrIGJyb3duIGZveCBqdW1wcyBvdmVyIHRoZSBsYXp5IGRvZzwvcD48L2JvZHk+PC9odG1sPgAA\"}]") == 0);
        ASSERT_TRUE(strcmp(json_2, "[{\"type\":\"my-fancy-format\", \"encoding\":\"base64\", \"value\":\"VGhpcyByZXBvcnQsIGJ5IGl0cyB2ZXJ5IGxlbmd0aCwgZGVmZW5kcyBpdHNlbGYgYWdhaW5zdCB0aGUgcmlzayBvZiBiZWluZyByZWFkLgAA\"}, {\"type\":\"my-other-nonstandard-format\", \"encoding\":\"base64\", \"value\":\"PGh0bWw+PGJvZHk+PHA+VGhlIHF1aWNrIGJyb3duIGZveCBqdW1wcyBvdmVyIHRoZSBsYXp5IGRvZzwvcD48L2JvZHk+PC9odG1sPgAA\"}]") == 0);

        str_buf_drop(&sb);
        rec_drop(&doc);
        rec_drop(&rev_doc);
        free(json_1);
        free(json_2);
}










TEST(CarbonTest, CarbonRemoveArrayToEmpty)
{
        rec doc, rev_doc;
        rec_new context;
        rev revise;
        arr_it rev_it;
        str_buf sb;
        arr_state state;
        insert *array_ins;
        bool has_next;
        str_buf_create(&sb);

        // -------------------------------------------------------------------------------------------------------------
        insert *ins = rec_create_begin(&context, &doc, KEY_NOKEY, KEEP);

        array_ins = insert_array_begin(&state, ins, 10);
        insert_u8(array_ins, 1);
        insert_u8(array_ins, 2);
        insert_u8(array_ins, 3);
        insert_array_end(&state);

        rec_create_end(&context);
        // -------------------------------------------------------------------------------------------------------------

        char *json_1 = strdup(rec_to_json(&sb, &doc));

        //rec_hexdump_print(stdout, &doc);

        // -------------------------------------------------------------------------------------------------------------
        revise_begin(&revise, &rev_doc, &doc);
        revise_iterator_open(&rev_it, &revise);
        has_next = arr_it_next(&rev_it);
        ASSERT_TRUE(has_next);
        internal_arr_it_remove(&rev_it);
        has_next = arr_it_next(&rev_it);
        ASSERT_FALSE(has_next);
        revise_end(&revise);
        // -------------------------------------------------------------------------------------------------------------

        char *json_2 = strdup(rec_to_json(&sb, &rev_doc));

        // -------------------------------------------------------------------------------------------------------------


        // printf("BEFORE\t'%s'\nAFTER\t'%s'\n", json_1, json_2);

        ASSERT_TRUE(strcmp(json_1, "[1, 2, 3]") == 0);
        ASSERT_TRUE(strcmp(json_2, "[]") == 0);

        str_buf_drop(&sb);
        rec_drop(&doc);
        rec_drop(&rev_doc);
        free(json_1);
        free(json_2);
}

TEST(CarbonTest, CarbonRemoveFirstArray)
{
        rec doc, rev_doc;
        rec_new context;
        rev revise;
        arr_it rev_it;
        str_buf sb;
        bool has_next;
        str_buf_create(&sb);

        arr_state state;
        insert *array_ins;

        // -------------------------------------------------------------------------------------------------------------
        insert *ins = rec_create_begin(&context, &doc, KEY_NOKEY, KEEP);

        array_ins = insert_array_begin(&state, ins, 10);
        insert_u8(array_ins, 1);
        insert_u8(array_ins, 2);
        insert_u8(array_ins, 3);
        insert_array_end(&state);

        array_ins = insert_array_begin(&state, ins, 10);
        insert_u8(array_ins, 4);
        insert_u8(array_ins, 5);
        insert_u8(array_ins, 6);
        insert_array_end(&state);

        rec_create_end(&context);
        // -------------------------------------------------------------------------------------------------------------

        char *json_1 = strdup(rec_to_json(&sb, &doc));

        // -------------------------------------------------------------------------------------------------------------
        revise_begin(&revise, &rev_doc, &doc);
        revise_iterator_open(&rev_it, &revise);
        has_next = arr_it_next(&rev_it);
        ASSERT_TRUE(has_next);
        internal_arr_it_remove(&rev_it);
        has_next = arr_it_next(&rev_it);
        ASSERT_TRUE(has_next);
        field_e next_type;
        arr_it_field_type(&next_type, &rev_it);
        ASSERT_EQ(next_type, FIELD_ARRAY_UNSORTED_MULTISET);
        revise_end(&revise);
        // -------------------------------------------------------------------------------------------------------------

        char *json_2 = strdup(rec_to_json(&sb, &rev_doc));

        // -------------------------------------------------------------------------------------------------------------


        // printf("BEFORE\t'%s'\nAFTER\t'%s'\n", json_1, json_2);

        ASSERT_TRUE(strcmp(json_1, "[[1, 2, 3], [4, 5, 6]]") == 0);
        ASSERT_TRUE(strcmp(json_2, "[4, 5, 6]") == 0);

        str_buf_drop(&sb);
        rec_drop(&doc);
        rec_drop(&rev_doc);
        free(json_1);
        free(json_2);
}

TEST(CarbonTest, CarbonRemoveLastArray)
{
        rec doc, rev_doc;
        rec_new context;
        rev revise;
        arr_it rev_it;
        str_buf sb;
        bool has_next;
        str_buf_create(&sb);

        arr_state state;
        insert *array_ins;

        // -------------------------------------------------------------------------------------------------------------
        insert *ins = rec_create_begin(&context, &doc, KEY_NOKEY, KEEP);

        array_ins = insert_array_begin(&state, ins, 10);
        insert_u8(array_ins, 1);
        insert_u8(array_ins, 2);
        insert_u8(array_ins, 3);
        insert_array_end(&state);

        array_ins = insert_array_begin(&state, ins, 10);
        insert_u8(array_ins, 4);
        insert_u8(array_ins, 5);
        insert_u8(array_ins, 6);
        insert_array_end(&state);

        rec_create_end(&context);
        // -------------------------------------------------------------------------------------------------------------

        char *json_1 = strdup(rec_to_json(&sb, &doc));

        // -------------------------------------------------------------------------------------------------------------
        revise_begin(&revise, &rev_doc, &doc);
        revise_iterator_open(&rev_it, &revise);
        has_next = arr_it_next(&rev_it);
        ASSERT_TRUE(has_next);
        has_next = arr_it_next(&rev_it);
        ASSERT_TRUE(has_next);
        internal_arr_it_remove(&rev_it);
        has_next = arr_it_next(&rev_it);
        ASSERT_FALSE(has_next);
        revise_end(&revise);
        // -------------------------------------------------------------------------------------------------------------

        char *json_2 = strdup(rec_to_json(&sb, &rev_doc));

        // -------------------------------------------------------------------------------------------------------------


        // printf("BEFORE\t'%s'\nAFTER\t'%s'\n", json_1, json_2);

        ASSERT_TRUE(strcmp(json_1, "[[1, 2, 3], [4, 5, 6]]") == 0);
        ASSERT_TRUE(strcmp(json_2, "[1, 2, 3]") == 0);

        str_buf_drop(&sb);
        rec_drop(&doc);
        rec_drop(&rev_doc);
        free(json_1);
        free(json_2);
}

TEST(CarbonTest, CarbonRemoveMiddleArray)
{
        rec doc, rev_doc;
        rec_new context;
        rev revise;
        arr_it rev_it;
        str_buf sb;
        bool has_next;
        str_buf_create(&sb);

        arr_state state;
        insert *array_ins;

        // -------------------------------------------------------------------------------------------------------------
        insert *ins = rec_create_begin(&context, &doc, KEY_NOKEY, KEEP);

        array_ins = insert_array_begin(&state, ins, 10);
        insert_u8(array_ins, 1);
        insert_u8(array_ins, 2);
        insert_u8(array_ins, 3);
        insert_array_end(&state);

        array_ins = insert_array_begin(&state, ins, 10);
        insert_u8(array_ins, 4);
        insert_u8(array_ins, 5);
        insert_u8(array_ins, 6);
        insert_array_end(&state);

        array_ins = insert_array_begin(&state, ins, 10);
        insert_u8(array_ins, 7);
        insert_u8(array_ins, 8);
        insert_u8(array_ins, 9);
        insert_array_end(&state);

        rec_create_end(&context);
        // -------------------------------------------------------------------------------------------------------------

        char *json_1 = strdup(rec_to_json(&sb, &doc));

        // -------------------------------------------------------------------------------------------------------------
        revise_begin(&revise, &rev_doc, &doc);
        revise_iterator_open(&rev_it, &revise);
        has_next = arr_it_next(&rev_it);
        ASSERT_TRUE(has_next);
        has_next = arr_it_next(&rev_it);
        ASSERT_TRUE(has_next);
        internal_arr_it_remove(&rev_it);
        has_next = arr_it_next(&rev_it);
        ASSERT_TRUE(has_next);
        field_e type;
        arr_it_field_type(&type, &rev_it);
        ASSERT_EQ(type, FIELD_ARRAY_UNSORTED_MULTISET);
        has_next = arr_it_next(&rev_it);
        ASSERT_FALSE(has_next);
        revise_end(&revise);
        // -------------------------------------------------------------------------------------------------------------

        char *json_2 = strdup(rec_to_json(&sb, &rev_doc));

        // -------------------------------------------------------------------------------------------------------------

        // printf("BEFORE\t'%s'\nAFTER\t'%s'\n", json_1, json_2);

        ASSERT_TRUE(strcmp(json_1, "[[1, 2, 3], [4, 5, 6], [7, 8, 9]]") == 0);
        ASSERT_TRUE(strcmp(json_2, "[[1, 2, 3], [7, 8, 9]]") == 0);

        str_buf_drop(&sb);
        rec_drop(&doc);
        rec_drop(&rev_doc);
        free(json_1);
        free(json_2);
}

TEST(CarbonTest, CarbonColumnRemoveTest)
{
        rec doc, rev_doc;
        rec_new context;
        rev revise;
        arr_it rev_it;
        str_buf sb;
        bool has_next;
        str_buf_create(&sb);
        bool status;
        const u16 *values;

        col_state state;
        insert *array_ins;

        // -------------------------------------------------------------------------------------------------------------
        insert *ins = rec_create_begin(&context, &doc, KEY_NOKEY, KEEP);

        array_ins = insert_column_begin(&state, ins, COLUMN_U16, 10);
        insert_u16(array_ins, 1);
        insert_u16(array_ins, 2);
        insert_u16(array_ins, 3);
        insert_column_end(&state);

        rec_create_end(&context);
        // -------------------------------------------------------------------------------------------------------------

        char *json_1 = strdup(rec_to_json(&sb, &doc));

        // -------------------------------------------------------------------------------------------------------------

        revise_begin(&revise, &rev_doc, &doc);
        revise_iterator_open(&rev_it, &revise);
        has_next = arr_it_next(&rev_it);
        ASSERT_TRUE(has_next);
        col_it cit;
        item_get_column(&cit, &(rev_it.item));
        field_e type;
        u32 num_elems = COL_IT_VALUES_INFO(&type, &cit);
        ASSERT_EQ(type, FIELD_COLUMN_U16_UNSORTED_MULTISET);
        ASSERT_EQ(num_elems, 3u);

        status = col_it_remove(&cit, 1);
        ASSERT_TRUE(status);
        num_elems = COL_IT_VALUES_INFO(&type, &cit);
        ASSERT_EQ(type, FIELD_COLUMN_U16_UNSORTED_MULTISET);
        ASSERT_EQ(num_elems, 2u);
        values = COL_IT_U16_VALUES(&num_elems, &cit);
        ASSERT_EQ(values[0], 1);
        ASSERT_EQ(values[1], 3);

        char *json_2 = strdup(rec_to_json(&sb, &rev_doc));

        status = col_it_remove(&cit, 0);
        ASSERT_TRUE(status);
        num_elems = COL_IT_VALUES_INFO(&type, &cit);
        ASSERT_EQ(type, FIELD_COLUMN_U16_UNSORTED_MULTISET);
        ASSERT_EQ(num_elems, 1u);
        values = COL_IT_U16_VALUES(&num_elems, &cit);
        ASSERT_EQ(values[0], 3);

        char *json_3 = strdup(rec_to_json(&sb, &rev_doc));

        status = col_it_remove(&cit, 0);
        ASSERT_TRUE(status);
        num_elems = COL_IT_VALUES_INFO(&type, &cit);
        ASSERT_EQ(type, FIELD_COLUMN_U16_UNSORTED_MULTISET);
        ASSERT_EQ(num_elems, 0u);

        char *json_4 = strdup(rec_to_json(&sb, &rev_doc));

        revise_end(&revise);

        // -------------------------------------------------------------------------------------------------------------


        // printf(">> %s\n", json_1);
        // printf(">> %s\n", json_2);
        // printf(">> %s\n", json_3);
        // printf(">> %s\n", json_4);

        ASSERT_TRUE(strcmp(json_1, "[1, 2, 3]") == 0);
        ASSERT_TRUE(strcmp(json_2, "[1, 3]") == 0);
        ASSERT_TRUE(strcmp(json_3, "[3]") == 0);
        ASSERT_TRUE(strcmp(json_4, "[]") == 0);

        str_buf_drop(&sb);
        rec_drop(&doc);
        rec_drop(&rev_doc);
        free(json_1);
        free(json_2);
        free(json_3);
        free(json_4);
}

TEST(CarbonTest, CarbonRemoveComplexTest)
{
        rec doc, rev_doc, rev_doc2, rev_doc3, rev_doc4, rev_doc5, rev_doc6, rev_doc7, rev_doc8, rev_doc9,
                rev_doc10, rev_doc11, rev_doc12, rev_doc13, rev_doc14;
        rec_new context;
        str_buf sb;
        str_buf_create(&sb);

        arr_state state, state2, state3;
        col_state cstate;
        insert *array_ins, *array_ins2, *array_ins3, *column_ins;

        // -------------------------------------------------------------------------------------------------------------
        insert *ins = rec_create_begin(&context, &doc, KEY_NOKEY, OPTIMIZE);

        insert_u8(ins, 1);
        insert_string(ins, "Hello");
        insert_u16(ins, 2);
        insert_u32(ins, 3);
        insert_u64(ins, 3);
        insert_string(ins, "World");

        array_ins = insert_array_begin(&state, ins, 10);
        insert_array_end(&state);

        array_ins = insert_array_begin(&state, ins, 10);
        insert_u8(array_ins, 4);
        insert_string(array_ins, "Fox!");
        insert_u8(array_ins, 6);
        insert_array_end(&state);

        array_ins = insert_array_begin(&state, ins, 10);
        array_ins2 = insert_array_begin(&state2, array_ins, 10);
        insert_array_end(&state2);
        array_ins2 = insert_array_begin(&state2, array_ins, 10);
        insert_u8(array_ins2, 4);
        insert_array_end(&state2);
        insert_null(array_ins);
        array_ins2 = insert_array_begin(&state2, array_ins, 10);
        insert_string(array_ins2, "Dog!");
        array_ins3 = insert_array_begin(&state3, array_ins2, 10);
        insert_array_end(&state3);
        array_ins3 = insert_array_begin(&state3, array_ins2, 10);
        column_ins = insert_column_begin(&cstate, array_ins3, COLUMN_U8, 10);

        insert_u8(column_ins, 41);
        insert_u8(column_ins, 42);
        insert_u8(column_ins, 43);
        insert_column_end(&cstate);

        insert_array_end(&state3);
        array_ins3 = insert_array_begin(&state3, array_ins2, 10);
        insert_array_end(&state3);

        insert_array_end(&state2);
        insert_array_end(&state);

        rec_create_end(&context);
        // -------------------------------------------------------------------------------------------------------------

        char *json_1 = strdup(rec_to_json(&sb, &doc));
        ASSERT_TRUE(strcmp(json_1, "[1, \"Hello\", 2, 3, 3, \"World\", [], [4, \"Fox!\", 6], [[], [4], null, [\"Dog!\", [], [[41, 42, 43]], []]]]") == 0);

        // -------------------------------------------------------------------------------------------------------------

        revise_remove_one("3", &rev_doc, &doc);
        char *json_2 = strdup(rec_to_json(&sb, &rev_doc));
        ASSERT_TRUE(strcmp(json_2, "[1, \"Hello\", 2, 3, \"World\", [], [4, \"Fox!\", 6], [[], [4], null, [\"Dog!\", [], [[41, 42, 43]], []]]]") == 0);

        // -------------------------------------------------------------------------------------------------------------

        revise_remove_one("5", &rev_doc2, &rev_doc);
        char *json_3 = strdup(rec_to_json(&sb, &rev_doc2));
        ASSERT_TRUE(strcmp(json_3, "[1, \"Hello\", 2, 3, \"World\", [4, \"Fox!\", 6], [[], [4], null, [\"Dog!\", [], [[41, 42, 43]], []]]]") == 0);

        // -------------------------------------------------------------------------------------------------------------

        revise_remove_one("5.1", &rev_doc3, &rev_doc2);
        char *json_4 = strdup(rec_to_json(&sb, &rev_doc3));
        ASSERT_TRUE(strcmp(json_4, "[1, \"Hello\", 2, 3, \"World\", [4, 6], [[], [4], null, [\"Dog!\", [], [[41, 42, 43]], []]]]") == 0);

        // -------------------------------------------------------------------------------------------------------------

        revise_remove_one("6.0", &rev_doc4, &rev_doc3);
        char *json_5 = strdup(rec_to_json(&sb, &rev_doc4));
        ASSERT_TRUE(strcmp(json_5, "[1, \"Hello\", 2, 3, \"World\", [4, 6], [[4], null, [\"Dog!\", [], [[41, 42, 43]], []]]]") == 0);

        // -------------------------------------------------------------------------------------------------------------

        revise_remove_one("5", &rev_doc5, &rev_doc4);
        char *json_6 = strdup(rec_to_json(&sb, &rev_doc5));
        ASSERT_TRUE(strcmp(json_6, "[1, \"Hello\", 2, 3, \"World\", [[4], null, [\"Dog!\", [], [[41, 42, 43]], []]]]") == 0);

        // -------------------------------------------------------------------------------------------------------------

        revise_remove_one("0", &rev_doc6, &rev_doc5);
        revise_remove_one("1", &rev_doc7, &rev_doc6);
        revise_remove_one("0", &rev_doc8, &rev_doc7);
        revise_remove_one("1", &rev_doc9, &rev_doc8);
        revise_remove_one("0", &rev_doc10, &rev_doc9);
        char *json_11 = strdup(rec_to_json(&sb, &rev_doc10));
        ASSERT_TRUE(strcmp(json_11, "[[4], null, [\"Dog!\", [], [[41, 42, 43]], []]]") == 0);

        // -------------------------------------------------------------------------------------------------------------

        revise_remove_one("0.2.2.0", &rev_doc11, &rev_doc10);

        char *json_12 = strdup(rec_to_json(&sb, &rev_doc11));
        ASSERT_TRUE(strcmp(json_12, "[[4], null, [\"Dog!\", [], [], []]]") == 0);

        // -------------------------------------------------------------------------------------------------------------

        revise_remove_one("0.2.2", &rev_doc12, &rev_doc11);

        char *json_13 = strdup(rec_to_json(&sb, &rev_doc12));
        ASSERT_TRUE(strcmp(json_13, "[[4], null, [\"Dog!\", [], []]]") == 0);

        // -------------------------------------------------------------------------------------------------------------

        revise_remove_one("0.2", &rev_doc13, &rev_doc12);

        char *json_14 = strdup(rec_to_json(&sb, &rev_doc13));
        ASSERT_TRUE(strcmp(json_14, "[[4], null]") == 0);

        // -------------------------------------------------------------------------------------------------------------

        revise_remove_one("0", &rev_doc14, &rev_doc13);

        char *json_15 = strdup(rec_to_json(&sb, &rev_doc14));
        ASSERT_TRUE(strcmp(json_15, "[]") == 0);


        str_buf_drop(&sb);
        rec_drop(&doc);
        rec_drop(&rev_doc);
        rec_drop(&rev_doc2);
        rec_drop(&rev_doc3);
        rec_drop(&rev_doc4);
        rec_drop(&rev_doc5);
        rec_drop(&rev_doc6);
        rec_drop(&rev_doc7);
        rec_drop(&rev_doc8);
        rec_drop(&rev_doc9);
        rec_drop(&rev_doc10);
        rec_drop(&rev_doc11);
        rec_drop(&rev_doc12);
        rec_drop(&rev_doc13);
        rec_drop(&rev_doc14);
        free(json_1);
        free(json_2);
        free(json_3);
        free(json_4);
        free(json_5);
        free(json_6);
        free(json_11);
        free(json_12);
        free(json_13);
        free(json_14);
        free(json_15);
}

TEST(CarbonTest, CarbonUpdateMixedFixedTypesTypeChangeSimple)
{
        rec doc, rev_doc, rev_doc2;
        rev revise;
        arr_it it;
        insert in;
        str_buf sb;
        const char *json;

        str_buf_create(&sb);
        rec_create_empty(&doc, LIST_UNSORTED_MULTISET, KEY_NOKEY);

        // -------------------------------------------------------------------------------------------------------------

        revise_begin(&revise, &rev_doc, &doc);
        revise_iterator_open(&it, &revise);
        arr_it_insert_begin(&in, &it);

        insert_u8(&in, 1);
        insert_i64(&in, -42);
        insert_float(&in, 23);

        arr_it_insert_end(&in);
        revise_end(&revise);


        json = rec_to_json(&sb, &rev_doc);
        ASSERT_TRUE(strcmp(json, "[1, -42, 23.00]") == 0);

        // -------------------------------------------------------------------------------------------------------------

        revise_begin(&revise, &rev_doc2, &rev_doc);
        update_set_u32(&revise, "1", 1024);
        revise_end(&revise);


        json = rec_to_json(&sb, &rev_doc2);
        ASSERT_TRUE(strcmp(json, "[1, 1024, 23.00]") == 0);

        // -------------------------------------------------------------------------------------------------------------

        str_buf_drop(&sb);

        rec_drop(&doc);
        rec_drop(&rev_doc);
        rec_drop(&rev_doc2);
}


TEST(CarbonTest, CarbonShrinkIssueFix)
{
        rec doc;
        rec_new context;

        // -------------------------------------------------------------------------------------------------------------
        insert *ins = rec_create_begin(&context, &doc, KEY_NOKEY, OPTIMIZE);

        insert_string(ins, "Hello");
        insert_string(ins, "World");

        rec_create_end(&context);

        // -------------------------------------------------------------------------------------------------------------

        str_buf sb;
        str_buf_create(&sb);

        ASSERT_TRUE(strcmp(rec_to_json(&sb, &doc), "[\"Hello\", \"World\"]") == 0);

        str_buf_drop(&sb);
        rec_drop(&doc);
}

TEST(CarbonTest, CarbonKeyTypeNoKey)
{
        rec doc;
        rec_new context;

        // -------------------------------------------------------------------------------------------------------------
        insert *ins = rec_create_begin(&context, &doc, KEY_NOKEY, OPTIMIZE);

        insert_string(ins, "Hello");
        insert_string(ins, "World");

        rec_create_end(&context);

        // -------------------------------------------------------------------------------------------------------------

        str_buf sb;
        str_buf_create(&sb);

        // rec_print(stdout, &doc);
        ASSERT_TRUE(strcmp(rec_to_json(&sb, &doc), "[\"Hello\", \"World\"]") == 0);

        str_buf_drop(&sb);
        rec_drop(&doc);
}

TEST(CarbonTest, CarbonKeyTypeNoKeyNoRevInc)
{
        rec doc, rev_doc;
        rec_new context;
        rev revise;
        u64 rev_old, rev_new;

        // -------------------------------------------------------------------------------------------------------------
        insert *ins = rec_create_begin(&context, &doc, KEY_NOKEY, OPTIMIZE);

        insert_string(ins, "Hello");
        insert_string(ins, "World");

        rec_create_end(&context);

        // -------------------------------------------------------------------------------------------------------------

        rec_commit_hash(&rev_old, &doc);

        revise_begin(&revise, &rev_doc, &doc);
        revise_end(&revise);

        rec_commit_hash(&rev_new, &rev_doc);

        ASSERT_EQ(rev_old, 0U);
        ASSERT_EQ(rev_new, rev_old);

        rec_drop(&doc);
        rec_drop(&rev_doc);
}

TEST(CarbonTest, CarbonKeyTypeAutoKey)
{
        rec doc;
        rec_new context;

        // -------------------------------------------------------------------------------------------------------------
        insert *ins = rec_create_begin(&context, &doc, KEY_NOKEY, OPTIMIZE);

        insert_string(ins, "Hello");
        insert_string(ins, "World");

        rec_create_end(&context);

        // -------------------------------------------------------------------------------------------------------------

        str_buf sb;
        str_buf_create(&sb);

        // rec_print(stdout, &doc);
        ASSERT_TRUE(strcmp(rec_to_json(&sb, &doc), "[\"Hello\", \"World\"]") == 0);

        str_buf_drop(&sb);
        rec_drop(&doc);
}

TEST(CarbonTest, CarbonKeyTypeAutoKeyRevInc)
{
        rec doc, rev_doc;
        rec_new context;
        rev revise;
        u64 rev_old, rev_new;

        // -------------------------------------------------------------------------------------------------------------
        insert *ins = rec_create_begin(&context, &doc, KEY_NOKEY, OPTIMIZE);

        insert_string(ins, "Hello");
        insert_string(ins, "World");

        rec_create_end(&context);

        // -------------------------------------------------------------------------------------------------------------

        rec_commit_hash(&rev_old, &doc);

        revise_begin(&revise, &rev_doc, &doc);
        revise_end(&revise);

        rec_commit_hash(&rev_new, &rev_doc);

        ASSERT_EQ(rev_old, rev_new);

        rec_drop(&doc);
        rec_drop(&rev_doc);
}

TEST(CarbonTest, CarbonKeyTypeAutoKeyUpdate)
{
        rec doc, rev_doc;
        rec_new context;
        rev revise;
        unique_id_t id, id_read;

        // -------------------------------------------------------------------------------------------------------------
        insert *ins = rec_create_begin(&context, &doc, KEY_AUTOKEY, OPTIMIZE);

        insert_string(ins, "Hello");
        insert_string(ins, "World");

        rec_create_end(&context);

        // -------------------------------------------------------------------------------------------------------------


        revise_begin(&revise, &rev_doc, &doc);
        revise_key_generate(&id, &revise);
        revise_end(&revise);

        rec_key_unsigned_value(&id_read, &rev_doc);
        ASSERT_NE(id, 0U);
        ASSERT_EQ(id, id_read);

        // rec_print(stdout, &rev_doc);

        rec_drop(&doc);
        rec_drop(&rev_doc);
}

TEST(CarbonTest, CarbonKeyTypeUnsignedKeyUpdate)
{
        rec doc, rev_doc;
        rec_new context;
        rev revise;
        str_buf sb;

        str_buf_create(&sb);

        u64 id_read;

        // -------------------------------------------------------------------------------------------------------------
        insert *ins = rec_create_begin(&context, &doc, KEY_UKEY, OPTIMIZE);

        insert_string(ins, "Hello");
        insert_string(ins, "World");

        rec_create_end(&context);

        // -------------------------------------------------------------------------------------------------------------


        revise_begin(&revise, &rev_doc, &doc);
        revise_key_set_unsigned(&revise, 42);
        revise_end(&revise);

        rec_key_unsigned_value(&id_read, &rev_doc);
        ASSERT_EQ(id_read, 42U);

        rec_drop(&doc);
        rec_drop(&rev_doc);
        str_buf_drop(&sb);
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}