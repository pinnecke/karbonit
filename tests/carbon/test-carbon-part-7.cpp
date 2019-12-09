#include <gtest/gtest.h>
#include <printf.h>

#include <jakson/jakson.h>

TEST(CarbonTest, CarbonObjectInsertPropDuringItAtIndex5)
{
        rec doc, rev_doc;
        rec_new context;
        rev revise;
        arr_it rev_it;
        str_buf sb;
        bool has_next;
        str_buf_create(&sb);

        obj_state state;
        insert nested_ins;

        // -------------------------------------------------------------------------------------------------------------
        insert *ins = rec_create_begin(&context, &doc, KEY_NOKEY, OPTIMIZE);

        insert *nested_obj_ins = insert_object_begin(&state, ins, 200);

        insert_prop_false(nested_obj_ins, "1");
        insert_prop_null(nested_obj_ins, "2");
        insert_prop_u8(nested_obj_ins, "3", 1);
        insert_prop_string(nested_obj_ins, "4", "v1");
        insert_prop_u16(nested_obj_ins, "5", 2);

        insert_prop_object_end(&state);

        rec_create_end(&context);
        // -------------------------------------------------------------------------------------------------------------

        char *json_1 = strdup(rec_to_json(&sb, &doc));

        // -------------------------------------------------------------------------------------------------------------

        revise_begin(&revise, &rev_doc, &doc);
        revise_iterator_open(&rev_it, &revise);
        has_next = arr_it_next(&rev_it);
        ASSERT_TRUE(has_next);

        // -------------------------------------------------------------------------------------------------------------

        field_e field_type;
        arr_it_field_type(&field_type, &rev_it);
        ASSERT_EQ(field_type, FIELD_OBJECT_UNSORTED_MULTIMAP);
        obj_it *obj_it = item_get_object(&(rev_it.item));
        ASSERT_TRUE(obj_it_next(obj_it));
        ASSERT_TRUE(obj_it_next(obj_it));
        ASSERT_TRUE(obj_it_next(obj_it));
        ASSERT_TRUE(obj_it_next(obj_it));
        ASSERT_TRUE(obj_it_next(obj_it));
        ASSERT_FALSE(obj_it_next(obj_it));

        internal_obj_it_insert_begin(&nested_ins, obj_it);
        insert_prop_string(&nested_ins, "Hello Long Key", "Hello Long Value");
        internal_obj_it_insert_end(&nested_ins);
        obj_it_drop(obj_it);

        // -------------------------------------------------------------------------------------------------------------

        has_next = arr_it_next(&rev_it);
        ASSERT_FALSE(has_next);

        revise_iterator_close(&rev_it);
        revise_end(&revise);

        char *json_2 = strdup(rec_to_json(&sb, &rev_doc));

        // -------------------------------------------------------------------------------------------------------------

        ASSERT_TRUE(strcmp(json_1, "{\"1\":false, \"2\":null, \"3\":1, \"4\":\"v1\", \"5\":2}") == 0);
        ASSERT_TRUE(strcmp(json_2, "{\"1\":false, \"2\":null, \"3\":1, \"4\":\"v1\", \"5\":2, \"Hello Long Key\":\"Hello Long Value\"}") == 0);

        str_buf_drop(&sb);
        rec_drop(&doc);
        rec_drop(&rev_doc);
        free(json_1);
        free(json_2);
}

TEST(CarbonTest, CarbonObjectRemovePropByKey)
{
        rec doc, rev_doc;
        rec_new context;
        rev revise;
        arr_it rev_it;
        str_buf sb;
        bool has_next;
        str_buf_create(&sb);

        obj_state state;

        // -------------------------------------------------------------------------------------------------------------
        insert *ins = rec_create_begin(&context, &doc, KEY_NOKEY, OPTIMIZE);

        insert *nested_obj_ins = insert_object_begin(&state, ins, 200);

        insert_prop_false(nested_obj_ins, "1");
        insert_prop_null(nested_obj_ins, "2");
        insert_prop_u8(nested_obj_ins, "3", 1);
        insert_prop_string(nested_obj_ins, "4", "v1");
        insert_prop_u16(nested_obj_ins, "5", 2);

        insert_prop_object_end(&state);

        rec_create_end(&context);
        // -------------------------------------------------------------------------------------------------------------

        char *json_1 = strdup(rec_to_json(&sb, &doc));

        // -------------------------------------------------------------------------------------------------------------

        revise_begin(&revise, &rev_doc, &doc);
        revise_iterator_open(&rev_it, &revise);
        has_next = arr_it_next(&rev_it);
        ASSERT_TRUE(has_next);

        // -------------------------------------------------------------------------------------------------------------

        field_e field_type;
        arr_it_field_type(&field_type, &rev_it);
        ASSERT_EQ(field_type, FIELD_OBJECT_UNSORTED_MULTIMAP);
        obj_it *obj_it = item_get_object(&(rev_it.item));
        has_next = obj_it_next(obj_it);
        ASSERT_TRUE(has_next);
        auto prop_key = internal_obj_it_prop_name(obj_it);
        ASSERT_TRUE(strncmp(prop_key.str, "1", strlen("1")) == 0);

        internal_obj_it_remove(obj_it);
        obj_it_drop(obj_it);

        // -------------------------------------------------------------------------------------------------------------

        has_next = arr_it_next(&rev_it);
        ASSERT_FALSE(has_next);

        revise_iterator_close(&rev_it);
        revise_end(&revise);

        char *json_2 = strdup(rec_to_json(&sb, &rev_doc));

        // -------------------------------------------------------------------------------------------------------------

        ASSERT_TRUE(strcmp(json_1, "{\"1\":false, \"2\":null, \"3\":1, \"4\":\"v1\", \"5\":2}") == 0);
        ASSERT_TRUE(strcmp(json_2, "{\"2\":null, \"3\":1, \"4\":\"v1\", \"5\":2}") == 0);

        str_buf_drop(&sb);
        rec_drop(&doc);
        rec_drop(&rev_doc);
        free(json_1);
        free(json_2);
}

TEST(CarbonTest, CarbonObjectRemovePropByKeyTypeObjectNonEmpty)
{
        rec doc, rev_doc;
        rec_new context;
        rev revise;
        arr_it rev_it;
        str_buf sb;
        bool has_next;
        str_buf_create(&sb);

        obj_state state;
        obj_state nested_obj;

        // -------------------------------------------------------------------------------------------------------------
        insert *ins = rec_create_begin(&context, &doc, KEY_NOKEY, OPTIMIZE);

        insert *nested_obj_ins = insert_object_begin(&state, ins, 200);

        insert *nested_nested_obj_ins = insert_prop_object_begin(&nested_obj, nested_obj_ins, "1", 100);
        insert_prop_null(nested_nested_obj_ins, "2");
        insert_prop_u8(nested_nested_obj_ins, "3", 1);
        insert_prop_string(nested_nested_obj_ins, "4", "v1");
        insert_prop_u16(nested_nested_obj_ins, "5", 2);
        insert_prop_object_end(&nested_obj);

        insert_prop_null(nested_obj_ins, "6");
        insert_prop_u8(nested_obj_ins, "7", 1);
        insert_prop_string(nested_obj_ins, "8", "v1");
        insert_prop_u16(nested_obj_ins, "9", 2);

        insert_prop_object_end(&state);

        rec_create_end(&context);
        // -------------------------------------------------------------------------------------------------------------

        char *json_1 = strdup(rec_to_json(&sb, &doc));

        // -------------------------------------------------------------------------------------------------------------

        revise_begin(&revise, &rev_doc, &doc);
        revise_iterator_open(&rev_it, &revise);
        has_next = arr_it_next(&rev_it);
        ASSERT_TRUE(has_next);

        // -------------------------------------------------------------------------------------------------------------

        field_e field_type;
        arr_it_field_type(&field_type, &rev_it);
        ASSERT_EQ(field_type, FIELD_OBJECT_UNSORTED_MULTIMAP);
        obj_it *obj_it = item_get_object(&(rev_it.item));
        has_next = obj_it_next(obj_it);
        ASSERT_TRUE(has_next);
        auto prop_key = internal_obj_it_prop_name(obj_it);
        ASSERT_TRUE(strncmp(prop_key.str, "1", strlen("1")) == 0);

        internal_obj_it_remove(obj_it);
        obj_it_drop(obj_it);

        // -------------------------------------------------------------------------------------------------------------

        has_next = arr_it_next(&rev_it);
        ASSERT_FALSE(has_next);

        revise_iterator_close(&rev_it);
        revise_end(&revise);

        char *json_2 = strdup(rec_to_json(&sb, &rev_doc));

        // -------------------------------------------------------------------------------------------------------------

        ASSERT_TRUE(strcmp(json_1, "{\"1\":{\"2\":null, \"3\":1, \"4\":\"v1\", \"5\":2}, \"6\":null, \"7\":1, \"8\":\"v1\", \"9\":2}") == 0);
        ASSERT_TRUE(strcmp(json_2, "{\"6\":null, \"7\":1, \"8\":\"v1\", \"9\":2}") == 0);

        str_buf_drop(&sb);
        rec_drop(&doc);
        rec_drop(&rev_doc);
        free(json_1);
        free(json_2);
}

TEST(CarbonTest, CarbonObjectRemovePropByKeyTypeArrayEmpty)
{
        rec doc, rev_doc;
        rec_new context;
        rev revise;
        arr_it rev_it;
        str_buf sb;
        bool has_next;
        str_buf_create(&sb);

        obj_state state;
        arr_state nested_arr;

        // -------------------------------------------------------------------------------------------------------------
        insert *ins = rec_create_begin(&context, &doc, KEY_NOKEY, OPTIMIZE);

        insert *nested_obj_ins = insert_object_begin(&state, ins, 200);

        insert_prop_array_begin(&nested_arr, nested_obj_ins, "1", 100);

        insert_prop_array_end(&nested_arr);

        insert_prop_null(nested_obj_ins, "2");
        insert_prop_u8(nested_obj_ins, "3", 1);
        insert_prop_string(nested_obj_ins, "4", "v1");
        insert_prop_u16(nested_obj_ins, "5", 2);

        insert_prop_object_end(&state);

        rec_create_end(&context);
        // -------------------------------------------------------------------------------------------------------------

        char *json_1 = strdup(rec_to_json(&sb, &doc));

        // -------------------------------------------------------------------------------------------------------------

        revise_begin(&revise, &rev_doc, &doc);
        revise_iterator_open(&rev_it, &revise);
        has_next = arr_it_next(&rev_it);
        ASSERT_TRUE(has_next);

        // -------------------------------------------------------------------------------------------------------------

        field_e field_type;
        arr_it_field_type(&field_type, &rev_it);
        ASSERT_EQ(field_type, FIELD_OBJECT_UNSORTED_MULTIMAP);
        obj_it *obj_it = item_get_object(&(rev_it.item));
        has_next = obj_it_next(obj_it);
        ASSERT_TRUE(has_next);
        auto prop_key = internal_obj_it_prop_name(obj_it);
        ASSERT_TRUE(strncmp(prop_key.str, "1", strlen("1")) == 0);

        internal_obj_it_remove(obj_it);
        obj_it_drop(obj_it);

        // -------------------------------------------------------------------------------------------------------------

        has_next = arr_it_next(&rev_it);
        ASSERT_FALSE(has_next);

        revise_iterator_close(&rev_it);
        revise_end(&revise);

        char *json_2 = strdup(rec_to_json(&sb, &rev_doc));

        // -------------------------------------------------------------------------------------------------------------

        //printf("%s\n", json_1);

        ASSERT_TRUE(strcmp(json_1, "{\"1\":[], \"2\":null, \"3\":1, \"4\":\"v1\", \"5\":2}") == 0);
        ASSERT_TRUE(strcmp(json_2, "{\"2\":null, \"3\":1, \"4\":\"v1\", \"5\":2}") == 0);

        str_buf_drop(&sb);
        rec_drop(&doc);
        rec_drop(&rev_doc);
        free(json_1);
        free(json_2);
}

TEST(CarbonTest, CarbonObjectRemovePropByKeyTypeArrayNonEmpty)
{
        rec doc, rev_doc;
        rec_new context;
        rev revise;
        arr_it rev_it;
        str_buf sb;
        bool has_next;
        str_buf_create(&sb);

        obj_state state;
        arr_state nested_arr;

        // -------------------------------------------------------------------------------------------------------------
        insert *ins = rec_create_begin(&context, &doc, KEY_NOKEY, OPTIMIZE);

        insert *nested_obj_ins = insert_object_begin(&state, ins, 200);

        insert *nested_arr_it = insert_prop_array_begin(&nested_arr, nested_obj_ins, "1", 100);
        insert_null(nested_arr_it);
        insert_u8(nested_arr_it, 1);
        insert_string(nested_arr_it, "v1");
        insert_u16(nested_arr_it, 2);
        insert_prop_array_end(&nested_arr);

        insert_prop_null(nested_obj_ins, "2");
        insert_prop_u8(nested_obj_ins, "3", 1);
        insert_prop_string(nested_obj_ins, "4", "v1");
        insert_prop_u16(nested_obj_ins, "5", 2);

        insert_prop_object_end(&state);

        rec_create_end(&context);
        // -------------------------------------------------------------------------------------------------------------

        char *json_1 = strdup(rec_to_json(&sb, &doc));

        // -------------------------------------------------------------------------------------------------------------

        revise_begin(&revise, &rev_doc, &doc);
        revise_iterator_open(&rev_it, &revise);
        has_next = arr_it_next(&rev_it);
        ASSERT_TRUE(has_next);

        // -------------------------------------------------------------------------------------------------------------

        field_e field_type;
        arr_it_field_type(&field_type, &rev_it);
        ASSERT_EQ(field_type, FIELD_OBJECT_UNSORTED_MULTIMAP);
        obj_it *obj_it = item_get_object(&(rev_it.item));
        has_next = obj_it_next(obj_it);
        ASSERT_TRUE(has_next);
        auto prop_key = internal_obj_it_prop_name(obj_it);
        ASSERT_TRUE(strncmp(prop_key.str, "1", strlen("1")) == 0);

        internal_obj_it_remove(obj_it);
        obj_it_drop(obj_it);

        // -------------------------------------------------------------------------------------------------------------

        has_next = arr_it_next(&rev_it);
        ASSERT_FALSE(has_next);

        revise_iterator_close(&rev_it);
        revise_end(&revise);

        char *json_2 = strdup(rec_to_json(&sb, &rev_doc));

        // -------------------------------------------------------------------------------------------------------------

        ASSERT_TRUE(strcmp(json_1, "{\"1\":[null, 1, \"v1\", 2], \"2\":null, \"3\":1, \"4\":\"v1\", \"5\":2}") == 0);
        ASSERT_TRUE(strcmp(json_2, "{\"2\":null, \"3\":1, \"4\":\"v1\", \"5\":2}") == 0);

        str_buf_drop(&sb);
        rec_drop(&doc);
        rec_drop(&rev_doc);
        free(json_1);
        free(json_2);
}

TEST(CarbonTest, CarbonObjectRemovePropByKeyTypeColumnEmpty)
{
        rec doc, rev_doc;
        rec_new context;
        rev revise;
        arr_it rev_it;
        str_buf sb;
        bool has_next;
        str_buf_create(&sb);

        obj_state state;
        col_state nested_col;

        // -------------------------------------------------------------------------------------------------------------
        insert *ins = rec_create_begin(&context, &doc, KEY_NOKEY, OPTIMIZE);

        insert *nested_obj_ins = insert_object_begin(&state, ins, 200);

        insert_prop_column_begin(&nested_col, nested_obj_ins, "1", COLUMN_U32, 100);

        insert_prop_column_end(&nested_col);

        insert_prop_null(nested_obj_ins, "2");
        insert_prop_u8(nested_obj_ins, "3", 1);
        insert_prop_string(nested_obj_ins, "4", "v1");
        insert_prop_u16(nested_obj_ins, "5", 2);

        insert_prop_object_end(&state);

        rec_create_end(&context);
        // -------------------------------------------------------------------------------------------------------------

        char *json_1 = strdup(rec_to_json(&sb, &doc));

        // -------------------------------------------------------------------------------------------------------------

        revise_begin(&revise, &rev_doc, &doc);
        revise_iterator_open(&rev_it, &revise);
        has_next = arr_it_next(&rev_it);
        ASSERT_TRUE(has_next);

        // -------------------------------------------------------------------------------------------------------------

        field_e field_type;
        arr_it_field_type(&field_type, &rev_it);
        ASSERT_EQ(field_type, FIELD_OBJECT_UNSORTED_MULTIMAP);
        obj_it *obj_it = item_get_object(&(rev_it.item));
        has_next = obj_it_next(obj_it);
        ASSERT_TRUE(has_next);
        auto prop_key = internal_obj_it_prop_name(obj_it);
        ASSERT_TRUE(strncmp(prop_key.str, "1", strlen("1")) == 0);

        internal_obj_it_remove(obj_it);
        obj_it_drop(obj_it);

        // -------------------------------------------------------------------------------------------------------------

        has_next = arr_it_next(&rev_it);
        ASSERT_FALSE(has_next);

        revise_iterator_close(&rev_it);
        revise_end(&revise);

        char *json_2 = strdup(rec_to_json(&sb, &rev_doc));

        // -------------------------------------------------------------------------------------------------------------

        ASSERT_TRUE(strcmp(json_1, "{\"1\":[], \"2\":null, \"3\":1, \"4\":\"v1\", \"5\":2}") == 0);
        ASSERT_TRUE(strcmp(json_2, "{\"2\":null, \"3\":1, \"4\":\"v1\", \"5\":2}") == 0);

        str_buf_drop(&sb);
        rec_drop(&doc);
        rec_drop(&rev_doc);
        free(json_1);
        free(json_2);
}

TEST(CarbonTest, CarbonObjectRemovePropByKeyTypeObjectEmpty)
{
        rec doc, rev_doc;
        rec_new context;
        rev revise;
        arr_it rev_it;
        str_buf sb;
        bool has_next;
        str_buf_create(&sb);

        obj_state state;
        obj_state nested_obj;

        // -------------------------------------------------------------------------------------------------------------
        insert *ins = rec_create_begin(&context, &doc, KEY_NOKEY, OPTIMIZE);

        insert *nested_obj_ins = insert_object_begin(&state, ins, 200);

        insert_prop_object_begin(&nested_obj, nested_obj_ins, "1", 100);
        insert_prop_object_end(&nested_obj);

        insert_prop_null(nested_obj_ins, "2");
        insert_prop_u8(nested_obj_ins, "3", 1);
        insert_prop_string(nested_obj_ins, "4", "v1");
        insert_prop_u16(nested_obj_ins, "5", 2);

        insert_prop_object_end(&state);

        rec_create_end(&context);
        // -------------------------------------------------------------------------------------------------------------

        char *json_1 = strdup(rec_to_json(&sb, &doc));

        // -------------------------------------------------------------------------------------------------------------

        revise_begin(&revise, &rev_doc, &doc);
        revise_iterator_open(&rev_it, &revise);
        has_next = arr_it_next(&rev_it);
        ASSERT_TRUE(has_next);

        // -------------------------------------------------------------------------------------------------------------

        field_e field_type;
        arr_it_field_type(&field_type, &rev_it);
        ASSERT_EQ(field_type, FIELD_OBJECT_UNSORTED_MULTIMAP);
        obj_it *obj_it = item_get_object(&(rev_it.item));
        has_next = obj_it_next(obj_it);
        ASSERT_TRUE(has_next);
        auto prop_key = internal_obj_it_prop_name(obj_it);
        ASSERT_TRUE(strncmp(prop_key.str, "1", strlen("1")) == 0);

        internal_obj_it_remove(obj_it);
        obj_it_drop(obj_it);

        // -------------------------------------------------------------------------------------------------------------

        has_next = arr_it_next(&rev_it);
        ASSERT_FALSE(has_next);

        revise_iterator_close(&rev_it);
        revise_end(&revise);

        char *json_2 = strdup(rec_to_json(&sb, &rev_doc));

        // -------------------------------------------------------------------------------------------------------------

        ASSERT_TRUE(strcmp(json_1, "{\"1\":{}, \"2\":null, \"3\":1, \"4\":\"v1\", \"5\":2}") == 0);
        ASSERT_TRUE(strcmp(json_2, "{\"2\":null, \"3\":1, \"4\":\"v1\", \"5\":2}") == 0);

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