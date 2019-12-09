#include <gtest/gtest.h>
#include <printf.h>

#include <jakson/jakson.h>



TEST(CarbonTest, CarbonUpdateU8Simple)
{
        rec doc, rev_doc, rev_doc2, rev_doc3, rev_doc4;
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

        insert_u8(&in, 'X');

        arr_it_insert_end(&in);
        revise_iterator_close(&it);
        revise_end(&revise);


        json = rec_to_json(&sb, &rev_doc);
        // printf("JSON (rev1): %s\n", json);

        // -------------------------------------------------------------------------------------------------------------

        revise_begin(&revise, &rev_doc2, &rev_doc);
        revise_iterator_open(&it, &revise);
        arr_it_insert_begin(&in, &it);

        update_set_u8(&revise, "0", 'Y');

        arr_it_insert_end(&in);
        revise_iterator_close(&it);
        revise_end(&revise);


        json = rec_to_json(&sb, &rev_doc2);
        // printf("JSON (rev2): %s\n", json);
        ASSERT_TRUE(strcmp(json, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [89]}") == 0);

        // -------------------------------------------------------------------------------------------------------------

        revise_begin(&revise, &rev_doc3, &rev_doc2);
        revise_iterator_open(&it, &revise);
        arr_it_insert_begin(&in, &it);

        insert_u8(&in, 'A');
        insert_u8(&in, 'B');
        update_set_u8(&revise, "2", 'C');

        arr_it_insert_end(&in);
        revise_iterator_close(&it);
        revise_end(&revise);


        json = rec_to_json(&sb, &rev_doc3);
        // printf("JSON (rev3): %s\n", json);
        ASSERT_TRUE(strcmp(json, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [65, 66, 67]}") == 0);

        // -------------------------------------------------------------------------------------------------------------

        revise_begin(&revise, &rev_doc4, &rev_doc3);
        revise_iterator_open(&it, &revise);
        arr_it_insert_begin(&in, &it);

        update_set_u8(&revise, "0", 1);
        update_set_u8(&revise, "1", 2);
        update_set_u8(&revise, "2", 3);

        arr_it_insert_end(&in);
        revise_iterator_close(&it);
        revise_end(&revise);


        json = rec_to_json(&sb, &rev_doc4);
        // printf("JSON (rev4): %s\n", json);
        ASSERT_TRUE(strcmp(json, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [1, 2, 3]}") == 0);

        // -------------------------------------------------------------------------------------------------------------

        str_buf_drop(&sb);

        rec_drop(&doc);
        rec_drop(&rev_doc);
        rec_drop(&rev_doc2);
        rec_drop(&rev_doc3);
        rec_drop(&rev_doc4);
}

TEST(CarbonTest, CarbonUpdateMixedFixedTypesSimple)
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
        revise_iterator_close(&it);
        revise_end(&revise);


        json = rec_to_json(&sb, &rev_doc);
        // printf("JSON (rev1): %s\n", json);
        ASSERT_TRUE(strcmp(json, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [1, -42, 23.00]}") == 0);

        // -------------------------------------------------------------------------------------------------------------

        revise_begin(&revise, &rev_doc2, &rev_doc);
        revise_iterator_open(&it, &revise);
        arr_it_insert_begin(&in, &it);

        update_set_i64(&revise, "1", 1024);

        arr_it_insert_end(&in);
        revise_iterator_close(&it);
        revise_end(&revise);


        json = rec_to_json(&sb, &rev_doc2);
        // printf("JSON (rev2): %s\n", json);
        ASSERT_TRUE(strcmp(json, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [1, 1024, 23.00]}") == 0);

        // -------------------------------------------------------------------------------------------------------------

        find find;
        field_e result_type;
        u64 result;
        i64 resulti64;
        float resultfloat;

        find_begin(&find, "0", &rev_doc2);
        ASSERT_TRUE(find_has_result(&find));
        find_result_type(&result_type, &find);
        ASSERT_EQ(result_type, FIELD_NUMBER_U8);
        find_result_unsigned(&result, &find);
        find_end(&find);
        ASSERT_EQ(result, 1U);

        find_begin(&find, "1", &rev_doc2);
        ASSERT_TRUE(find_has_result(&find));
        find_result_type(&result_type, &find);
        ASSERT_EQ(result_type, FIELD_NUMBER_I64);
        find_result_signed(&resulti64, &find);
        find_end(&find);
        ASSERT_EQ(resulti64, 1024U);

        find_begin(&find, "2", &rev_doc2);
        ASSERT_TRUE(find_has_result(&find));
        find_result_type(&result_type, &find);
        ASSERT_EQ(result_type, FIELD_NUMBER_FLOAT);
        find_result_float(&resultfloat, &find);
        find_end(&find);
        ASSERT_GE(resultfloat, 22.9f);
        ASSERT_LE(resultfloat, 24.0f);

        // -------------------------------------------------------------------------------------------------------------

        str_buf_drop(&sb);

        rec_drop(&doc);
        rec_drop(&rev_doc);
        rec_drop(&rev_doc2);

}

TEST(CarbonTest, CarbonRemoveConstantsToEmpty)
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

        insert_null(ins);

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
        ASSERT_FALSE(has_next);
        revise_iterator_close(&rev_it);
        revise_end(&revise);
        // -------------------------------------------------------------------------------------------------------------

        char *json_2 = strdup(rec_to_json(&sb, &rev_doc));

        // -------------------------------------------------------------------------------------------------------------


        // printf("BEFORE\t'%s'\nAFTER\t'%s'\n", json_1, json_2);

        ASSERT_TRUE(strcmp(json_1, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [null]}") == 0);
        ASSERT_TRUE(strcmp(json_2, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": []}") == 0);

        str_buf_drop(&sb);
        rec_drop(&doc);
        rec_drop(&rev_doc);
        free(json_1);
        free(json_2);
}

TEST(CarbonTest, CarbonRemoveFirstConstants)
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

        insert_true(ins);
        insert_false(ins);

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
        ASSERT_EQ(next_type, FIELD_FALSE);
        revise_iterator_close(&rev_it);
        revise_end(&revise);
        // -------------------------------------------------------------------------------------------------------------

        char *json_2 = strdup(rec_to_json(&sb, &rev_doc));

        // -------------------------------------------------------------------------------------------------------------


        // printf("BEFORE\t'%s'\nAFTER\t'%s'\n", json_1, json_2);

        ASSERT_TRUE(strcmp(json_1, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [true, false]}") == 0);
        ASSERT_TRUE(strcmp(json_2, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [false]}") == 0);

        str_buf_drop(&sb);
        rec_drop(&doc);
        rec_drop(&rev_doc);
        free(json_1);
        free(json_2);
}

TEST(CarbonTest, CarbonRemoveLastConstants)
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

        insert_true(ins);
        insert_false(ins);

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
        revise_iterator_close(&rev_it);
        revise_end(&revise);
        // -------------------------------------------------------------------------------------------------------------

        char *json_2 = strdup(rec_to_json(&sb, &rev_doc));

        // -------------------------------------------------------------------------------------------------------------


        // printf("BEFORE\t'%s'\nAFTER\t'%s'\n", json_1, json_2);

        ASSERT_TRUE(strcmp(json_1, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [true, false]}") == 0);
        ASSERT_TRUE(strcmp(json_2, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [true]}") == 0);

        str_buf_drop(&sb);
        rec_drop(&doc);
        rec_drop(&rev_doc);
        free(json_1);
        free(json_2);
}

TEST(CarbonTest, CarbonRemoveMiddleConstants)
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

        insert_true(ins);
        insert_null(ins);
        insert_false(ins);

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
        ASSERT_EQ(type, FIELD_FALSE);
        has_next = arr_it_next(&rev_it);
        ASSERT_FALSE(has_next);
        revise_iterator_close(&rev_it);
        revise_end(&revise);
        // -------------------------------------------------------------------------------------------------------------

        char *json_2 = strdup(rec_to_json(&sb, &rev_doc));

        // -------------------------------------------------------------------------------------------------------------

        // printf("BEFORE\t'%s'\nAFTER\t'%s'\n", json_1, json_2);

        ASSERT_TRUE(strcmp(json_1, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [true, null, false]}") == 0);
        ASSERT_TRUE(strcmp(json_2, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [true, false]}") == 0);

        str_buf_drop(&sb);
        rec_drop(&doc);
        rec_drop(&rev_doc);
        free(json_1);
        free(json_2);
}

TEST(CarbonTest, CarbonRemoveNumberToEmpty)
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

        insert_u8(ins, 42);

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
        ASSERT_FALSE(has_next);
        revise_iterator_close(&rev_it);
        revise_end(&revise);
        // -------------------------------------------------------------------------------------------------------------

        char *json_2 = strdup(rec_to_json(&sb, &rev_doc));

        // -------------------------------------------------------------------------------------------------------------


        // printf("BEFORE\t'%s'\nAFTER\t'%s'\n", json_1, json_2);

        ASSERT_TRUE(strcmp(json_1, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [42]}") == 0);
        ASSERT_TRUE(strcmp(json_2, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": []}") == 0);

        str_buf_drop(&sb);
        rec_drop(&doc);
        rec_drop(&rev_doc);
        free(json_1);
        free(json_2);
}

TEST(CarbonTest, CarbonRemoveFirstNumber)
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

        insert_u8(ins, 42);
        insert_u32(ins, 23);

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
        ASSERT_EQ(next_type, FIELD_NUMBER_U32);
        revise_iterator_close(&rev_it);
        revise_end(&revise);
        // -------------------------------------------------------------------------------------------------------------

        char *json_2 = strdup(rec_to_json(&sb, &rev_doc));

        // -------------------------------------------------------------------------------------------------------------


        // printf("BEFORE\t'%s'\nAFTER\t'%s'\n", json_1, json_2);

        ASSERT_TRUE(strcmp(json_1, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [42, 23]}") == 0);
        ASSERT_TRUE(strcmp(json_2, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [23]}") == 0);

        str_buf_drop(&sb);
        rec_drop(&doc);
        rec_drop(&rev_doc);
        free(json_1);
        free(json_2);
}

TEST(CarbonTest, CarbonRemoveLastNumber)
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

        insert_u8(ins, 42);
        insert_u32(ins, 23);

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
        revise_iterator_close(&rev_it);
        revise_end(&revise);
        // -------------------------------------------------------------------------------------------------------------

        char *json_2 = strdup(rec_to_json(&sb, &rev_doc));

        // -------------------------------------------------------------------------------------------------------------


        // printf("BEFORE\t'%s'\nAFTER\t'%s'\n", json_1, json_2);

        ASSERT_TRUE(strcmp(json_1, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [42, 23]}") == 0);
        ASSERT_TRUE(strcmp(json_2, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [42]}") == 0);

        str_buf_drop(&sb);
        rec_drop(&doc);
        rec_drop(&rev_doc);
        free(json_1);
        free(json_2);
}

TEST(CarbonTest, CarbonRemoveMiddleNumber)
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

        insert_u8(ins, 42);
        insert_u16(ins, 21);
        insert_u32(ins, 23);

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
        ASSERT_EQ(type, FIELD_NUMBER_U32);
        has_next = arr_it_next(&rev_it);
        ASSERT_FALSE(has_next);
        revise_iterator_close(&rev_it);
        revise_end(&revise);
        // -------------------------------------------------------------------------------------------------------------

        char *json_2 = strdup(rec_to_json(&sb, &rev_doc));

        // -------------------------------------------------------------------------------------------------------------

        // printf("BEFORE\t'%s'\nAFTER\t'%s'\n", json_1, json_2);

        ASSERT_TRUE(strcmp(json_1, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [42, 21, 23]}") == 0);
        ASSERT_TRUE(strcmp(json_2, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [42, 23]}") == 0);

        str_buf_drop(&sb);
        rec_drop(&doc);
        rec_drop(&rev_doc);
        free(json_1);
        free(json_2);
}


TEST(CarbonTest, CarbonRemoveStringToEmpty)
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

        insert_string(ins, "Hello");

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
        ASSERT_FALSE(has_next);
        revise_iterator_close(&rev_it);
        revise_end(&revise);
        // -------------------------------------------------------------------------------------------------------------

        char *json_2 = strdup(rec_to_json(&sb, &rev_doc));

        // -------------------------------------------------------------------------------------------------------------


        // printf("BEFORE\t'%s'\nAFTER\t'%s'\n", json_1, json_2);

        ASSERT_TRUE(strcmp(json_1, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [\"Hello\"]}") == 0);
        ASSERT_TRUE(strcmp(json_2, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": []}") == 0);

        str_buf_drop(&sb);
        rec_drop(&doc);
        rec_drop(&rev_doc);
        free(json_1);
        free(json_2);
}

TEST(CarbonTest, CarbonRemoveFirstString)
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

        insert_string(ins, "Hello");
        insert_string(ins, "World");

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
        ASSERT_EQ(next_type, FIELD_STRING);
        revise_iterator_close(&rev_it);
        revise_end(&revise);
        // -------------------------------------------------------------------------------------------------------------

        char *json_2 = strdup(rec_to_json(&sb, &rev_doc));

        // -------------------------------------------------------------------------------------------------------------


        // printf("BEFORE\t'%s'\nAFTER\t'%s'\n", json_1, json_2);

        ASSERT_TRUE(strcmp(json_1, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [\"Hello\", \"World\"]}") == 0);
        ASSERT_TRUE(strcmp(json_2, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [\"World\"]}") == 0);

        str_buf_drop(&sb);
        rec_drop(&doc);
        rec_drop(&rev_doc);
        free(json_1);
        free(json_2);
}

TEST(CarbonTest, CarbonRemoveLastString)
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

        insert_string(ins, "Hello");
        insert_string(ins, "World");

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
        revise_iterator_close(&rev_it);
        revise_end(&revise);
        // -------------------------------------------------------------------------------------------------------------

        char *json_2 = strdup(rec_to_json(&sb, &rev_doc));

        // -------------------------------------------------------------------------------------------------------------


        // printf("BEFORE\t'%s'\nAFTER\t'%s'\n", json_1, json_2);

        ASSERT_TRUE(strcmp(json_1, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [\"Hello\", \"World\"]}") == 0);
        ASSERT_TRUE(strcmp(json_2, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [\"Hello\"]}") == 0);

        str_buf_drop(&sb);
        rec_drop(&doc);
        rec_drop(&rev_doc);
        free(json_1);
        free(json_2);
}

TEST(CarbonTest, CarbonRemoveMiddleString)
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

        insert_string(ins, "Plato");
        insert_string(ins, "Kant");
        insert_string(ins, "Nietzsche");

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
        ASSERT_EQ(type, FIELD_STRING);
        has_next = arr_it_next(&rev_it);
        ASSERT_FALSE(has_next);
        revise_iterator_close(&rev_it);
        revise_end(&revise);
        // -------------------------------------------------------------------------------------------------------------

        char *json_2 = strdup(rec_to_json(&sb, &rev_doc));

        // -------------------------------------------------------------------------------------------------------------

        // printf("BEFORE\t'%s'\nAFTER\t'%s'\n", json_1, json_2);

        ASSERT_TRUE(strcmp(json_1, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [\"Plato\", \"Kant\", \"Nietzsche\"]}") == 0);
        ASSERT_TRUE(strcmp(json_2, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [\"Plato\", \"Nietzsche\"]}") == 0);

        str_buf_drop(&sb);
        rec_drop(&doc);
        rec_drop(&rev_doc);
        free(json_1);
        free(json_2);
}





TEST(CarbonTest, CarbonRemoveBinaryToEmpty)
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

        const char *data = "This report, by its very length, defends itself against the risk of being read.";
        insert_binary(ins, data, strlen(data), "txt", NULL);

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
        ASSERT_FALSE(has_next);
        revise_iterator_close(&rev_it);
        revise_end(&revise);
        // -------------------------------------------------------------------------------------------------------------

        char *json_2 = strdup(rec_to_json(&sb, &rev_doc));

        // -------------------------------------------------------------------------------------------------------------


        //printf("\nBEFORE\t'%s'\nAFTER\t'%s'\n", json_1, json_2);

        ASSERT_TRUE(strcmp(json_1, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [{ \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"VGhpcyByZXBvcnQsIGJ5IGl0cyB2ZXJ5IGxlbmd0aCwgZGVmZW5kcyBpdHNlbGYgYWdhaW5zdCB0aGUgcmlzayBvZiBiZWluZyByZWFkLgAA\" }]}") == 0);
        ASSERT_TRUE(strcmp(json_2, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": []}") == 0);

        str_buf_drop(&sb);
        rec_drop(&doc);
        rec_drop(&rev_doc);
        free(json_1);
        free(json_2);
}

TEST(CarbonTest, CarbonRemoveFirstBinary)
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
        insert_binary(ins, data1, strlen(data1), "txt", NULL);

        const char *data2 = "{\"key\": \"value\"}";
        insert_binary(ins, data2, strlen(data2), "json", NULL);

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
        ASSERT_EQ(next_type, FIELD_BINARY);
        revise_iterator_close(&rev_it);
        revise_end(&revise);
        // -------------------------------------------------------------------------------------------------------------

        char *json_2 = strdup(rec_to_json(&sb, &rev_doc));

        // -------------------------------------------------------------------------------------------------------------


        //printf("BEFORE\t'%s'\nAFTER\t'%s'\n", json_1, json_2);

        ASSERT_TRUE(strcmp(json_1, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [{ \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"VGhpcyByZXBvcnQsIGJ5IGl0cyB2ZXJ5IGxlbmd0aCwgZGVmZW5kcyBpdHNlbGYgYWdhaW5zdCB0aGUgcmlzayBvZiBiZWluZyByZWFkLgAA\" }, { \"type\": \"application/json\", \"encoding\": \"base64\", \"binary-str_buf\": \"eyJrZXkiOiAidmFsdWUifQAA\" }]}") == 0);
        ASSERT_TRUE(strcmp(json_2, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [{ \"type\": \"application/json\", \"encoding\": \"base64\", \"binary-str_buf\": \"eyJrZXkiOiAidmFsdWUifQAA\" }]}") == 0);

        str_buf_drop(&sb);
        rec_drop(&doc);
        rec_drop(&rev_doc);
        free(json_1);
        free(json_2);
}

TEST(CarbonTest, CarbonRemoveLastBinary)
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
        insert_binary(ins, data1, strlen(data1), "txt", NULL);

        const char *data2 = "{\"key\": \"value\"}";
        insert_binary(ins, data2, strlen(data2), "json", NULL);

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
        revise_iterator_close(&rev_it);
        revise_end(&revise);
        // -------------------------------------------------------------------------------------------------------------

        char *json_2 = strdup(rec_to_json(&sb, &rev_doc));

        // -------------------------------------------------------------------------------------------------------------


        // printf("BEFORE\t'%s'\nAFTER\t'%s'\n", json_1, json_2);

        ASSERT_TRUE(strcmp(json_1, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [{ \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"VGhpcyByZXBvcnQsIGJ5IGl0cyB2ZXJ5IGxlbmd0aCwgZGVmZW5kcyBpdHNlbGYgYWdhaW5zdCB0aGUgcmlzayBvZiBiZWluZyByZWFkLgAA\" }, { \"type\": \"application/json\", \"encoding\": \"base64\", \"binary-str_buf\": \"eyJrZXkiOiAidmFsdWUifQAA\" }]}") == 0);
        ASSERT_TRUE(strcmp(json_2, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [{ \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"VGhpcyByZXBvcnQsIGJ5IGl0cyB2ZXJ5IGxlbmd0aCwgZGVmZW5kcyBpdHNlbGYgYWdhaW5zdCB0aGUgcmlzayBvZiBiZWluZyByZWFkLgAA\" }]}") == 0);

        str_buf_drop(&sb);
        rec_drop(&doc);
        rec_drop(&rev_doc);
        free(json_1);
        free(json_2);
}

TEST(CarbonTest, CarbonRemoveMiddleBinary)
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
        insert_binary(ins, data1, strlen(data1), "txt", NULL);

        const char *data2 = "{\"key\": \"value\"}";
        insert_binary(ins, data2, strlen(data2), "json", NULL);

        const char *data3 = "<html><body><p>The quick brown fox jumps over the lazy dog</p></body></html>";
        insert_binary(ins, data3, strlen(data3), "html", NULL);

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
        ASSERT_EQ(type, FIELD_BINARY);
        has_next = arr_it_next(&rev_it);
        ASSERT_FALSE(has_next);
        revise_iterator_close(&rev_it);
        revise_end(&revise);
        // -------------------------------------------------------------------------------------------------------------

        char *json_2 = strdup(rec_to_json(&sb, &rev_doc));

        // -------------------------------------------------------------------------------------------------------------

        printf("BEFORE\t'%s'\nAFTER\t'%s'\n", json_1, json_2);

        ASSERT_TRUE(strcmp(json_1, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [{ \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"VGhpcyByZXBvcnQsIGJ5IGl0cyB2ZXJ5IGxlbmd0aCwgZGVmZW5kcyBpdHNlbGYgYWdhaW5zdCB0aGUgcmlzayBvZiBiZWluZyByZWFkLgAA\" }, { \"type\": \"application/json\", \"encoding\": \"base64\", \"binary-str_buf\": \"eyJrZXkiOiAidmFsdWUifQAA\" }, { \"type\": \"text/html\", \"encoding\": \"base64\", \"binary-str_buf\": \"PGh0bWw+PGJvZHk+PHA+VGhlIHF1aWNrIGJyb3duIGZveCBqdW1wcyBvdmVyIHRoZSBsYXp5IGRvZzwvcD48L2JvZHk+PC9odG1sPgAA\" }]}") == 0);
        ASSERT_TRUE(strcmp(json_2, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [{ \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"VGhpcyByZXBvcnQsIGJ5IGl0cyB2ZXJ5IGxlbmd0aCwgZGVmZW5kcyBpdHNlbGYgYWdhaW5zdCB0aGUgcmlzayBvZiBiZWluZyByZWFkLgAA\" }, { \"type\": \"text/html\", \"encoding\": \"base64\", \"binary-str_buf\": \"PGh0bWw+PGJvZHk+PHA+VGhlIHF1aWNrIGJyb3duIGZveCBqdW1wcyBvdmVyIHRoZSBsYXp5IGRvZzwvcD48L2JvZHk+PC9odG1sPgAA\" }]}") == 0);

        str_buf_drop(&sb);
        rec_drop(&doc);
        rec_drop(&rev_doc);
        free(json_1);
        free(json_2);
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}