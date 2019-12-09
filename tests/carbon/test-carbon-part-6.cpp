#include <gtest/gtest.h>
#include <printf.h>

#include <jakson/jakson.h>


TEST(CarbonTest, CarbonObjectInsertMultipleStringMixedTypes)
{
        rec doc;
        rec_new context;
        obj_state state;

        // -------------------------------------------------------------------------------------------------------------

        insert *ins = rec_create_begin(&context, &doc, KEY_NOKEY, OPTIMIZE);

        insert *obj_ins = insert_object_begin(&state, ins, 1);

        insert_prop_false(obj_ins, "k2");
        insert_prop_null(obj_ins, "k3");
        insert_prop_u8(obj_ins, "k4", 1);
        insert_prop_string(obj_ins, "s1", "v1");
        insert_prop_u16(obj_ins, "k5", 2);
        insert_prop_string(obj_ins, "s2-longer", "world");
        insert_prop_u32(obj_ins, "k6", 3);
        insert_prop_u64(obj_ins, "k7", 4);
        insert_prop_i8(obj_ins, "k8", -1);
        insert_prop_string(obj_ins, "s3", "there");
        insert_prop_i16(obj_ins, "k9", -2);
        insert_prop_i32(obj_ins, "k10", -3);
        insert_prop_i64(obj_ins, "k11", -4);
        insert_prop_float(obj_ins, "k12", 42.23);
        insert_prop_true(obj_ins, "k1");


        insert_object_end(&state);

        rec_create_end(&context);

        // -------------------------------------------------------------------------------------------------------------

        str_buf sb;
        str_buf_create(&sb);

        // rec_print(stdout, &doc);
        ASSERT_TRUE(strcmp(rec_to_json(&sb, &doc), "{\"k2\":false, \"k3\":null, \"k4\":1, \"s1\":\"v1\", \"k5\":2, \"s2-longer\":\"world\", \"k6\":3, \"k7\":4, \"k8\":-1, \"s3\":\"there\", \"k9\":-2, \"k10\":-3, \"k11\":-4, \"k12\":42.23, \"k1\":true}") == 0);

        str_buf_drop(&sb);
        rec_drop(&doc);
}

TEST(CarbonTest, CarbonObjectInsertBinary)
{
        rec doc;
        rec_new context;
        obj_state state;

        // -------------------------------------------------------------------------------------------------------------

        insert *ins = rec_create_begin(&context, &doc, KEY_NOKEY, OPTIMIZE);

        insert *obj_ins = insert_object_begin(&state, ins, 1);

        insert_prop_binary(obj_ins, "my binary", "My Plain-Text", strlen("My Plain-Text"), "txt", NULL);

        insert_object_end(&state);

        rec_create_end(&context);

        // -------------------------------------------------------------------------------------------------------------

        str_buf sb;
        str_buf_create(&sb);

        // rec_print(stdout, &doc);
        ASSERT_TRUE(strcmp(rec_to_json(&sb, &doc), "{\"my binary\":{\"type\":\"text/plain\", \"encoding\":\"base64\", \"value\":\"TXkgUGxhaW4tVGV4dAAA\"}}") == 0);

        str_buf_drop(&sb);
        rec_drop(&doc);
}

TEST(CarbonTest, CarbonObjectInsertMultipleBinariesMixedTypes)
{
        rec doc;
        rec_new context;
        obj_state state;

        // -------------------------------------------------------------------------------------------------------------

        insert *ins = rec_create_begin(&context, &doc, KEY_NOKEY, OPTIMIZE);

        insert *obj_ins = insert_object_begin(&state, ins, 1);

        insert_prop_float(obj_ins, "k12", 42.23);
        insert_prop_true(obj_ins, "k1");
        insert_prop_binary(obj_ins, "b1", "Hello", strlen("Hello"), "txt", NULL);
        insert_prop_binary(obj_ins, "my binary", ",", strlen(","), "txt", NULL);
        insert_prop_false(obj_ins, "k2");
        insert_prop_null(obj_ins, "k3");
        insert_prop_u8(obj_ins, "k4", 1);
        insert_prop_string(obj_ins, "s1", "v1");
        insert_prop_u16(obj_ins, "k5", 2);
        insert_prop_binary(obj_ins, "b2", "World", strlen("World"), "txt", NULL);
        insert_prop_string(obj_ins, "s2-longer", "world");
        insert_prop_u32(obj_ins, "k6", 3);
        insert_prop_u64(obj_ins, "k7", 4);
        insert_prop_i8(obj_ins, "k8", -1);
        insert_prop_string(obj_ins, "s3", "there");
        insert_prop_i16(obj_ins, "k9", -2);
        insert_prop_i32(obj_ins, "k10", -3);
        insert_prop_i64(obj_ins, "k11", -4);

        insert_object_end(&state);

        rec_create_end(&context);

        // -------------------------------------------------------------------------------------------------------------

        str_buf sb;
        str_buf_create(&sb);

        // rec_print(stdout, &doc);
        const char *out = rec_to_json(&sb, &doc);
        ASSERT_TRUE(strcmp(out, "{\"k12\":42.23, \"k1\":true, \"b1\":{\"type\":\"text/plain\", \"encoding\":\"base64\", \"value\":\"A==sbG8AA\"}, \"my binary\":{\"type\":\"text/plain\", \"encoding\":\"base64\", \"value\":\"LAAA\"}, \"k2\":false, \"k3\":null, \"k4\":1, \"s1\":\"v1\", \"k5\":2, \"b2\":{\"type\":\"text/plain\", \"encoding\":\"base64\", \"value\":\"A==ybGQAA\"}, \"s2-longer\":\"world\", \"k6\":3, \"k7\":4, \"k8\":-1, \"s3\":\"there\", \"k9\":-2, \"k10\":-3, \"k11\":-4}") == 0);

        str_buf_drop(&sb);
        rec_drop(&doc);
}

TEST(CarbonTest, CarbonObjectInsertMultipleBinaries)
{
        rec doc;
        rec_new context;
        obj_state state;

        // -------------------------------------------------------------------------------------------------------------

        insert *ins = rec_create_begin(&context, &doc, KEY_NOKEY, OPTIMIZE);

        insert *obj_ins = insert_object_begin(&state, ins, 1);

        insert_prop_binary(obj_ins, "b1", "Hello", strlen("Hello"), "txt", NULL);
        insert_prop_binary(obj_ins, "my binary", ",", strlen(","), "txt", NULL);
        insert_prop_binary(obj_ins, "b2", "World", strlen("World"), "txt", NULL);

        insert_object_end(&state);

        rec_create_end(&context);

        // -------------------------------------------------------------------------------------------------------------

        str_buf sb;
        str_buf_create(&sb);

        // rec_print(stdout, &doc);
        ASSERT_TRUE(strcmp(rec_to_json(&sb, &doc), "{\"b1\":{\"type\":\"text/plain\", \"encoding\":\"base64\", \"value\":\"A==sbG8AA\"}, \"my binary\":{\"type\":\"text/plain\", \"encoding\":\"base64\", \"value\":\"LAAA\"}, \"b2\":{\"type\":\"text/plain\", \"encoding\":\"base64\", \"value\":\"A==ybGQAA\"}}") == 0);

        str_buf_drop(&sb);
        rec_drop(&doc);
}

TEST(CarbonTest, CarbonObjectInsertObjectEmpty)
{
        rec doc;
        rec_new context;
        obj_state state, nested;

        // -------------------------------------------------------------------------------------------------------------

        insert *ins = rec_create_begin(&context, &doc, KEY_NOKEY, OPTIMIZE);

        insert *obj_ins = insert_object_begin(&state, ins, 1);

        insert_prop_object_begin(&nested, obj_ins, "my nested", 200);
        insert_prop_object_end(&nested);

        insert_object_end(&state);

        rec_create_end(&context);

        // -------------------------------------------------------------------------------------------------------------

        str_buf sb;
        str_buf_create(&sb);

        // rec_print(stdout, &doc);
        ASSERT_TRUE(strcmp(rec_to_json(&sb, &doc), "{\"my nested\":{}}") == 0);

        str_buf_drop(&sb);
        rec_drop(&doc);
}

TEST(CarbonTest, CarbonObjectInsertObjectMixedMxed)
{
        rec doc;
        rec_new context;
        obj_state state, nested;

        // -------------------------------------------------------------------------------------------------------------


        insert *ins = rec_create_begin(&context, &doc, KEY_NOKEY, OPTIMIZE);

        insert *obj_ins = insert_object_begin(&state, ins, 1);

        insert_prop_float(obj_ins, "1", 42.23);
        insert_prop_true(obj_ins, "2");
        insert_prop_binary(obj_ins, "3", "Hello", strlen("Hello"), "txt", NULL);
        insert_prop_binary(obj_ins, "4", ",", strlen(","), "txt", NULL);
        insert_prop_binary(obj_ins, "5", "World", strlen("World"), "txt", NULL);
        insert_prop_string(obj_ins, "6", "world");

        insert *nested_obj_ins = insert_prop_object_begin(&nested, obj_ins, "my nested", 200);

        insert_prop_false(nested_obj_ins, "7");
        insert_prop_null(nested_obj_ins, "8");
        insert_prop_u8(nested_obj_ins, "9", 1);
        insert_prop_string(nested_obj_ins, "10", "v1");
        insert_prop_u16(nested_obj_ins, "11", 2);

        insert_prop_object_end(&nested);

        insert_prop_u32(obj_ins, "12", 3);
        insert_prop_u64(obj_ins, "13", 4);
        insert_prop_i8(obj_ins, "14", -1);
        insert_prop_string(obj_ins, "15", "there");
        insert_prop_i16(obj_ins, "16", -2);
        insert_prop_i32(obj_ins, "17", -3);
        insert_prop_i64(obj_ins, "18", -4);

        insert_object_end(&state);

        rec_create_end(&context);

        // -------------------------------------------------------------------------------------------------------------

        str_buf sb;
        str_buf_create(&sb);

        // rec_print(stdout, &doc);
        ASSERT_TRUE(strcmp(rec_to_json(&sb, &doc), "{\"1\":42.23, \"2\":true, \"3\":{\"type\":\"text/plain\", \"encoding\":\"base64\", \"value\":\"A==sbG8AA\"}, \"4\":{\"type\":\"text/plain\", \"encoding\":\"base64\", \"value\":\"LAAA\"}, \"5\":{\"type\":\"text/plain\", \"encoding\":\"base64\", \"value\":\"A==ybGQAA\"}, \"6\":\"world\", \"my nested\":{\"7\":false, \"8\":null, \"9\":1, \"10\":\"v1\", \"11\":2}, \"12\":3, \"13\":4, \"14\":-1, \"15\":\"there\", \"16\":-2, \"17\":-3, \"18\":-4}") == 0);

        str_buf_drop(&sb);
        rec_drop(&doc);
}

TEST(CarbonTest, CarbonObjectInsertArrayEmpty)
{
        rec doc;
        rec_new context;
        obj_state state;
        arr_state array_state;

        // -------------------------------------------------------------------------------------------------------------

        insert *ins = rec_create_begin(&context, &doc, KEY_NOKEY, OPTIMIZE);

        insert *obj_ins = insert_object_begin(&state, ins, 1);

        insert_prop_array_begin(&array_state, obj_ins, "my array", 200);
        insert_prop_array_end(&array_state);

        insert_object_end(&state);

        rec_create_end(&context);

        // -------------------------------------------------------------------------------------------------------------

        str_buf sb;
        str_buf_create(&sb);

        // rec_print(stdout, &doc);
        ASSERT_TRUE(strcmp(rec_to_json(&sb, &doc), "{\"my array\":[]}") == 0);

        str_buf_drop(&sb);
        rec_drop(&doc);
}

TEST(CarbonTest, CarbonObjectInsertArrayData)
{
        rec doc;
        rec_new context;
        obj_state state;
        arr_state array_state, nested_array_state;
        col_state column_state;

        // -------------------------------------------------------------------------------------------------------------

        insert *ins = rec_create_begin(&context, &doc, KEY_NOKEY, OPTIMIZE);

        insert *obj_ins = insert_object_begin(&state, ins, 1);

        insert *nested_array_ins = insert_prop_array_begin(&array_state, obj_ins, "my array", 200);

        insert *column_ins = insert_column_begin(&column_state, nested_array_ins, COLUMN_U32, 10);
        insert_u32(column_ins, 'X');
        insert_u32(column_ins, 'Y');
        insert_u32(column_ins, 'Z');
        insert_column_end(&column_state);
        insert *nested_ins = insert_array_begin(&nested_array_state, nested_array_ins, 10);
        insert_string(nested_ins, "Hello");
        column_ins = insert_column_begin(&column_state, nested_ins, COLUMN_U32, 10);
        insert_u32(column_ins, 'A');
        insert_u32(column_ins, 'B');
        insert_u32(column_ins, 'C');
        insert_column_end(&column_state);
        insert_string(nested_ins, "World");
        insert_array_end(&nested_array_state);
        insert_u8(nested_array_ins, 1);
        insert_u8(nested_array_ins, 1);
        column_ins = insert_column_begin(&column_state, nested_array_ins, COLUMN_U32, 10);
        insert_u32(column_ins, 23);
        insert_u32(column_ins, 24);
        insert_u32(column_ins, 25);
        insert_column_end(&column_state);
        insert_u8(nested_array_ins, 1);

        insert_prop_array_end(&array_state);

        insert_object_end(&state);

        rec_create_end(&context);

        // -------------------------------------------------------------------------------------------------------------

        str_buf sb;
        str_buf_create(&sb);

        // rec_print(stdout, &doc);
        ASSERT_TRUE(strcmp(rec_to_json(&sb, &doc), "{\"my array\":[[88, 89, 90], [\"Hello\", [65, 66, 67], \"World\"], 1, 1, [23, 24, 25], 1]}") == 0);

        str_buf_drop(&sb);
        rec_drop(&doc);
}

TEST(CarbonTest, CarbonObjectInsertColumnNonEmpty)
{
        rec doc;
        rec_new context;
        obj_state state;
        col_state column_state;

        // -------------------------------------------------------------------------------------------------------------

        insert *ins = rec_create_begin(&context, &doc, KEY_NOKEY, OPTIMIZE);

        insert *obj_ins = insert_object_begin(&state, ins, 1);

        insert *nested_column_ins = insert_prop_column_begin(&column_state, obj_ins, "my column", COLUMN_U16, 200);
        insert_u16(nested_column_ins, 1);
        insert_u16(nested_column_ins, 2);
        insert_u16(nested_column_ins, 3);
        insert_prop_column_end(&column_state);

        insert_object_end(&state);

        rec_create_end(&context);

        // -------------------------------------------------------------------------------------------------------------

        str_buf sb;
        str_buf_create(&sb);

        // rec_print(stdout, &doc);
        ASSERT_TRUE(strcmp(rec_to_json(&sb, &doc), "{\"my column\":[1, 2, 3]}") == 0);

        str_buf_drop(&sb);
        rec_drop(&doc);
}

//static void create_nested_doc(carbon *rev_doc)
//{
//        carbon doc;
//        carbon_revise revise;
//        arr_it it;
//        insert nested_ins, *array_ins, *col_ins, *nested_array_ins;
//        arr_state array_state, nested_array_state;
//        col_state column_state;
//
//        rec_create_empty(&doc, LIST_UNSORTED_MULTISET, KEY_NOKEY);
//        revise_begin(&revise, rev_doc, &doc);
//        revise_iterator_open(&it, &revise);
//
//        arr_it_insert_begin(&nested_ins, &it);
//
//        array_ins = insert_array_begin(&array_state, &nested_ins, 10);
//
//        insert_null(array_ins);
//        insert_true(array_ins);
//        insert_false(array_ins);
//        insert_u8(array_ins, 8);
//        insert_i16(array_ins, -16);
//        insert_string(array_ins, "Hello, World!");
//        insert_binary(array_ins, "My Plain-Text", strlen("My Plain-Text"), "txt", NULL);
//        insert_binary(array_ins, "My Own Format", strlen("My Own Format"), NULL, "own");
//        col_ins = insert_column_begin(&column_state, array_ins, COLUMN_U32, 20);
//
//        insert_u32(col_ins, 32);
//        insert_u32(col_ins, 33);
//        insert_u32(col_ins, 34);
//        insert_u32(col_ins, 35);
//
//        insert_column_end(&column_state);
//
//        insert_array_begin(&nested_array_state, array_ins, 20);
//        insert_array_end(&nested_array_state);
//
//        nested_array_ins = insert_array_begin(&nested_array_state, array_ins, 20);
//
//        insert_null(nested_array_ins);
//        insert_true(nested_array_ins);
//        insert_false(nested_array_ins);
//        insert_u8(nested_array_ins, 8);
//        insert_i16(nested_array_ins, -16);
//        insert_string(nested_array_ins, "Hello, World!");
//        insert_binary(nested_array_ins, "My Plain-Text", strlen("My Plain-Text"), "txt", NULL);
//        insert_binary(nested_array_ins, "My Own Format", strlen("My Own Format"), NULL, "own");
//        col_ins = insert_column_begin(&column_state, nested_array_ins, COLUMN_U32, 20);
//
//        insert_u32(col_ins, 32);
//        insert_u32(col_ins, 33);
//        insert_u32(col_ins, 34);
//        insert_u32(col_ins, 35);
//
//        insert_column_end(&column_state);
//
//        insert_array_end(&nested_array_state);
//
//        insert_array_end(&array_state);
//
//        array_ins = insert_array_begin(&array_state, &nested_ins, 10);
//
//        insert_null(array_ins);
//        insert_true(array_ins);
//        insert_false(array_ins);
//        insert_u8(array_ins, 8);
//        insert_i16(array_ins, -16);
//        insert_string(array_ins, "Hello, World!");
//        insert_binary(array_ins, "My Plain-Text", strlen("My Plain-Text"), "txt", NULL);
//        insert_binary(array_ins, "My Own Format", strlen("My Own Format"), NULL, "own");
//        col_ins = insert_column_begin(&column_state, array_ins, COLUMN_U32, 20);
//
//        insert_u32(col_ins, 32);
//        insert_u32(col_ins, 33);
//        insert_u32(col_ins, 34);
//        insert_u32(col_ins, 35);
//
//        insert_column_end(&column_state);
//
//        insert_array_begin(&nested_array_state, array_ins, 20);
//        insert_array_end(&nested_array_state);
//
//        nested_array_ins = insert_array_begin(&nested_array_state, array_ins, 20);
//
//        insert_null(nested_array_ins);
//        insert_true(nested_array_ins);
//        insert_false(nested_array_ins);
//        insert_u8(nested_array_ins, 8);
//        insert_i16(nested_array_ins, -16);
//        insert_string(nested_array_ins, "Hello, World!");
//        insert_binary(nested_array_ins, "My Plain-Text", strlen("My Plain-Text"), "txt", NULL);
//        insert_binary(nested_array_ins, "My Own Format", strlen("My Own Format"), NULL, "own");
//        col_ins = insert_column_begin(&column_state, nested_array_ins, COLUMN_U32, 20);
//
//        insert_u32(col_ins, 32);
//        insert_u32(col_ins, 33);
//        insert_u32(col_ins, 34);
//        insert_u32(col_ins, 35);
//
//        insert_column_end(&column_state);
//
//        insert_array_end(&nested_array_state);
//
//        insert_array_end(&array_state);
//
//        arr_it_insert_end(&nested_ins);
//
//        revise_iterator_close(&it);
//        revise_end(&revise);
//}

TEST(CarbonTest, CarbonObjectRemoveTest)
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

        nested_obj_ins = insert_object_begin(&state, ins, 200);

        insert_prop_false(nested_obj_ins, "6");
        insert_prop_null(nested_obj_ins, "7");
        insert_prop_u8(nested_obj_ins, "8", 1);
        insert_prop_string(nested_obj_ins, "9", "v1");
        insert_prop_u16(nested_obj_ins, "10", 2);

        insert_prop_object_end(&state);

        nested_obj_ins = insert_object_begin(&state, ins, 200);

        insert_prop_false(nested_obj_ins, "11");
        insert_prop_null(nested_obj_ins, "12");
        insert_prop_u8(nested_obj_ins, "13", 1);
        insert_prop_string(nested_obj_ins, "14", "v1");
        insert_prop_u16(nested_obj_ins, "15", 2);

        insert_prop_object_end(&state);

        rec_create_end(&context);
        // -------------------------------------------------------------------------------------------------------------

        char *json_1 = strdup(rec_to_json(&sb, &doc));

        // -------------------------------------------------------------------------------------------------------------

        revise_begin(&revise, &rev_doc, &doc);
        revise_iterator_open(&rev_it, &revise);
        has_next = arr_it_next(&rev_it);
        ASSERT_TRUE(has_next);

        field_e field_type;
        arr_it_field_type(&field_type, &rev_it);
        ASSERT_EQ(field_type, FIELD_OBJECT_UNSORTED_MULTIMAP);

        internal_arr_it_remove(&rev_it);
        has_next = arr_it_next(&rev_it);
        ASSERT_TRUE(has_next);

        internal_arr_it_remove(&rev_it);
        has_next = arr_it_next(&rev_it);
        ASSERT_TRUE(has_next);

        internal_arr_it_remove(&rev_it);
        has_next = arr_it_next(&rev_it);
        ASSERT_FALSE(has_next);

        revise_iterator_close(&rev_it);
        revise_end(&revise);

        char *json_2 = strdup(rec_to_json(&sb, &rev_doc));

        // -------------------------------------------------------------------------------------------------------------

        ASSERT_TRUE(strcmp(json_1, "[{\"1\":false, \"2\":null, \"3\":1, \"4\":\"v1\", \"5\":2}, {\"6\":false, \"7\":null, \"8\":1, \"9\":\"v1\", \"10\":2}, {\"11\":false, \"12\":null, \"13\":1, \"14\":\"v1\", \"15\":2}]") == 0);
        ASSERT_TRUE(strcmp(json_2, "[]") == 0);

        str_buf_drop(&sb);
        rec_drop(&doc);
        rec_drop(&rev_doc);
        free(json_1);
        free(json_2);
}

TEST(CarbonTest, CarbonObjectRemoveSkipOneTest)
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

        nested_obj_ins = insert_object_begin(&state, ins, 200);

        insert_prop_false(nested_obj_ins, "6");
        insert_prop_null(nested_obj_ins, "7");
        insert_prop_u8(nested_obj_ins, "8", 1);
        insert_prop_string(nested_obj_ins, "9", "v1");
        insert_prop_u16(nested_obj_ins, "10", 2);

        insert_prop_object_end(&state);

        nested_obj_ins = insert_object_begin(&state, ins, 200);

        insert_prop_false(nested_obj_ins, "11");
        insert_prop_null(nested_obj_ins, "12");
        insert_prop_u8(nested_obj_ins, "13", 1);
        insert_prop_string(nested_obj_ins, "14", "v1");
        insert_prop_u16(nested_obj_ins, "15", 2);

        insert_prop_object_end(&state);

        rec_create_end(&context);
        // -------------------------------------------------------------------------------------------------------------

        char *json_1 = strdup(rec_to_json(&sb, &doc));

        // -------------------------------------------------------------------------------------------------------------

        revise_begin(&revise, &rev_doc, &doc);
        revise_iterator_open(&rev_it, &revise);
        has_next = arr_it_next(&rev_it);
        ASSERT_TRUE(has_next);

        field_e field_type;
        arr_it_field_type(&field_type, &rev_it);
        ASSERT_EQ(field_type, FIELD_OBJECT_UNSORTED_MULTIMAP);

        has_next = arr_it_next(&rev_it);
        ASSERT_TRUE(has_next);

        internal_arr_it_remove(&rev_it);
        has_next = arr_it_next(&rev_it);
        ASSERT_TRUE(has_next);

        internal_arr_it_remove(&rev_it);
        has_next = arr_it_next(&rev_it);
        ASSERT_FALSE(has_next);

        revise_iterator_close(&rev_it);
        revise_end(&revise);

        char *json_2 = strdup(rec_to_json(&sb, &rev_doc));

        // -------------------------------------------------------------------------------------------------------------

        ASSERT_TRUE(strcmp(json_1, "[{\"1\":false, \"2\":null, \"3\":1, \"4\":\"v1\", \"5\":2}, {\"6\":false, \"7\":null, \"8\":1, \"9\":\"v1\", \"10\":2}, {\"11\":false, \"12\":null, \"13\":1, \"14\":\"v1\", \"15\":2}]") == 0);
        ASSERT_TRUE(strcmp(json_2, "{\"1\":false, \"2\":null, \"3\":1, \"4\":\"v1\", \"5\":2}") == 0);

        str_buf_drop(&sb);
        rec_drop(&doc);
        rec_drop(&rev_doc);
        free(json_1);
        free(json_2);
}

TEST(CarbonTest, CarbonObjectInsertPropDuringIt)
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
        has_next = obj_it_next(obj_it);
        ASSERT_TRUE(has_next);
        auto prop_key = internal_obj_it_prop_name(obj_it);
        ASSERT_TRUE(strncmp(prop_key.str, "1", strlen("1")) == 0);

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
        //printf("\n%s\n", json_2);

        // -------------------------------------------------------------------------------------------------------------

        ASSERT_TRUE(strcmp(json_1, "{\"1\":false, \"2\":null, \"3\":1, \"4\":\"v1\", \"5\":2}") == 0);
        ASSERT_TRUE(strcmp(json_2, "{\"Hello Long Key\":\"Hello Long Value\", \"1\":false, \"2\":null, \"3\":1, \"4\":\"v1\", \"5\":2}") == 0);

        str_buf_drop(&sb);
        rec_drop(&doc);
        rec_drop(&rev_doc);
        free(json_1);
        free(json_2);
}

TEST(CarbonTest, CarbonObjectInsertPropDuringItAtIndex1)
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
        ASSERT_TRUE(strcmp(json_2, "{\"1\":false, \"Hello Long Key\":\"Hello Long Value\", \"2\":null, \"3\":1, \"4\":\"v1\", \"5\":2}") == 0);

        str_buf_drop(&sb);
        rec_drop(&doc);
        rec_drop(&rev_doc);
        free(json_1);
        free(json_2);
}

TEST(CarbonTest, CarbonObjectInsertPropDuringItAtIndex2)
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
        ASSERT_TRUE(strcmp(json_2, "{\"1\":false, \"2\":null, \"Hello Long Key\":\"Hello Long Value\", \"3\":1, \"4\":\"v1\", \"5\":2}") == 0);

        str_buf_drop(&sb);
        rec_drop(&doc);
        rec_drop(&rev_doc);
        free(json_1);
        free(json_2);
}

TEST(CarbonTest, CarbonObjectInsertPropDuringItAtIndex3)
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
        ASSERT_TRUE(strcmp(json_2, "{\"1\":false, \"2\":null, \"3\":1, \"Hello Long Key\":\"Hello Long Value\", \"4\":\"v1\", \"5\":2}") == 0);

        str_buf_drop(&sb);
        rec_drop(&doc);
        rec_drop(&rev_doc);
        free(json_1);
        free(json_2);
}

TEST(CarbonTest, CarbonObjectInsertPropDuringItAtIndex4)
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
        ASSERT_TRUE(strcmp(json_2, "{\"1\":false, \"2\":null, \"3\":1, \"4\":\"v1\", \"Hello Long Key\":\"Hello Long Value\", \"5\":2}") == 0);

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