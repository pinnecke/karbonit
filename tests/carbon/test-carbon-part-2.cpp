#include <gtest/gtest.h>
#include <printf.h>

#include <jakson/jakson.h>



TEST(CarbonTest, CarbonInsertInsertColumnNumbersZeroWithOverflow) {
        rec doc, rev_doc;
        rev revise;
        arr_it it;
        insert in;
        col_state column_state;

        rec_create_empty_ex(&doc, LIST_UNSORTED_MULTISET, KEY_NOKEY,16, 1);

        revise_begin(&revise, &rev_doc, &doc);
        revise_iterator_open(&it, &revise);
        arr_it_insert_begin(&in, &it);

        insert *nested_inserter_l1 = insert_column_begin(&column_state, &in, COLUMN_U8, 1);

        ASSERT_TRUE(nested_inserter_l1 != NULL);
        insert_u8(nested_inserter_l1, 1);
        insert_u8(nested_inserter_l1, 2);
        insert_u8(nested_inserter_l1, 3);
        insert_column_end(&column_state);

        arr_it_drop(&it);
        revise_end(&revise);

        //rec_hexdump_print(stdout, &rev_doc);

        // printf("Carbon DOC PRINT:");
        // rec_print(stdout, &rev_doc);
        // fflush(stdout);
        str_buf sb;
        str_buf_create(&sb);
        rec_to_json(&sb, &rev_doc);
        ASSERT_TRUE(0 == strcmp(str_buf_cstr(&sb), "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [[1, 2, 3]]}"));
        str_buf_drop(&sb);

        rec_drop(&doc);
        rec_drop(&rev_doc);
}

TEST(CarbonTest, CarbonInsertInsertColumnNumbersWithHighOverflow) {
        rec doc, rev_doc;
        rev revise;
        arr_it it;
        insert in;
        col_state column_state;

        rec_create_empty_ex(&doc, LIST_UNSORTED_MULTISET, KEY_NOKEY,16, 1);

        revise_begin(&revise, &rev_doc, &doc);
        revise_iterator_open(&it, &revise);
        arr_it_insert_begin(&in, &it);

        insert *nested_inserter_l1 = insert_column_begin(&column_state, &in, COLUMN_U32, 1);

        ASSERT_TRUE(nested_inserter_l1 != NULL);
        for (u32 i = 0; i < 100; i++) {
                insert_u32(nested_inserter_l1, i);
                insert_u32(nested_inserter_l1, i);
                insert_u32(nested_inserter_l1, i);
        }

        insert_column_end(&column_state);

        arr_it_drop(&it);
        revise_end(&revise);

        //rec_hexdump_print(stdout, &rev_doc);

        // printf("Carbon DOC PRINT:");
        // rec_print(stdout, &rev_doc);
        // fflush(stdout);
        str_buf sb;
        str_buf_create(&sb);
        rec_to_json(&sb, &rev_doc);
        ASSERT_TRUE(0 == strcmp(str_buf_cstr(&sb), "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [[0, 0, 0, 1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 4, 5, 5, 5, 6, 6, 6, 7, 7, 7, 8, 8, 8, 9, 9, 9, 10, 10, 10, 11, 11, 11, 12, 12, 12, 13, 13, 13, 14, 14, 14, 15, 15, 15, 16, 16, 16, 17, 17, 17, 18, 18, 18, 19, 19, 19, 20, 20, 20, 21, 21, 21, 22, 22, 22, 23, 23, 23, 24, 24, 24, 25, 25, 25, 26, 26, 26, 27, 27, 27, 28, 28, 28, 29, 29, 29, 30, 30, 30, 31, 31, 31, 32, 32, 32, 33, 33, 33, 34, 34, 34, 35, 35, 35, 36, 36, 36, 37, 37, 37, 38, 38, 38, 39, 39, 39, 40, 40, 40, 41, 41, 41, 42, 42, 42, 43, 43, 43, 44, 44, 44, 45, 45, 45, 46, 46, 46, 47, 47, 47, 48, 48, 48, 49, 49, 49, 50, 50, 50, 51, 51, 51, 52, 52, 52, 53, 53, 53, 54, 54, 54, 55, 55, 55, 56, 56, 56, 57, 57, 57, 58, 58, 58, 59, 59, 59, 60, 60, 60, 61, 61, 61, 62, 62, 62, 63, 63, 63, 64, 64, 64, 65, 65, 65, 66, 66, 66, 67, 67, 67, 68, 68, 68, 69, 69, 69, 70, 70, 70, 71, 71, 71, 72, 72, 72, 73, 73, 73, 74, 74, 74, 75, 75, 75, 76, 76, 76, 77, 77, 77, 78, 78, 78, 79, 79, 79, 80, 80, 80, 81, 81, 81, 82, 82, 82, 83, 83, 83, 84, 84, 84, 85, 85, 85, 86, 86, 86, 87, 87, 87, 88, 88, 88, 89, 89, 89, 90, 90, 90, 91, 91, 91, 92, 92, 92, 93, 93, 93, 94, 94, 94, 95, 95, 95, 96, 96, 96, 97, 97, 97, 98, 98, 98, 99, 99, 99]]}"));
        str_buf_drop(&sb);

        rec_drop(&doc);
        rec_drop(&rev_doc);
}

TEST(CarbonTest, CarbonInsertInsertMultipleColumnsNumbersWithHighOverflow) {
        rec doc, rev_doc;
        rev revise;
        arr_it it;
        insert in;
        col_state column_state;

        rec_create_empty_ex(&doc, LIST_UNSORTED_MULTISET, KEY_NOKEY,16, 1);

        revise_begin(&revise, &rev_doc, &doc);
        revise_iterator_open(&it, &revise);
        arr_it_insert_begin(&in, &it);

        for (u32 k = 0; k < 3; k++) {
                insert *nested_inserter_l1 = insert_column_begin(&column_state, &in, COLUMN_U32, 1);

                ASSERT_TRUE(nested_inserter_l1 != NULL);
                for (u32 i = 0; i < 4; i++) {
                        insert_u32(nested_inserter_l1, 'a' + i);
                        insert_u32(nested_inserter_l1, 'a' + i);
                        insert_u32(nested_inserter_l1, 'a' + i);
                }

                insert_column_end(&column_state);
        }

        arr_it_drop(&it);
        revise_end(&revise);

        ////rec_hexdump_print(stdout, &rev_doc);

        // printf("Carbon DOC PRINT:");
        // rec_print(stdout, &rev_doc);
        // fflush(stdout);
        str_buf sb;
        str_buf_create(&sb);
        rec_to_json(&sb, &rev_doc);
        ASSERT_TRUE(0 == strcmp(str_buf_cstr(&sb), "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [[97, 97, 97, 98, 98, 98, 99, 99, 99, 100, 100, 100], [97, 97, 97, 98, 98, 98, 99, 99, 99, 100, 100, 100], [97, 97, 97, 98, 98, 98, 99, 99, 99, 100, 100, 100]]}"));
        str_buf_drop(&sb);

        rec_drop(&doc);
        rec_drop(&rev_doc);
}

TEST(CarbonTest, CarbonInsertNullTest) {
        rec doc, rev_doc;
        rev revise;
        arr_it it;
        insert in;
        col_state column_state;
        insert *ins;

        rec_create_empty_ex(&doc, LIST_UNSORTED_MULTISET, KEY_NOKEY,20, 1);

        revise_begin(&revise, &rev_doc, &doc);
        revise_iterator_open(&it, &revise);
        arr_it_insert_begin(&in, &it);

        ins = insert_column_begin(&column_state, &in, COLUMN_U8, 10);
        insert_u8(ins, 1);
        insert_u8(ins, 2);
        insert_u8(ins, 3);
        insert_column_end(&column_state);

        ins = insert_column_begin(&column_state, &in, COLUMN_BOOLEAN, 10);
        insert_true(ins);
        insert_true(ins);
        insert_true(ins);
        insert_column_end(&column_state);

        ins = insert_column_begin(&column_state, &in, COLUMN_BOOLEAN, 10);
        insert_false(ins);
        insert_false(ins);
        insert_false(ins);
        insert_column_end(&column_state);

        ins = insert_column_begin(&column_state, &in, COLUMN_U8, 10);
        insert_u8(ins, 1);
        insert_u8(ins, U8_NULL);
        insert_u8(ins, 3);
        insert_column_end(&column_state);

        ins = insert_column_begin(&column_state, &in, COLUMN_U16, 10);
        insert_u16(ins, 4);
        insert_u16(ins, U16_NULL);
        insert_u16(ins, 6);
        insert_column_end(&column_state);

        ins = insert_column_begin(&column_state, &in, COLUMN_U32, 10);
        insert_u32(ins, 7);
        insert_u32(ins, U32_NULL);
        insert_u32(ins, 9);
        insert_column_end(&column_state);

        ins = insert_column_begin(&column_state, &in, COLUMN_U64, 10);
        insert_u64(ins, 10);
        insert_u64(ins, U64_NULL);
        insert_u64(ins, 12);
        insert_column_end(&column_state);

        ins = insert_column_begin(&column_state, &in, COLUMN_I8, 10);
        insert_i8(ins, -1);
        insert_i8(ins, I8_NULL);
        insert_i8(ins, -3);
        insert_column_end(&column_state);

        ins = insert_column_begin(&column_state, &in, COLUMN_I16, 10);
        insert_i16(ins, -4);
        insert_i16(ins, I16_NULL);
        insert_i16(ins, -6);
        insert_column_end(&column_state);

        ins = insert_column_begin(&column_state, &in, COLUMN_I32, 10);
        insert_i32(ins, -7);
        insert_i32(ins, I32_NULL);
        insert_i32(ins, -9);
        insert_column_end(&column_state);

        ins = insert_column_begin(&column_state, &in, COLUMN_I64, 10);
        insert_i64(ins, -10);
        insert_i64(ins, I64_NULL);
        insert_i64(ins, -12);
        insert_column_end(&column_state);

        ins = insert_column_begin(&column_state, &in, COLUMN_FLOAT, 10);
        insert_float(ins, 42.0f);
        insert_float(ins, CARBON_NULL_FLOAT);
        insert_float(ins, 23.4221f);
        insert_column_end(&column_state);

        arr_it_drop(&it);
        revise_end(&revise);

        //rec_hexdump_print(stdout, &rev_doc);

        // rec_print(stdout, &rev_doc);
        str_buf sb;
        str_buf_create(&sb);
        rec_to_json(&sb, &rev_doc);
        ASSERT_TRUE(0 == strcmp(str_buf_cstr(&sb), "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [[1, 2, 3], [true, true, true], [false, false, false], [1, null, 3], [4, null, 6], [7, null, 9], [10, null, 12], [-1, null, -3], [-4, null, -6], [-7, null, -9], [-10, null, -12], [42.00, null, 23.42]]}"));
        str_buf_drop(&sb);

        rec_drop(&doc);
        rec_drop(&rev_doc);
}

TEST(CarbonTest, CarbonShrinkColumnListTest) {
        rec doc, rev_doc;
        rev revise;
        arr_it it;
        insert in;
        col_state column_state;
        insert *ins;

        rec_create_empty_ex(&doc, LIST_UNSORTED_MULTISET, KEY_NOKEY,20, 1);

        revise_begin(&revise, &rev_doc, &doc);
        revise_iterator_open(&it, &revise);
        arr_it_insert_begin(&in, &it);

        ins = insert_column_begin(&column_state, &in, COLUMN_U8, 10);
        insert_u8(ins, 1);
        insert_u8(ins, 2);
        insert_u8(ins, 3);
        insert_column_end(&column_state);

        ins = insert_column_begin(&column_state, &in, COLUMN_BOOLEAN, 10);
        insert_true(ins);
        insert_true(ins);
        insert_true(ins);
        insert_column_end(&column_state);

        ins = insert_column_begin(&column_state, &in, COLUMN_BOOLEAN, 10);
        insert_false(ins);
        insert_false(ins);
        insert_false(ins);
        insert_column_end(&column_state);

        ins = insert_column_begin(&column_state, &in, COLUMN_U8, 10);
        insert_u8(ins, 1);
        insert_u8(ins, U8_NULL);
        insert_u8(ins, 2);
        insert_column_end(&column_state);

        ins = insert_column_begin(&column_state, &in, COLUMN_U16, 10);
        insert_u16(ins, 3);
        insert_u16(ins, U16_NULL);
        insert_u16(ins, 4);
        insert_column_end(&column_state);

        ins = insert_column_begin(&column_state, &in, COLUMN_U32, 10);
        insert_u32(ins, 5);
        insert_u32(ins, U32_NULL);
        insert_u32(ins, 6);
        insert_column_end(&column_state);

        ins = insert_column_begin(&column_state, &in, COLUMN_U64, 10);
        insert_u64(ins, 7);
        insert_u64(ins, U64_NULL);
        insert_u64(ins, 8);
        insert_column_end(&column_state);

        ins = insert_column_begin(&column_state, &in, COLUMN_I8, 10);
        insert_i8(ins, 9);
        insert_i8(ins, I8_NULL);
        insert_i8(ins, 10);
        insert_column_end(&column_state);

        ins = insert_column_begin(&column_state, &in, COLUMN_I16, 10);
        insert_i16(ins, 11);
        insert_i16(ins, I16_NULL);
        insert_i16(ins, 12);
        insert_column_end(&column_state);

        ins = insert_column_begin(&column_state, &in, COLUMN_I32, 10);
        insert_i32(ins, 13);
        insert_i32(ins, I32_NULL);
        insert_i32(ins, 14);
        insert_column_end(&column_state);

        ins = insert_column_begin(&column_state, &in, COLUMN_I64, 10);
        insert_i64(ins, 15);
        insert_i64(ins, I64_NULL);
        insert_i64(ins, 16);
        insert_column_end(&column_state);

        //rec_hexdump_print(stdout, &rev_doc);
        revise_shrink(&revise);
        //rec_hexdump_print(stdout, &rev_doc);

        arr_it_drop(&it);
        revise_end(&revise);

        //rec_hexdump_print(stdout, &rev_doc);

        // rec_print(stdout, &rev_doc);
        str_buf sb;
        str_buf_create(&sb);
        rec_to_json(&sb, &rev_doc);
        ASSERT_TRUE(0 == strcmp(str_buf_cstr(&sb), "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [[1, 2, 3], [true, true, true], [false, false, false], [1, null, 2], [3, null, 4], [5, null, 6], [7, null, 8], [9, null, 10], [11, null, 12], [13, null, 14], [15, null, 16]]}"));
        str_buf_drop(&sb);

        rec_drop(&doc);
        rec_drop(&rev_doc);
}

TEST(CarbonTest, CarbonShrinkArrayListTest) {
        rec doc, rev_doc;
        rev revise;
        arr_it it;
        insert in;
        arr_state array_state;
        insert *ins;

        rec_create_empty_ex(&doc, LIST_UNSORTED_MULTISET, KEY_NOKEY,20, 1);

        revise_begin(&revise, &rev_doc, &doc);
        revise_iterator_open(&it, &revise);
        arr_it_insert_begin(&in, &it);

        ins = insert_array_begin(&array_state, &in, 10);
        insert_u8(ins, 1);
        insert_u8(ins, 1);
        insert_u8(ins, 1);
        insert_array_end(&array_state);

        ins = insert_array_begin(&array_state, &in, 10);
        insert_u8(ins, 2);
        insert_u8(ins, 3);
        insert_u8(ins, 4);
        insert_array_end(&array_state);

        ins = insert_array_begin(&array_state, &in, 10);
        insert_u8(ins, 5);
        insert_u8(ins, 6);
        insert_u8(ins, 7);
        insert_array_end(&array_state);

        //rec_hexdump_print(stdout, &rev_doc);
        revise_shrink(&revise);
        //rec_hexdump_print(stdout, &rev_doc);

        arr_it_drop(&it);
        revise_end(&revise);

        //rec_hexdump_print(stdout, &rev_doc);

        // rec_print(stdout, &rev_doc);
        str_buf sb;
        str_buf_create(&sb);
        rec_to_json(&sb, &rev_doc);
        ASSERT_TRUE(0 == strcmp(str_buf_cstr(&sb), "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [[1, 1, 1], [2, 3, 4], [5, 6, 7]]}"));
        str_buf_drop(&sb);

        rec_drop(&doc);
        rec_drop(&rev_doc);
}

TEST(CarbonTest, CarbonShrinkNestedArrayListTest) {
        rec doc, rev_doc;
        rev revise;
        arr_it it;
        insert in;
        arr_state array_state, nested_array_state;
        insert *ins, *nested_ins;

        rec_create_empty_ex(&doc, LIST_UNSORTED_MULTISET, KEY_NOKEY,20, 1);

        revise_begin(&revise, &rev_doc, &doc);
        revise_iterator_open(&it, &revise);
        arr_it_insert_begin(&in, &it);

        ins = insert_array_begin(&array_state, &in, 10);
        nested_ins = insert_array_begin(&nested_array_state, ins, 10);
        insert_string(nested_ins, "Hello");
        insert_string(nested_ins, "World");
        insert_array_end(&nested_array_state);
        insert_u8(ins, 1);
        insert_u8(ins, 1);
        insert_u8(ins, 1);
        insert_array_end(&array_state);

        ins = insert_array_begin(&array_state, &in, 10);
        insert_u8(ins, 2);
        nested_ins = insert_array_begin(&nested_array_state, ins, 10);
        insert_string(nested_ins, "Hello");
        insert_string(nested_ins, "World");
        insert_array_end(&nested_array_state);
        insert_u8(ins, 3);
        insert_u8(ins, 4);
        insert_array_end(&array_state);

        ins = insert_array_begin(&array_state, &in, 10);
        insert_u8(ins, 5);
        insert_u8(ins, 6);
        nested_ins = insert_array_begin(&nested_array_state, ins, 10);
        insert_string(nested_ins, "Hello");
        insert_string(nested_ins, "World");
        insert_array_end(&nested_array_state);
        insert_u8(ins, 7);
        insert_array_end(&array_state);

        ins = insert_array_begin(&array_state, &in, 10);
        insert_u8(ins, 8);
        insert_u8(ins, 9);
        insert_u8(ins, 10);
        nested_ins = insert_array_begin(&nested_array_state, ins, 10);
        insert_string(nested_ins, "Hello");
        insert_string(nested_ins, "World");
        insert_array_end(&nested_array_state);
        insert_array_end(&array_state);

        //rec_hexdump_print(stdout, &rev_doc);
        revise_shrink(&revise);
        //rec_hexdump_print(stdout, &rev_doc);

        arr_it_drop(&it);
        revise_end(&revise);

        //rec_hexdump_print(stdout, &rev_doc);

        // rec_print(stdout, &rev_doc);
        str_buf sb;
        str_buf_create(&sb);
        rec_to_json(&sb, &rev_doc);
        ASSERT_TRUE(0 == strcmp(str_buf_cstr(&sb), "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [[[\"Hello\", \"World\"], 1, 1, 1], [2, [\"Hello\", \"World\"], 3, 4], [5, 6, [\"Hello\", \"World\"], 7], [8, 9, 10, [\"Hello\", \"World\"]]]}"));
        str_buf_drop(&sb);

        rec_drop(&doc);
        rec_drop(&rev_doc);
}

TEST(CarbonTest, CarbonShrinkNestedArrayListAndColumnListTest) {
        rec doc, rev_doc;
        rev revise;
        arr_it it;
        insert in;
        col_state column_state;
        arr_state array_state, nested_array_state;
        insert *ins, *nested_ins, *column_ins;

        rec_create_empty_ex(&doc, LIST_UNSORTED_MULTISET, KEY_NOKEY,20, 1);

        revise_begin(&revise, &rev_doc, &doc);
        revise_iterator_open(&it, &revise);
        arr_it_insert_begin(&in, &it);

        insert_u64(&in, 4223);
        ins = insert_array_begin(&array_state, &in, 10);
        column_ins = insert_column_begin(&column_state, ins, COLUMN_U32, 10);
        insert_u32(column_ins, 'X');
        insert_u32(column_ins, 'Y');
        insert_u32(column_ins, 'Z');
        insert_column_end(&column_state);
        nested_ins = insert_array_begin(&nested_array_state, ins, 10);
        insert_string(nested_ins, "Hello");
        column_ins = insert_column_begin(&column_state, nested_ins, COLUMN_U32, 10);
        insert_u32(column_ins, 'A');
        insert_u32(column_ins, 'B');
        insert_u32(column_ins, 'C');
        insert_column_end(&column_state);
        insert_string(nested_ins, "World");
        insert_array_end(&nested_array_state);
        insert_u8(ins, 1);
        insert_u8(ins, 1);
        column_ins = insert_column_begin(&column_state, ins, COLUMN_U32, 10);
        insert_u32(column_ins, 23);
        insert_u32(column_ins, 24);
        insert_u32(column_ins, 25);
        insert_column_end(&column_state);
        insert_u8(ins, 1);
        insert_array_end(&array_state);

        //rec_hexdump_print(stdout, &rev_doc);
        revise_shrink(&revise);
        //rec_hexdump_print(stdout, &rev_doc);

        arr_it_drop(&it);
        revise_end(&revise);

        //rec_hexdump_print(stdout, &rev_doc);

        // rec_print(stdout, &rev_doc);

        str_buf sb;
        str_buf_create(&sb);
        rec_to_json(&sb, &rev_doc);

        // fprintf(stdout, "IST  %s\n", string_buffer_cstr(&sb));
        // fprintf(stdout, "SOLL {\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [4223, [[88, 89, 90], [\"Hello\", [65, 66, 67], \"World\"], 1, 1, [23, 24, 25], 1]]}\n");

        ASSERT_TRUE(0 == strcmp(str_buf_cstr(&sb), "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [4223, [[88, 89, 90], [\"Hello\", [65, 66, 67], \"World\"], 1, 1, [23, 24, 25], 1]]}"));
        str_buf_drop(&sb);

        rec_drop(&doc);
        rec_drop(&rev_doc);
}

TEST(CarbonTest, CarbonDotNotation) {
        dot path;
        str_buf sb;
        str_buf_create(&sb);

        dot_create(&path);

        dot_to_str(&sb, &path);
        ASSERT_TRUE(strcmp(str_buf_cstr(&sb), "") == 0);
        str_buf_clear(&sb);

        dot_add_key(&path, "name");
        dot_to_str(&sb, &path);
        ASSERT_TRUE(strcmp(str_buf_cstr(&sb), "name") == 0);
        str_buf_clear(&sb);

        dot_add_key(&path, "my name");
        dot_to_str(&sb, &path);
        ASSERT_TRUE(strcmp(str_buf_cstr(&sb), "name.\"my name\"") == 0);
        str_buf_clear(&sb);

        dot_add_key(&path, "");
        dot_to_str(&sb, &path);
        ASSERT_TRUE(strcmp(str_buf_cstr(&sb), "name.\"my name\".\"\"") == 0);
        str_buf_clear(&sb);

        dot_add_idx(&path, 42);
        dot_to_str(&sb, &path);
        ASSERT_TRUE(strcmp(str_buf_cstr(&sb), "name.\"my name\".\"\".42") == 0);
        str_buf_clear(&sb);

        dot_add_idx(&path, 23);
        dot_to_str(&sb, &path);
        ASSERT_TRUE(strcmp(str_buf_cstr(&sb), "name.\"my name\".\"\".42.23") == 0);
        str_buf_clear(&sb);

        dot_add_key(&path, "\"already quotes\"");
        dot_to_str(&sb, &path);
        ASSERT_TRUE(strcmp(str_buf_cstr(&sb), "name.\"my name\".\"\".42.23.\"already quotes\"") == 0);
        str_buf_clear(&sb);

        dot_drop(&path);
        str_buf_drop(&sb);
}

TEST(CarbonTest, CarbonDotNotationParsing) {
        dot path;
        str_buf sb;
        str_buf_create(&sb);

        dot_from_string(&path, "name");
        dot_to_str(&sb, &path);
        ASSERT_TRUE(strcmp(str_buf_cstr(&sb), "name") == 0);
        str_buf_clear(&sb);
        dot_drop(&path);

        dot_from_string(&path, "   name");
        dot_to_str(&sb, &path);
        ASSERT_TRUE(strcmp(str_buf_cstr(&sb), "name") == 0);
        str_buf_clear(&sb);
        dot_drop(&path);

        dot_from_string(&path, "   name    ");
        dot_to_str(&sb, &path);
        ASSERT_TRUE(strcmp(str_buf_cstr(&sb), "name") == 0);
        str_buf_clear(&sb);
        dot_drop(&path);

        dot_from_string(&path, "");
        dot_to_str(&sb, &path);
        ASSERT_TRUE(strcmp(str_buf_cstr(&sb), "") == 0);
        str_buf_clear(&sb);
        dot_drop(&path);

        dot_from_string(&path, "\"name\"");
        dot_to_str(&sb, &path);
        ASSERT_TRUE(strcmp(str_buf_cstr(&sb), "name") == 0);
        str_buf_clear(&sb);
        dot_drop(&path);

        dot_from_string(&path, "\"nam e\"");
        dot_to_str(&sb, &path);
        ASSERT_TRUE(strcmp(str_buf_cstr(&sb), "\"nam e\"") == 0);
        str_buf_clear(&sb);
        dot_drop(&path);

        dot_from_string(&path, "nam e");
        dot_to_str(&sb, &path);
        ASSERT_TRUE(strcmp(str_buf_cstr(&sb), "nam.e") == 0);
        str_buf_clear(&sb);
        dot_drop(&path);

        dot_from_string(&path, "\"My Doc\" names 5 age");
        dot_to_str(&sb, &path);
        ASSERT_TRUE(strcmp(str_buf_cstr(&sb), "\"My Doc\".names.5.age") == 0);
        str_buf_clear(&sb);
        dot_drop(&path);

        dot_from_string(&path, "23.authors.3.name");
        dot_to_str(&sb, &path);
        ASSERT_TRUE(strcmp(str_buf_cstr(&sb), "23.authors.3.name") == 0);
        str_buf_clear(&sb);
        dot_drop(&path);

        str_buf_drop(&sb);
}

TEST(CarbonTest, CarbonFind) {
        rec doc, rev_doc;
        rev revise;
        arr_it it;
        insert ins;
        find finder;
        u64 result_unsigned;
        field_e type;
        rec_create_empty(&doc, LIST_UNSORTED_MULTISET, KEY_NOKEY);

        revise_begin(&revise, &rev_doc, &doc);

        revise_iterator_open(&it, &revise);
        arr_it_insert_begin(&ins, &it);
        insert_u8(&ins, 'a');
        insert_u8(&ins, 'b');
        insert_u8(&ins, 'c');
        arr_it_insert_end(&ins);
        revise_iterator_close(&it);

        revise_end(&revise);

        {
                find_begin(&finder, "0", &rev_doc);

                ASSERT_TRUE(find_has_result(&finder));

                find_result_type(&type, &finder);
                ASSERT_EQ(type, FIELD_NUMBER_U8);

                find_result_unsigned(&result_unsigned, &finder);
                ASSERT_EQ(result_unsigned, (u64) 'a');

                find_end(&finder);
        }

        {
                find_begin(&finder, "1", &rev_doc);

                ASSERT_TRUE(find_has_result(&finder));

                find_result_type(&type, &finder);
                ASSERT_EQ(type, FIELD_NUMBER_U8);

                find_result_unsigned(&result_unsigned, &finder);
                ASSERT_EQ(result_unsigned, (u64) 'b');

                find_end(&finder);
        }

        {
                find_begin(&finder, "2", &rev_doc);

                ASSERT_TRUE(find_has_result(&finder));

                find_result_type(&type, &finder);
                ASSERT_EQ(type, FIELD_NUMBER_U8);

                find_result_unsigned(&result_unsigned, &finder);
                ASSERT_EQ(result_unsigned, (u64) 'c');

                find_end(&finder);
        }

        {
                find_begin(&finder, "3", &rev_doc);

                ASSERT_FALSE(find_has_result(&finder));

                find_end(&finder);
        }

        // rec_print(stdout, &rev_doc);
        rec_drop(&doc);
        rec_drop(&rev_doc);
}

TEST(CarbonTest, CarbonFindTypes) {
        rec doc, rev_doc;
        rev revise;
        arr_it it;
        insert in, *ins, *nested_ins, *column_ins;
        col_state column_state;
        arr_state array_state, nested_array_state;
        find finder;
        u64 result_unsigned;
        field_e type;
        rec_create_empty(&doc, LIST_UNSORTED_MULTISET, KEY_NOKEY);

        revise_begin(&revise, &rev_doc, &doc);
        revise_iterator_open(&it, &revise);
        arr_it_insert_begin(&in, &it);

        insert_u64(&in, 4223);
        ins = insert_array_begin(&array_state, &in, 10);
        column_ins = insert_column_begin(&column_state, ins, COLUMN_U32, 10);
        insert_u32(column_ins, 'X');
        insert_u32(column_ins, 'Y');
        insert_u32(column_ins, 'Z');
        insert_column_end(&column_state);
        nested_ins = insert_array_begin(&nested_array_state, ins, 10);
        insert_string(nested_ins, "Hello");
        column_ins = insert_column_begin(&column_state, nested_ins, COLUMN_U32, 10);
        insert_u32(column_ins, 'A');
        insert_u32(column_ins, 'B');
        insert_u32(column_ins, 'C');
        insert_column_end(&column_state);
        insert_string(nested_ins, "World");
        insert_array_end(&nested_array_state);
        insert_u8(ins, 1);
        insert_u8(ins, 1);
        column_ins = insert_column_begin(&column_state, ins, COLUMN_U32, 10);
        insert_u32(column_ins, 23);
        insert_u32(column_ins, 24);
        insert_u32(column_ins, 25);
        insert_column_end(&column_state);
        insert_u8(ins, 1);
        insert_array_end(&array_state);

        revise_shrink(&revise);


        //rec_print(stdout, &rev_doc);

        {
                find_begin(&finder, "0", &rev_doc);
                ASSERT_TRUE(find_has_result(&finder));
                find_result_type(&type, &finder);
                ASSERT_EQ(type, FIELD_NUMBER_U64);
                find_result_unsigned(&result_unsigned, &finder);
                ASSERT_EQ(result_unsigned, 4223U);
                find_end(&finder);
        }

        {
                find_begin(&finder, "1", &rev_doc);
                ASSERT_TRUE(find_has_result(&finder));
                find_result_type(&type, &finder);
                ASSERT_EQ(type, FIELD_ARRAY_UNSORTED_MULTISET);
                arr_it *retval = find_result_array(&finder);
                ASSERT_TRUE(retval != NULL);
                find_end(&finder);
        }

        {
                find_begin(&finder, "1.0", &rev_doc);
                ASSERT_TRUE(find_has_result(&finder));
                find_result_type(&type, &finder);
                ASSERT_TRUE(type == FIELD_COLUMN_U8_UNSORTED_MULTISET ||
                            type == FIELD_COLUMN_U16_UNSORTED_MULTISET ||
                            type == FIELD_COLUMN_U32_UNSORTED_MULTISET ||
                            type == FIELD_COLUMN_U64_UNSORTED_MULTISET ||
                            type == FIELD_COLUMN_I8_UNSORTED_MULTISET ||
                            type == FIELD_COLUMN_I16_UNSORTED_MULTISET ||
                            type == FIELD_COLUMN_I32_UNSORTED_MULTISET ||
                            type == FIELD_COLUMN_I64_UNSORTED_MULTISET ||
                            type == FIELD_COLUMN_FLOAT_UNSORTED_MULTISET ||
                            type == FIELD_COLUMN_BOOLEAN_UNSORTED_MULTISET);
                col_it *retval = find_result_column(&finder);
                ASSERT_TRUE(retval != NULL);
                find_end(&finder);
        }

        {
                find_begin(&finder, "1.0.0", &rev_doc);
                ASSERT_TRUE(find_has_result(&finder));
                find_result_type(&type, &finder);
                ASSERT_EQ(type, FIELD_NUMBER_U32);
                find_result_unsigned(&result_unsigned, &finder);
                ASSERT_EQ(result_unsigned, 88U);
                find_end(&finder);
        }

        {
                find_begin(&finder, "1.0.1", &rev_doc);
                ASSERT_TRUE(find_has_result(&finder));
                find_result_type(&type, &finder);
                ASSERT_EQ(type, FIELD_NUMBER_U32);
                find_result_unsigned(&result_unsigned, &finder);
                ASSERT_EQ(result_unsigned, 89U);
                find_end(&finder);
        }

        {
                find_begin(&finder, "1.0.2", &rev_doc);
                ASSERT_TRUE(find_has_result(&finder));
                find_result_type(&type, &finder);
                ASSERT_EQ(type, FIELD_NUMBER_U32);
                find_result_unsigned(&result_unsigned, &finder);
                ASSERT_EQ(result_unsigned, 90U);
                find_end(&finder);
        }

        {
                find_begin(&finder, "1.0.3", &rev_doc);
                ASSERT_FALSE(find_has_result(&finder));
                find_end(&finder);
        }

        {
                find_begin(&finder, "1.1", &rev_doc);
                ASSERT_TRUE(find_has_result(&finder));
                find_result_type(&type, &finder);
                ASSERT_EQ(type, FIELD_ARRAY_UNSORTED_MULTISET);
                arr_it *retval = find_result_array(&finder);
                ASSERT_TRUE(retval != NULL);
                find_end(&finder);
        }

        {
                find_begin(&finder, "1.1.0", &rev_doc);
                ASSERT_TRUE(find_has_result(&finder));
                find_result_type(&type, &finder);
                ASSERT_EQ(type, FIELD_STRING);
                u64 str_len;
                const char *retval = find_result_string(&str_len, &finder);
                ASSERT_TRUE(strncmp(retval, "Hello", str_len) == 0);
                find_end(&finder);
        }

        {
                find_begin(&finder, "1.1.1", &rev_doc);
                ASSERT_TRUE(find_has_result(&finder));
                find_result_type(&type, &finder);
                ASSERT_TRUE(type == FIELD_COLUMN_U8_UNSORTED_MULTISET ||
                            type == FIELD_COLUMN_U16_UNSORTED_MULTISET ||
                            type == FIELD_COLUMN_U32_UNSORTED_MULTISET ||
                            type == FIELD_COLUMN_U64_UNSORTED_MULTISET ||
                            type == FIELD_COLUMN_I8_UNSORTED_MULTISET ||
                            type == FIELD_COLUMN_I16_UNSORTED_MULTISET ||
                            type == FIELD_COLUMN_I32_UNSORTED_MULTISET ||
                            type == FIELD_COLUMN_I64_UNSORTED_MULTISET ||
                            type == FIELD_COLUMN_FLOAT_UNSORTED_MULTISET ||
                            type == FIELD_COLUMN_BOOLEAN_UNSORTED_MULTISET);
                col_it *retval = find_result_column(&finder);
                ASSERT_TRUE(retval != NULL);
                find_end(&finder);
        }

        {
                find_begin(&finder, "1.1.1.0", &rev_doc);
                ASSERT_TRUE(find_has_result(&finder));
                find_result_type(&type, &finder);
                ASSERT_EQ(type, FIELD_NUMBER_U32);
                find_result_unsigned(&result_unsigned, &finder);
                ASSERT_EQ(result_unsigned, 65U);
                find_end(&finder);
        }

        {
                find_begin(&finder, "1.1.1.1", &rev_doc);
                ASSERT_TRUE(find_has_result(&finder));
                find_result_type(&type, &finder);
                ASSERT_EQ(type, FIELD_NUMBER_U32);
                find_result_unsigned(&result_unsigned, &finder);
                ASSERT_EQ(result_unsigned, 66U);
                find_end(&finder);
        }

        {
                find_begin(&finder, "1.1.1.2", &rev_doc);
                ASSERT_TRUE(find_has_result(&finder));
                find_result_type(&type, &finder);
                ASSERT_EQ(type, FIELD_NUMBER_U32);
                find_result_unsigned(&result_unsigned, &finder);
                ASSERT_EQ(result_unsigned, 67U);
                find_end(&finder);
        }

        {
                find_begin(&finder, "1.1.1.3", &rev_doc);
                ASSERT_FALSE(find_has_result(&finder));
        }

        {
                find_begin(&finder, "1.1.2", &rev_doc);
                ASSERT_TRUE(find_has_result(&finder));
                find_result_type(&type, &finder);
                ASSERT_EQ(type, FIELD_STRING);
                u64 str_len;
                const char *retval = find_result_string(&str_len, &finder);
                ASSERT_TRUE(strncmp(retval, "World", str_len) == 0);
                find_end(&finder);
        }

        {
                find_begin(&finder, "1.1.3", &rev_doc);
                ASSERT_FALSE(find_has_result(&finder));
                find_end(&finder);
        }

        {
                find_begin(&finder, "1.2", &rev_doc);
                ASSERT_TRUE(find_has_result(&finder));
                find_result_type(&type, &finder);
                ASSERT_EQ(type, FIELD_NUMBER_U8);
                find_result_unsigned(&result_unsigned, &finder);
                ASSERT_EQ(result_unsigned, 1U);
                find_end(&finder);
        }

        {
                find_begin(&finder, "1.3", &rev_doc);
                ASSERT_TRUE(find_has_result(&finder));
                find_result_type(&type, &finder);
                ASSERT_EQ(type, FIELD_NUMBER_U8);
                find_result_unsigned(&result_unsigned, &finder);
                ASSERT_EQ(result_unsigned, 1U);
                find_end(&finder);
        }

        {
                find_begin(&finder, "1.4", &rev_doc);
                ASSERT_TRUE(find_has_result(&finder));
                find_result_type(&type, &finder);
                ASSERT_TRUE(type == FIELD_COLUMN_U8_UNSORTED_MULTISET ||
                            type == FIELD_COLUMN_U16_UNSORTED_MULTISET ||
                            type == FIELD_COLUMN_U32_UNSORTED_MULTISET ||
                            type == FIELD_COLUMN_U64_UNSORTED_MULTISET ||
                            type == FIELD_COLUMN_I8_UNSORTED_MULTISET ||
                            type == FIELD_COLUMN_I16_UNSORTED_MULTISET ||
                            type == FIELD_COLUMN_I32_UNSORTED_MULTISET ||
                            type == FIELD_COLUMN_I64_UNSORTED_MULTISET ||
                            type == FIELD_COLUMN_FLOAT_UNSORTED_MULTISET ||
                            type == FIELD_COLUMN_BOOLEAN_UNSORTED_MULTISET);
                col_it *retval = find_result_column(&finder);
                ASSERT_TRUE(retval != NULL);
                find_end(&finder);
        }

        {
                find_begin(&finder, "1.4.0", &rev_doc);
                ASSERT_TRUE(find_has_result(&finder));
                find_result_type(&type, &finder);
                ASSERT_EQ(type, FIELD_NUMBER_U32);
                find_result_unsigned(&result_unsigned, &finder);
                ASSERT_EQ(result_unsigned, 23U);
                find_end(&finder);
        }

        {
                find_begin(&finder, "1.4.1", &rev_doc);
                ASSERT_TRUE(find_has_result(&finder));
                find_result_type(&type, &finder);
                ASSERT_EQ(type, FIELD_NUMBER_U32);
                find_result_unsigned(&result_unsigned, &finder);
                ASSERT_EQ(result_unsigned, 24U);
                find_end(&finder);
        }

        {
                find_begin(&finder, "1.4.2", &rev_doc);
                ASSERT_TRUE(find_has_result(&finder));
                find_result_type(&type, &finder);
                ASSERT_EQ(type, FIELD_NUMBER_U32);
                find_result_unsigned(&result_unsigned, &finder);
                ASSERT_EQ(result_unsigned, 25U);
                find_end(&finder);
        }

        {
                find_begin(&finder, "1.4.3", &rev_doc);
                ASSERT_FALSE(find_has_result(&finder));
                find_end(&finder);
        }

        {
                find_begin(&finder, "1.5", &rev_doc);
                ASSERT_TRUE(find_has_result(&finder));
                find_result_type(&type, &finder);
                ASSERT_EQ(type, FIELD_NUMBER_U8);
                find_result_unsigned(&result_unsigned, &finder);
                ASSERT_EQ(result_unsigned, 1U);
                find_end(&finder);
        }

        {
                find_begin(&finder, "1.6", &rev_doc);
                ASSERT_FALSE(find_has_result(&finder));
                find_end(&finder);
        }

        arr_it_drop(&it);
        revise_end(&revise);

        //rec_hexdump_print(stdout, &rev_doc);

        rec_drop(&rev_doc);
        rec_drop(&doc);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}