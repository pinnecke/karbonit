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

        ASSERT_TRUE(strcmp(json_1, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [{\"1\": false, \"2\": null, \"3\": 1, \"4\": \"v1\", \"5\": 2}]}") == 0);
        ASSERT_TRUE(strcmp(json_2, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [{\"1\": false, \"2\": null, \"3\": 1, \"4\": \"v1\", \"5\": 2, \"Hello Long Key\": \"Hello Long Value\"}]}") == 0);

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

        ASSERT_TRUE(strcmp(json_1, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [{\"1\": false, \"2\": null, \"3\": 1, \"4\": \"v1\", \"5\": 2}]}") == 0);
        ASSERT_TRUE(strcmp(json_2, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [{\"2\": null, \"3\": 1, \"4\": \"v1\", \"5\": 2}]}") == 0);

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

        ASSERT_TRUE(strcmp(json_1, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [{\"1\": {\"2\": null, \"3\": 1, \"4\": \"v1\", \"5\": 2}, \"6\": null, \"7\": 1, \"8\": \"v1\", \"9\": 2}]}") == 0);
        ASSERT_TRUE(strcmp(json_2, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [{\"6\": null, \"7\": 1, \"8\": \"v1\", \"9\": 2}]}") == 0);

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

        ASSERT_TRUE(strcmp(json_1, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [{\"1\": [], \"2\": null, \"3\": 1, \"4\": \"v1\", \"5\": 2}]}") == 0);
        ASSERT_TRUE(strcmp(json_2, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [{\"2\": null, \"3\": 1, \"4\": \"v1\", \"5\": 2}]}") == 0);

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

        ASSERT_TRUE(strcmp(json_1, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [{\"1\": [null, 1, \"v1\", 2], \"2\": null, \"3\": 1, \"4\": \"v1\", \"5\": 2}]}") == 0);
        ASSERT_TRUE(strcmp(json_2, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [{\"2\": null, \"3\": 1, \"4\": \"v1\", \"5\": 2}]}") == 0);

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

        ASSERT_TRUE(strcmp(json_1, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [{\"1\": [], \"2\": null, \"3\": 1, \"4\": \"v1\", \"5\": 2}]}") == 0);
        ASSERT_TRUE(strcmp(json_2, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [{\"2\": null, \"3\": 1, \"4\": \"v1\", \"5\": 2}]}") == 0);

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

        ASSERT_TRUE(strcmp(json_1, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [{\"1\": {}, \"2\": null, \"3\": 1, \"4\": \"v1\", \"5\": 2}]}") == 0);
        ASSERT_TRUE(strcmp(json_2, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [{\"2\": null, \"3\": 1, \"4\": \"v1\", \"5\": 2}]}") == 0);

        str_buf_drop(&sb);
        rec_drop(&doc);
        rec_drop(&rev_doc);
        free(json_1);
        free(json_2);
}

TEST(CarbonTest, CarbonUpdateSetToNull)
{
        str_buf sb;

        str_buf_create(&sb);

        /* Each time 'create_nested_doc' is called, the following document will be generated

                [
                   [
                      null,
                      true,
                      false,
                      8,
                      -16,
                      "Hello, World!",
                      {
                         "type":"text/plain",
                         "encoding":"base64",
                         "binary-str_buf":"TXkgUGxhaW4tVGV4dAAA"
                      },
                      {
                         "type":"own",
                         "encoding":"base64",
                         "binary-str_buf":"TXkgT3duIEZvcm1hdAAA"
                      },
                      [
                         32,
                         33,
                         34,
                         35
                      ],
                      [

                      ],
                      [
                         null,
                         true,
                         false,
                         8,
                         -16,
                         "Hello, World!",
                         {
                            "type":"text/plain",
                            "encoding":"base64",
                            "binary-str_buf":"TXkgUGxhaW4tVGV4dAAA"
                         },
                         {
                            "type":"own",
                            "encoding":"base64",
                            "binary-str_buf":"TXkgT3duIEZvcm1hdAAA"
                         },
                         [
                            32,
                            33,
                            34,
                            35
                         ]
                      ]
                   ],
                   [
                      null,
                      true,
                      false,
                      8,
                      -16,
                      "Hello, World!",
                      {
                         "type":"text/plain",
                         "encoding":"base64",
                         "binary-str_buf":"TXkgUGxhaW4tVGV4dAAA"
                      },
                      {
                         "type":"own",
                         "encoding":"base64",
                         "binary-str_buf":"TXkgT3duIEZvcm1hdAAA"
                      },
                      [
                         32,
                         33,
                         34,
                         35
                      ],
                      [

                      ],
                      [
                         null,
                         true,
                         false,
                         8,
                         -16,
                         "Hello, World!",
                         {
                            "type":"text/plain",
                            "encoding":"base64",
                            "binary-str_buf":"TXkgUGxhaW4tVGV4dAAA"
                         },
                         {
                            "type":"own",
                            "encoding":"base64",
                            "binary-str_buf":"TXkgT3duIEZvcm1hdAAA"
                         },
                         [
                            32,
                            33,
                            34,
                            35
                         ]
                      ]
                   ]
                ]

         */

//        // -------------------------------------------------------------------------------------------------------------
//        // Update to null
//        // -------------------------------------------------------------------------------------------------------------
//
//        create_nested_doc(&doc);
//        status = update_one_set_null("0.0", &rev_doc, &doc); // replaces null with null
//        ASSERT_TRUE(status);
//        // printf("built:  \t'%s'\n", carbon_to_json(&sb, &doc));
//        // printf("altered:\t'%s'\n", carbon_to_json(&sb, &rev_doc));
//        ASSERT_TRUE(strcmp(carbon_to_json(&sb, &rev_doc), "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [[null, true, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35], [], [null, true, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35]]], [null, true, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35], [], [null, true, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35]]]]}") == 0);
//        rec_drop(&doc);
//        rec_drop(&rev_doc);
//
//        create_nested_doc(&doc);
//        status = update_one_set_null("0.1", &rev_doc, &doc); // replaces true with null
//        ASSERT_TRUE(status);
//        // printf("built:  \t'%s'\n", carbon_to_json(&sb, &doc));
//        // printf("altered:\t'%s'\n", carbon_to_json(&sb, &rev_doc));
//        ASSERT_TRUE(strcmp(carbon_to_json(&sb, &rev_doc), "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [[null, null, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35], [], [null, true, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35]]], [null, true, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35], [], [null, true, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35]]]]}") == 0);
//        rec_drop(&doc);
//        rec_drop(&rev_doc);
//
//        create_nested_doc(&doc);
//        status = update_one_set_null("0.2", &rev_doc, &doc); // replaces false with null
//        ASSERT_TRUE(status);
//        // printf("built:  \t'%s'\n", carbon_to_json(&sb, &doc));
//        // printf("altered:\t'%s'\n", carbon_to_json(&sb, &rev_doc));
//        ASSERT_TRUE(strcmp(carbon_to_json(&sb, &rev_doc), "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [[null, true, null, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35], [], [null, true, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35]]], [null, true, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35], [], [null, true, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35]]]]}") == 0);
//        rec_drop(&doc);
//        rec_drop(&rev_doc);
//
//        create_nested_doc(&doc);
//        status = update_one_set_null("0.3", &rev_doc, &doc); // replaces u8 (8) with null
//        ASSERT_TRUE(status);
//        // printf("built:  \t'%s'\n", carbon_to_json(&sb, &doc));
//        // printf("altered:\t'%s'\n", carbon_to_json(&sb, &rev_doc));
//        ASSERT_TRUE(strcmp(carbon_to_json(&sb, &rev_doc), "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [[null, true, false, null, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35], [], [null, true, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35]]], [null, true, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35], [], [null, true, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35]]]]}") == 0);
//        rec_drop(&doc);
//        rec_drop(&rev_doc);
//
//        create_nested_doc(&doc);
//        status = update_one_set_null("0.4", &rev_doc, &doc); // replaces i16 (-16) with null
//        ASSERT_TRUE(status);
//        // printf("built:  \t'%s'\n", carbon_to_json(&sb, &doc));
//        // printf("altered:\t'%s'\n", carbon_to_json(&sb, &rev_doc));
//        ASSERT_TRUE(strcmp(carbon_to_json(&sb, &rev_doc), "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [[null, true, false, 8, null, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35], [], [null, true, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35]]], [null, true, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35], [], [null, true, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35]]]]}") == 0);
//        rec_drop(&doc);
//        rec_drop(&rev_doc);
//
//        create_nested_doc(&doc);
//        status = update_one_set_null("0.5", &rev_doc, &doc); // replaces str_buf with null
//        ASSERT_TRUE(status);
//        // printf("built:  \t'%s'\n", carbon_to_json(&sb, &doc));
//        // printf("altered:\t'%s'\n", carbon_to_json(&sb, &rev_doc));
//        ASSERT_TRUE(strcmp(carbon_to_json(&sb, &rev_doc), "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [[null, true, false, 8, -16, null, { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35], [], [null, true, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35]]], [null, true, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35], [], [null, true, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35]]]]}") == 0);
//        rec_drop(&doc);
//        rec_drop(&rev_doc);
//
//        create_nested_doc(&doc);
//        status = update_one_set_null("0.6", &rev_doc, &doc); // replaces binary str_buf with null
//        ASSERT_TRUE(status);
//        // printf("built:  \t'%s'\n", carbon_to_json(&sb, &doc));
//        // printf("altered:\t'%s'\n", carbon_to_json(&sb, &rev_doc));
//        ASSERT_TRUE(strcmp(carbon_to_json(&sb, &rev_doc), "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [[null, true, false, 8, -16, \"Hello, World!\", null, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35], [], [null, true, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35]]], [null, true, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35], [], [null, true, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35]]]]}") == 0);
//        rec_drop(&doc);
//        rec_drop(&rev_doc);
//
//        create_nested_doc(&doc);
//        status = update_one_set_null("0.7", &rev_doc, &doc); // replaces custom binary with null
//        ASSERT_TRUE(status);
//        // printf("built:  \t'%s'\n", carbon_to_json(&sb, &doc));
//        // printf("altered:\t'%s'\n", carbon_to_json(&sb, &rev_doc));
//        ASSERT_TRUE(strcmp(carbon_to_json(&sb, &rev_doc), "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [[null, true, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, null, [32, 33, 34, 35], [], [null, true, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35]]], [null, true, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35], [], [null, true, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35]]]]}") == 0);
//        rec_drop(&doc);
//        rec_drop(&rev_doc);
//
//        create_nested_doc(&doc);
//        status = update_one_set_null("0.8", &rev_doc, &doc); // replaces column ([32, 33, 34, 35]) with null
//        ASSERT_TRUE(status);
//        // printf("built:  \t'%s'\n", carbon_to_json(&sb, &doc));
//        // printf("altered:\t'%s'\n", carbon_to_json(&sb, &rev_doc));
//        ASSERT_TRUE(strcmp(carbon_to_json(&sb, &rev_doc), "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [[null, true, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, null, [], [null, true, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35]]], [null, true, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35], [], [null, true, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35]]]]}") == 0);
//        rec_drop(&doc);
//        rec_drop(&rev_doc);
//
//        create_nested_doc(&doc);
//        status = update_one_set_null("0.8.0", &rev_doc, &doc); // replaces element in column with null value (special case) --> [NULL, 33, 34, 35]
//        ASSERT_TRUE(status);
//        // printf("built:  \t'%s'\n", carbon_to_json(&sb, &doc));
//        // printf("altered:\t'%s'\n", carbon_to_json(&sb, &rev_doc));
//        ASSERT_TRUE(strcmp(carbon_to_json(&sb, &rev_doc), "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [[null, true, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [null, 33, 34, 35], [], [null, true, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35]]], [null, true, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35], [], [null, true, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35]]]]}") == 0);
//        rec_drop(&doc);
//        rec_drop(&rev_doc);
//
//        create_nested_doc(&doc);
//        status = update_one_set_null("0.9", &rev_doc, &doc); // replaces empty array with null
//        ASSERT_TRUE(status);
//        // printf("built:  \t'%s'\n", carbon_to_json(&sb, &doc));
//        // printf("altered:\t'%s'\n", carbon_to_json(&sb, &rev_doc));
//        ASSERT_TRUE(strcmp(carbon_to_json(&sb, &rev_doc), "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [[null, true, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35], null, [null, true, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35]]], [null, true, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35], [], [null, true, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35]]]]}") == 0);
//        rec_drop(&doc);
//        rec_drop(&rev_doc);
//
//        create_nested_doc(&doc);
//        status = update_one_set_null("0.10", &rev_doc, &doc); // replaces complex array with null
//        ASSERT_TRUE(status);
//        // printf("built:  \t'%s'\n", carbon_to_json(&sb, &doc));
//        // printf("altered:\t'%s'\n", carbon_to_json(&sb, &rev_doc));
//        ASSERT_TRUE(strcmp(carbon_to_json(&sb, &rev_doc), "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [[null, true, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35], [], null], [null, true, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35], [], [null, true, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35]]]]}") == 0);
//        rec_drop(&doc);
//        rec_drop(&rev_doc);
//
//        create_nested_doc(&doc);
//        status = update_one_set_null("0", &rev_doc, &doc); // replaces 1st outermost array with null
//        ASSERT_TRUE(status);
//        // printf("built:  \t'%s'\n", carbon_to_json(&sb, &doc));
//        // printf("altered:\t'%s'\n", carbon_to_json(&sb, &rev_doc));
//        ASSERT_TRUE(strcmp(carbon_to_json(&sb, &rev_doc), "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [null, [null, true, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35], [], [null, true, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35]]]]}") == 0);
//        rec_drop(&doc);
//        rec_drop(&rev_doc);
//
//        create_nested_doc(&doc);
//        status = update_one_set_null("1", &rev_doc, &doc); // replaces 2nd outermost array with null
//        ASSERT_TRUE(status);
//        // printf("built:  \t'%s'\n", carbon_to_json(&sb, &doc));
//        // printf("altered:\t'%s'\n", carbon_to_json(&sb, &rev_doc));
//        ASSERT_TRUE(strcmp(carbon_to_json(&sb, &rev_doc), "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [[null, true, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35], [], [null, true, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35]]], null]}") == 0);
//        rec_drop(&doc);
//        rec_drop(&rev_doc);
//
//        // -------------------------------------------------------------------------------------------------------------
//        // Update to true
//        // -------------------------------------------------------------------------------------------------------------
//
//        create_nested_doc(&doc);
//        status = update_one_set_true("0.0", &rev_doc, &doc); // replaces null with true
//        ASSERT_TRUE(status);
//        // printf("built:  \t'%s'\n", carbon_to_json(&sb, &doc));
//        // printf("altered:\t'%s'\n", carbon_to_json(&sb, &rev_doc));
//        ASSERT_TRUE(strcmp(carbon_to_json(&sb, &rev_doc), "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [[true, true, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35], [], [null, true, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35]]], [null, true, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35], [], [null, true, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35]]]]}") == 0);
//        rec_drop(&doc);
//        rec_drop(&rev_doc);
//
//        create_nested_doc(&doc);
//        status = update_one_set_true("0.1", &rev_doc, &doc); // replaces true with true
//        ASSERT_TRUE(status);
//        // printf("built:  \t'%s'\n", carbon_to_json(&sb, &doc));
//        // printf("altered:\t'%s'\n", carbon_to_json(&sb, &rev_doc));
//        ASSERT_TRUE(strcmp(carbon_to_json(&sb, &rev_doc), "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [[null, true, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35], [], [null, true, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35]]], [null, true, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35], [], [null, true, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35]]]]}") == 0);
//        rec_drop(&doc);
//        rec_drop(&rev_doc);
//
//        create_nested_doc(&doc);
//        status = update_one_set_true("0.2", &rev_doc, &doc); // replaces false with true
//        ASSERT_TRUE(status);
//        // printf("built:  \t'%s'\n", carbon_to_json(&sb, &doc));
//        // printf("altered:\t'%s'\n", carbon_to_json(&sb, &rev_doc));
//        ASSERT_TRUE(strcmp(carbon_to_json(&sb, &rev_doc), "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [[null, true, true, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35], [], [null, true, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35]]], [null, true, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35], [], [null, true, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35]]]]}") == 0);
//        rec_drop(&doc);
//        rec_drop(&rev_doc);
//
//        create_nested_doc(&doc);
//        status = update_one_set_true("0.3", &rev_doc, &doc); // replaces u8 (8) with true
//        ASSERT_TRUE(status);
//        // printf("built:  \t'%s'\n", carbon_to_json(&sb, &doc));
//        // printf("altered:\t'%s'\n", carbon_to_json(&sb, &rev_doc));
//        ASSERT_TRUE(strcmp(carbon_to_json(&sb, &rev_doc), "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [[null, true, false, true, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35], [], [null, true, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35]]], [null, true, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35], [], [null, true, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35]]]]}") == 0);
//        rec_drop(&doc);
//        rec_drop(&rev_doc);
//
//        create_nested_doc(&doc);
//        status = update_one_set_true("0.4", &rev_doc, &doc); // replaces i16 (-16) with true
//        ASSERT_TRUE(status);
//        // printf("built:  \t'%s'\n", carbon_to_json(&sb, &doc));
//        // printf("altered:\t'%s'\n", carbon_to_json(&sb, &rev_doc));
//        ASSERT_TRUE(strcmp(carbon_to_json(&sb, &rev_doc), "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [[null, true, false, 8, true, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35], [], [null, true, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35]]], [null, true, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35], [], [null, true, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35]]]]}") == 0);
//        rec_drop(&doc);
//        rec_drop(&rev_doc);
//
//        create_nested_doc(&doc);
//        status = update_one_set_true("0.5", &rev_doc, &doc); // replaces str_buf with true
//        ASSERT_TRUE(status);
//        // printf("built:  \t'%s'\n", carbon_to_json(&sb, &doc));
//        // printf("altered:\t'%s'\n", carbon_to_json(&sb, &rev_doc));
//        ASSERT_TRUE(strcmp(carbon_to_json(&sb, &rev_doc), "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [[null, true, false, 8, -16, true, { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35], [], [null, true, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35]]], [null, true, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35], [], [null, true, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35]]]]}") == 0);
//        rec_drop(&doc);
//        rec_drop(&rev_doc);
//
//        create_nested_doc(&doc);
//        status = update_one_set_true("0.6", &rev_doc, &doc); // replaces binary str_buf with true
//        ASSERT_TRUE(status);
//        // printf("built:  \t'%s'\n", carbon_to_json(&sb, &doc));
//        // printf("altered:\t'%s'\n", carbon_to_json(&sb, &rev_doc));
//        ASSERT_TRUE(strcmp(carbon_to_json(&sb, &rev_doc), "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [[null, true, false, 8, -16, \"Hello, World!\", true, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35], [], [null, true, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35]]], [null, true, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35], [], [null, true, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35]]]]}") == 0);
//        rec_drop(&doc);
//        rec_drop(&rev_doc);
//
//        create_nested_doc(&doc);
//        status = update_one_set_true("0.7", &rev_doc, &doc); // replaces custom binary with true
//        ASSERT_TRUE(status);
//        // printf("built:  \t'%s'\n", carbon_to_json(&sb, &doc));
//        // printf("altered:\t'%s'\n", carbon_to_json(&sb, &rev_doc));
//        ASSERT_TRUE(strcmp(carbon_to_json(&sb, &rev_doc), "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [[null, true, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, true, [32, 33, 34, 35], [], [null, true, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35]]], [null, true, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35], [], [null, true, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35]]]]}") == 0);
//        rec_drop(&doc);
//        rec_drop(&rev_doc);
//
//        create_nested_doc(&doc);
//        status = update_one_set_true("0.8", &rev_doc, &doc); // replaces column ([32, 33, 34, 35]) with true
//        ASSERT_TRUE(status);
//        // printf("built:  \t'%s'\n", carbon_to_json(&sb, &doc));
//        // printf("altered:\t'%s'\n", carbon_to_json(&sb, &rev_doc));
//        ASSERT_TRUE(strcmp(carbon_to_json(&sb, &rev_doc), "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [[null, true, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, true, [], [null, true, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35]]], [null, true, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35], [], [null, true, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35]]]]}") == 0);
//        rec_drop(&doc);
//        rec_drop(&rev_doc);

//        create_nested_doc(&doc);
//        // ??????
//        status = update_one_set_true("0.8.0", &rev_doc, &doc); // replaces element in column with null value (special case) --> [NULL, 33, 34, 35]
//        ASSERT_TRUE(status);
//        printf("built:  \t'%s'\n", carbon_to_json(&sb, &doc));
//        printf("altered:\t'%s'\n", carbon_to_json(&sb, &rev_doc));
//        ASSERT_TRUE(strcmp(carbon_to_json(&sb, &rev_doc), "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [[null, true, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [true, 33, 34, 35], [], [null, true, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35]]], [null, true, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35], [], [null, true, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35]]]]}") == 0);
//        rec_drop(&doc);
//        rec_drop(&rev_doc);
//
//        create_nested_doc(&doc);
//        status = update_one_set_true("0.9", &rev_doc, &doc); // replaces empty array with true
//        ASSERT_TRUE(status);
//        printf("built:  \t'%s'\n", carbon_to_json(&sb, &doc));
//        printf("altered:\t'%s'\n", carbon_to_json(&sb, &rev_doc));
//        ASSERT_TRUE(strcmp(carbon_to_json(&sb, &rev_doc), "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [[null, true, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35], true, [null, true, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35]]], [null, true, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35], [], [null, true, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35]]]]}") == 0);
//        rec_drop(&doc);
//        rec_drop(&rev_doc);
//
//        create_nested_doc(&doc);
//        status = update_one_set_true("0.10", &rev_doc, &doc); // replaces complex array with true
//        ASSERT_TRUE(status);
//        printf("built:  \t'%s'\n", carbon_to_json(&sb, &doc));
//        printf("altered:\t'%s'\n", carbon_to_json(&sb, &rev_doc));
//        ASSERT_TRUE(strcmp(carbon_to_json(&sb, &rev_doc), "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [[null, true, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35], [], true], [null, true, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35], [], [null, true, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35]]]]}") == 0);
//        rec_drop(&doc);
//        rec_drop(&rev_doc);
//
//        create_nested_doc(&doc);
//        status = update_one_set_true("0", &rev_doc, &doc); // replaces 1st outermost array with true
//        ASSERT_TRUE(status);
//        printf("built:  \t'%s'\n", carbon_to_json(&sb, &doc));
//        printf("altered:\t'%s'\n", carbon_to_json(&sb, &rev_doc));
//        ASSERT_TRUE(strcmp(carbon_to_json(&sb, &rev_doc), "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [true, [null, true, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35], [], [null, true, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35]]]]}") == 0);
//        rec_drop(&doc);
//        rec_drop(&rev_doc);
//
//        create_nested_doc(&doc);
//        status = update_one_set_true("1", &rev_doc, &doc); // replaces 2nd outermost array with true
//        ASSERT_TRUE(status);
//        printf("built:  \t'%s'\n", carbon_to_json(&sb, &doc));
//        printf("altered:\t'%s'\n", carbon_to_json(&sb, &rev_doc));
//        ASSERT_TRUE(strcmp(carbon_to_json(&sb, &rev_doc), "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [[null, true, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35], [], [null, true, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35]]], true]}") == 0);
//        rec_drop(&doc);
//        rec_drop(&rev_doc);

        /*
        create_nested_doc(&doc);
        status = update_one_set_null("0.5", &rev_doc, &doc); // replaces str_buf with null
        ASSERT_TRUE(status);
        printf("built:  \t'%s'\n", carbon_to_json(&sb, &doc));
        printf("altered:\t'%s'\n", carbon_to_json(&sb, &rev_doc));
        ASSERT_TRUE(strcmp(carbon_to_json(&sb, &rev_doc), "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [[null, true, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35], [], [null, true, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35]]], [null, true, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35], [], [null, true, false, 8, -16, \"Hello, World!\", { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }, { \"type\": \"own\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgT3duIEZvcm1hdAAA\" }, [32, 33, 34, 35]]]]}") == 0);
        rec_drop(&doc);
        rec_drop(&rev_doc);
        */


        // Overwrite constant in-pace w/ fixed-type
        // Overwrite constant in-pace w/ str_buf
        // Overwrite constant in-pace w/ binary
        // Overwrite constant in-pace w/ custom binary
        // Overwrite constant in-pace w/ empty array
        // Overwrite constant in-pace w/ non-empty array
        // Overwrite constant in-pace w/ empty column
        // Overwrite constant in-pace w/ non-empty column

        // Update fixed-type in-place
        // Overwrite fixed-type in-pace w/ constant
        // Overwrite fixed-type in-pace w/ fixed-type (w/ same width)
        // Overwrite fixed-type in-pace w/ fixed-type (w/ other width)
        // Overwrite fixed-type in-pace w/ str_buf
        // Overwrite fixed-type in-pace w/ binary
        // Overwrite fixed-type in-pace w/ custom binary
        // Overwrite fixed-type in-pace w/ empty array
        // Overwrite fixed-type in-pace w/ non-empty array
        // Overwrite fixed-type in-pace w/ empty column
        // Overwrite fixed-type in-pace w/ non-empty column

        // Update str_buf in-place
        // Overwrite str_buf in-pace w/ constant
        // Overwrite str_buf in-pace w/ fixed-type
        // Overwrite str_buf in-pace w/ str_buf
        // Overwrite str_buf in-pace w/ binary
        // Overwrite str_buf in-pace w/ custom binary
        // Overwrite str_buf in-pace w/ empty array
        // Overwrite str_buf in-pace w/ non-empty array
        // Overwrite str_buf in-pace w/ empty column
        // Overwrite str_buf in-pace w/ non-empty column

        // Update binary in-place
        // Overwrite binary in-pace w/ constant
        // Overwrite binary in-pace w/ fixed-type
        // Overwrite binary in-pace w/ str_buf
        // Overwrite binary in-pace w/ binary
        // Overwrite binary in-pace w/ custom binary
        // Overwrite binary in-pace w/ empty array
        // Overwrite binary in-pace w/ non-empty array
        // Overwrite binary in-pace w/ empty column
        // Overwrite binary in-pace w/ non-empty column

        // Update custom binary in-place
        // Overwrite custom binary in-pace w/ constant
        // Overwrite custom binary in-pace w/ fixed-type
        // Overwrite custom binary in-pace w/ str_buf
        // Overwrite custom binary in-pace w/ binary
        // Overwrite custom binary in-pace w/ custom binary
        // Overwrite custom binary in-pace w/ empty array
        // Overwrite custom binary in-pace w/ non-empty array
        // Overwrite custom binary in-pace w/ empty column
        // Overwrite custom binary in-pace w/ non-empty column

        // Update empty-array binary in-place
        // Overwrite empty-array in-pace w/ constant
        // Overwrite empty-array in-pace w/ fixed-type
        // Overwrite empty-array in-pace w/ str_buf
        // Overwrite empty-array in-pace w/ binary
        // Overwrite empty-array in-pace w/ custom binary
        // Overwrite empty-array in-pace w/ non-empty array
        // Overwrite empty-array in-pace w/ empty column
        // Overwrite empty-array in-pace w/ non-empty column

        // Update non-empty array binary in-place
        // Overwrite non-empty array in-pace w/ constant
        // Overwrite non-empty array in-pace w/ fixed-type
        // Overwrite non-empty array in-pace w/ str_buf
        // Overwrite non-empty array in-pace w/ binary
        // Overwrite non-empty array in-pace w/ custom binary
        // Overwrite non-empty array in-pace w/ empty array
        // Overwrite non-empty array in-pace w/ non-empty array
        // Overwrite non-empty array in-pace w/ empty column
        // Overwrite non-empty array in-pace w/ non-empty column

        // Overwrite empty column in-pace w/ constant
        // Overwrite empty column in-pace w/ fixed-type
        // Overwrite empty column in-pace w/ str_buf
        // Overwrite empty column in-pace w/ binary
        // Overwrite empty column in-pace w/ custom binary
        // Overwrite empty column in-pace w/ empty array
        // Overwrite empty column in-pace w/ non-empty array
        // Overwrite empty column in-pace w/ non-empty column

        // Update non-empty column in-place
        // Overwrite non-empty column in-pace w/ constant
        // Overwrite non-empty column in-pace w/ fixed-type
        // Overwrite non-empty column in-pace w/ str_buf
        // Overwrite non-empty column in-pace w/ binary
        // Overwrite non-empty column in-pace w/ custom binary
        // Overwrite non-empty column in-pace w/ empty array
        // Overwrite non-empty column in-pace w/ non-empty array
        // Overwrite non-empty column in-pace w/ empty column
        // Overwrite non-empty column in-pace w/ non-empty column

        // Update column entry in-place
        // Overwrite column entry in-pace w/ constant (matching type)
        // Overwrite column entry in-pace w/ constant (not matching type)
        // Overwrite column entry in-pace w/ fixed-type (matching type)
        // Overwrite column entry in-pace w/ fixed-type (not matching type)

        // Overwrite entire document content in-pace w/ constant
        // Overwrite entire document content in-pace w/ fixed-type
        // Overwrite entire document content in-pace w/ str_buf
        // Overwrite entire document content in-pace w/ binary
        // Overwrite entire document content in-pace w/ custom binary
        // Overwrite entire document content in-pace w/ empty array
        // Overwrite entire document content in-pace w/ non-empty array
        // Overwrite entire document content in-pace w/ empty column
        // Overwrite entire document content in-pace w/ non-empty column


        str_buf_drop(&sb);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}