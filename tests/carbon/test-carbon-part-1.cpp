#include <gtest/gtest.h>
#include <printf.h>

#include <karbonit/karbonit.h>


TEST(CarbonTest, CreateCarbon) {
        rec doc;
        unique_id_t oid;
        u64 rev;
        str_buf buffer;
        bool status;

        str_buf_create(&buffer);

        rec_create_empty(&doc, LIST_UNSORTED_MULTISET, KEY_AUTOKEY);

        //rec_hexdump_print(stderr, &doc);

        status = rec_key_unsigned_value(&oid, &doc);
        EXPECT_TRUE(status);
        EXPECT_NE(oid, 0U);

        status = rec_commit_hash(&rev, &doc);
        EXPECT_TRUE(status);
        EXPECT_NE(rev, 0U);

        rec_to_json(&buffer, &doc);
        // printf("%s\n", string_buffer_cstr(&buffer));
        str_buf_drop(&buffer);

        rec_drop(&doc);
}

TEST(CarbonTest, CreateCarbonRevisionNumberingNoKey) {
        rec_new context;
        rec doc, rev_doc;
        rev revise;
        u64 commit_new, commit_mod;

        rec_create_begin(&context, &doc, KEY_NOKEY, OPTIMIZE);
        rec_create_end(&context);

        revise_begin(&revise, &rev_doc, &doc);
        revise_end(&revise);

        /* Commit hash value for records with 'nokey' option are always set to 0 (see specs) */
        rec_commit_hash(&commit_new, &doc);
        rec_commit_hash(&commit_mod, &rev_doc);
        ASSERT_EQ(commit_new, 0U);
        ASSERT_EQ(commit_mod, 0U);

        rec_drop(&doc);
        rec_drop(&rev_doc);
}

TEST(CarbonTest, CreateCarbonRevisionNumberingWithKey) {
        rec_new context;
        rec doc, rev_doc;
        rev revise;
        u64 commit_new, commit_mod, commit_mod_cmpr;

        rec_create_begin(&context, &doc, KEY_AUTOKEY, OPTIMIZE);
        rec_create_end(&context);

        revise_begin(&revise, &rev_doc, &doc);
        revise_end(&revise);

        /* Commit hash value for records with no 'nokey' option shall be a (almost global) random number, and
         * a 64bit bernstein hash value of the original document after any modification (see specs) */
        rec_commit_hash(&commit_new, &doc);
        rec_commit_hash(&commit_mod, &rev_doc);
        ASSERT_NE(commit_new, 0U);
        ASSERT_NE(commit_new, commit_mod);

        u64 raw_data_len = 0;
        const void *raw_data = rec_raw_data(&raw_data_len, &doc);
        commit_compute(&commit_mod_cmpr, raw_data, raw_data_len);

        ASSERT_EQ(commit_mod, commit_mod_cmpr);

        rec_drop(&doc);
        rec_drop(&rev_doc);
}

TEST(CarbonTest, CreateCarbonRevisionNumbering) {
        rec doc, rev_doc;
        u64 hash;
        str_buf buffer;
        bool status;

        str_buf_create(&buffer);

        rec_create_empty(&doc, LIST_UNSORTED_MULTISET, KEY_NOKEY);

        status = rec_commit_hash(&hash, &doc);
        EXPECT_TRUE(status);
        EXPECT_EQ(hash, 0U);

        rev revise;
        revise_begin(&revise, &rev_doc, &doc);
        revise_end(&revise);

        status = rec_commit_hash(&hash, &doc);
        EXPECT_TRUE(status);
        EXPECT_EQ(hash, 0U);

        status = rec_commit_hash(&hash, &rev_doc);
        EXPECT_TRUE(status);
        EXPECT_EQ(hash, 0U);

        rec_to_json(&buffer, &doc);
        // printf("%s\n", string_buffer_cstr(&buffer));
        str_buf_drop(&buffer);

        rec_drop(&doc);
        rec_drop(&rev_doc);
}

TEST(CarbonTest, CreateCarbonRevisionAbort) {
        rec doc, rev_doc;
        u64 hash;
        str_buf buffer;
        bool status;

        str_buf_create(&buffer);

        rec_create_empty(&doc, LIST_UNSORTED_MULTISET, KEY_NOKEY);

        status = rec_commit_hash(&hash, &doc);
        EXPECT_TRUE(status);
        EXPECT_EQ(hash, 0U);

        rev revise;
        revise_begin(&revise, &rev_doc, &doc);
        revise_abort(&revise);

        status = rec_commit_hash(&hash, &doc);
        EXPECT_TRUE(status);
        EXPECT_EQ(hash, 0U);

        rec_to_json(&buffer, &doc);
        // printf("%s\n", string_buffer_cstr(&buffer));
        str_buf_drop(&buffer);

        rec_drop(&doc);
}

TEST(CarbonTest, CreateCarbonRevisionAsyncReading) {
        rec doc, rev_doc;
        u64 hash;
        str_buf buffer;
        bool status;

        str_buf_create(&buffer);

        rec_create_empty(&doc, LIST_UNSORTED_MULTISET, KEY_NOKEY);

        status = rec_commit_hash(&hash, &doc);
        EXPECT_TRUE(status);
        EXPECT_EQ(hash, 0U);

        rev revise;
        revise_begin(&revise, &rev_doc, &doc);

        status = rec_commit_hash(&hash, &doc);
        EXPECT_TRUE(status);
        EXPECT_EQ(hash, 0U);

        revise_end(&revise);

        status = rec_commit_hash(&hash, &doc);
        EXPECT_TRUE(status);
        EXPECT_EQ(hash, 0U);

        rec_to_json(&buffer, &doc);
        // printf("%s\n", string_buffer_cstr(&buffer));
        str_buf_drop(&buffer);

        rec_drop(&doc);
        rec_drop(&rev_doc);
}

TEST(CarbonTest, ModifyCarbonObjectId) {
        rec doc, rev_doc;
        unique_id_t oid;
        unique_id_t new_oid;
        rev revise;
        u64 commit_hash_old, commit_hash_new;

        rec_create_empty(&doc, LIST_UNSORTED_MULTISET, KEY_AUTOKEY);

        rec_key_unsigned_value(&oid, &doc);
        EXPECT_NE(oid, 0U);

        rec_commit_hash(&commit_hash_old, &doc);

        revise_begin(&revise, &rev_doc, &doc);
        revise_key_generate(&new_oid, &revise);
        EXPECT_NE(oid, new_oid);
        revise_end(&revise);

        rec_commit_hash(&commit_hash_new, &rev_doc);
        EXPECT_NE(commit_hash_old, commit_hash_new);

        rec_key_unsigned_value(&oid, &rev_doc);
        EXPECT_EQ(oid, new_oid);

        // rec_print(stdout, &rev_doc);

        rec_drop(&doc);
        rec_drop(&rev_doc);
}

TEST(CarbonTest, CarbonArrayIteratorOpenAfterNew) {
        rec doc, rev_doc;
        rev revise;
        arr_it it;

        rec_create_empty(&doc, LIST_UNSORTED_MULTISET, KEY_AUTOKEY);

        revise_begin(&revise, &rev_doc, &doc);
        revise_key_generate(NULL, &revise);
        revise_iterator_open(&it, &revise);
        bool has_next = arr_it_next(&it);
        EXPECT_EQ(has_next, false);
        revise_end(&revise);

        // rec_print(stdout, &rev_doc);
        //rec_hexdump_print(stdout, &rev_doc);

        rec_drop(&doc);
        rec_drop(&rev_doc);
}

TEST(CarbonTest, CarbonArrayIteratorInsertNullAfterNew) {
        rec doc, rev_doc;
        rev revise;
        arr_it it;
        insert in;

        rec_create_empty(&doc, LIST_UNSORTED_MULTISET, KEY_AUTOKEY);

        revise_begin(&revise, &rev_doc, &doc);
        revise_iterator_open(&it, &revise);
        revise_key_generate(NULL, &revise);
        arr_it_insert_begin(&in, &it);
        insert_null(&in);
        revise_end(&revise);

        // rec_print(stdout, &rev_doc);
        //rec_hexdump_print(stdout, &rev_doc);

        rec_drop(&doc);
        rec_drop(&rev_doc);
}

TEST(CarbonTest, CarbonArrayIteratorInsertMultipleLiteralsAfterNewNoOverflow) {
        rec doc, rev_doc;
        rev revise;
        arr_it it;
        insert in;

        rec_create_empty(&doc, LIST_UNSORTED_MULTISET, KEY_NOKEY);

        revise_begin(&revise, &rev_doc, &doc);
        revise_iterator_open(&it, &revise);
        arr_it_insert_begin(&in, &it);
        for (i32 i = 0; i < 10; i++) {
                // fprintf(stdout, "before:\n");
                //rec_hexdump_print(stdout, &rev_doc);
                bool status;
                if (i % 3 == 0) {
                        status = insert_null(&in);
                } else if (i % 3 == 1) {
                        status = insert_true(&in);
                } else {
                        status = insert_false(&in);
                }
                ASSERT_TRUE(status);
                // fprintf(stdout, "after:\n");
                //rec_hexdump_print(stdout, &rev_doc);
                // fprintf(stdout, "\n\n");
        }
        revise_end(&revise);

        // rec_print(stdout, &rev_doc);
        //rec_hexdump_print(stdout, &rev_doc);

        rec_drop(&doc);
        rec_drop(&rev_doc);
}

TEST(CarbonTest, CarbonArrayIteratorOverwriteLiterals) {
        rec doc, rev_doc, rev_doc2;
        rev revise;
        arr_it it;
        insert in;

        rec_create_empty(&doc, LIST_UNSORTED_MULTISET, KEY_NOKEY);

        revise_begin(&revise, &rev_doc, &doc);
        revise_iterator_open(&it, &revise);
        arr_it_insert_begin(&in, &it);
        for (i32 i = 0; i < 3; i++) {
                if (i % 3 == 0) {
                        insert_null(&in);
                } else if (i % 3 == 1) {
                        insert_true(&in);
                } else {
                        insert_false(&in);
                }
        }
        revise_end(&revise);

        revise_begin(&revise, &rev_doc2, &rev_doc);
        revise_iterator_open(&it, &revise);
        arr_it_insert_begin(&in, &it);
        for (i32 i = 0; i < 2; i++) {
                insert_true(&in);
        }
        revise_end(&revise);

        // rec_print(stdout, &rev_doc2);

        rec_drop(&doc);
        rec_drop(&rev_doc);
        rec_drop(&rev_doc2);
}

TEST(CarbonTest, CarbonArrayIteratorOverwriteLiteralsWithDocOverflow) {
        rec doc, rev_doc, rev_doc2;
        rev revise;
        arr_it it;
        insert in;

        rec_create_empty_ex(&doc, LIST_UNSORTED_MULTISET, KEY_NOKEY, 20, 1);

        revise_begin(&revise, &rev_doc, &doc);
        revise_iterator_open(&it, &revise);
        arr_it_insert_begin(&in, &it);
        for (i32 i = 0; i < 22; i++) {
                if (i % 3 == 0) {
                        insert_null(&in);
                } else if (i % 3 == 1) {
                        insert_true(&in);
                } else {
                        insert_false(&in);
                }
                // fprintf(stdout, "after initial push:\n");
                // //rec_hexdump_print(stdout, &rev_doc);
        }
        revise_end(&revise);

        revise_begin(&revise, &rev_doc2, &rev_doc);
        revise_iterator_open(&it, &revise);
        arr_it_insert_begin(&in, &it);
        for (i32 i = 0; i < 2; i++) {
                // fprintf(stdout, "before:\n");
                //rec_hexdump_print(stdout, &rev_doc2);
                insert_true(&in);
                // fprintf(stdout, "after:\n");
                //rec_hexdump_print(stdout, &rev_doc2);
        }
        revise_end(&revise);
        // rec_print(stdout, &rev_doc2);
        rec_drop(&doc);
        rec_drop(&rev_doc);
        rec_drop(&rev_doc2);
}

TEST(CarbonTest, CarbonArrayIteratorUnsignedAndConstants) {
        rec doc, rev_doc;
        rev revise;
        arr_it it;
        insert in;

        rec_create_empty_ex(&doc, LIST_UNSORTED_MULTISET, KEY_NOKEY,20, 1);

        revise_begin(&revise, &rev_doc, &doc);
        revise_iterator_open(&it, &revise);
        arr_it_insert_begin(&in, &it);
        for (i32 i = 0; i < 500; i++) {
                if (i % 6 == 0) {
                        insert_null(&in);
                } else if (i % 6 == 1) {
                        insert_true(&in);
                } else if (i % 6 == 2) {
                        insert_false(&in);
                } else if (i % 6 == 3) {
                        u64 rand_value = random();
                        insert_unsigned(&in, rand_value);
                } else if (i % 6 == 4) {
                        i64 rand_value = random();
                        insert_signed(&in, rand_value);
                } else {
                        float rand_value = (float)rand()/(float)(RAND_MAX/INT32_MAX);
                        insert_float(&in, rand_value);
                }
                //fprintf(stdout, "after initial push:\n");
                ////rec_hexdump_print(stdout, &rev_doc);
        }
        revise_end(&revise);

        // rec_print(stdout, &rev_doc);

        rec_drop(&doc);
        rec_drop(&rev_doc);
}

TEST(CarbonTest, CarbonArrayIteratorStrings) {
        rec doc, rev_doc;
        rev revise;
        arr_it it;
        insert in;

        rec_create_empty_ex(&doc, LIST_UNSORTED_MULTISET, KEY_NOKEY,20, 1);

        revise_begin(&revise, &rev_doc, &doc);
        revise_iterator_open(&it, &revise);
        arr_it_insert_begin(&in, &it);
        for (i32 i = 0; i < 10; i++) {
                u64 strlen = rand() % (100 + 1 - 4) + 4;
                char buffer[strlen];
                for (u64 j = 0; j < strlen; j++) {
                        buffer[j] = 65 + (rand() % 25);
                }
                buffer[0] = '!';
                buffer[strlen - 2] = '!';
                buffer[strlen - 1] = '\0';
                insert_string(&in, buffer);
                //fprintf(stdout, "after initial push:\n");
                ////rec_hexdump_print(stdout, &rev_doc);
        }
        revise_end(&revise);

        // rec_print(stdout, &rev_doc);

        rec_drop(&doc);
        rec_drop(&rev_doc);
}

TEST(CarbonTest, CarbonInsertMimeTypedBlob) {
        rec doc, rev_doc;
        rev revise;
        arr_it it;
        insert in;

        rec_create_empty_ex(&doc, LIST_UNSORTED_MULTISET, KEY_NOKEY,20, 1);

        revise_begin(&revise, &rev_doc, &doc);
        revise_iterator_open(&it, &revise);
        arr_it_insert_begin(&in, &it);
        const char *data = "{ \"Message\":\"Hello World\" }";
        bool status = insert_binary(&in, data, strlen(data), "json", NULL);
        ASSERT_TRUE(status);

        revise_end(&revise);

        // rec_print(stdout, &rev_doc);

        rec_drop(&doc);
        rec_drop(&rev_doc);
}

TEST(CarbonTest, CarbonInsertCustomTypedBlob) {
        rec doc, rev_doc;
        rev revise;
        arr_it it;
        insert in;

        rec_create_empty_ex(&doc, LIST_UNSORTED_MULTISET, KEY_NOKEY,20, 1);

        revise_begin(&revise, &rev_doc, &doc);
        revise_iterator_open(&it, &revise);
        arr_it_insert_begin(&in, &it);
        const char *data = "{ \"Message\":\"Hello World\" }";
        bool status = insert_binary(&in, data, strlen(data), NULL, "my data");
        ASSERT_TRUE(status);
        ////rec_hexdump_print(stdout, &rev_doc);

        revise_end(&revise);

        // rec_print(stdout, &rev_doc);

        rec_drop(&doc);
        rec_drop(&rev_doc);
}

TEST(CarbonTest, CarbonInsertTwoMimeTypedBlob) {
        rec doc, rev_doc;
        rev revise;
        arr_it it;
        insert in;

        rec_create_empty_ex(&doc, LIST_UNSORTED_MULTISET, KEY_NOKEY,20, 1);

        revise_begin(&revise, &rev_doc, &doc);
        revise_iterator_open(&it, &revise);
        arr_it_insert_begin(&in, &it);
        const char *data1 = "{ \"Message\":\"Hello World\" }";
        const char *data2 = "{ \"Blog-Header\":\"My Fancy Blog\" }";
        bool status = insert_binary(&in, data1, strlen(data1), "json", NULL);
        ASSERT_TRUE(status);
        status = insert_binary(&in, data2, strlen(data2), "txt", NULL);
        ASSERT_TRUE(status);
        ////rec_hexdump_print(stdout, &rev_doc);

        revise_end(&revise);

        // rec_print(stdout, &rev_doc);

        rec_drop(&doc);
        rec_drop(&rev_doc);
}

TEST(CarbonTest, CarbonInsertMimeTypedBlobsWithOverflow) {
        rec doc, rev_doc;
        rev revise;
        arr_it it;
        insert in;

        rec_create_empty_ex(&doc, LIST_UNSORTED_MULTISET, KEY_NOKEY,20, 1);

        revise_begin(&revise, &rev_doc, &doc);
        revise_iterator_open(&it, &revise);
        arr_it_insert_begin(&in, &it);
        const char *data1 = "{ \"Message\":\"Hello World\" }";
        const char *data2 = "{ \"Blog-Header\":\"My Fancy Blog\" }";
        for (u32 i = 0; i < 100; i++) {
                bool status = insert_binary(&in, i % 2 == 0 ? data1 : data2,
                                            strlen(i % 2 == 0 ? data1 : data2), "json", NULL);
                ASSERT_TRUE(status);
        }
        //rec_hexdump_print(stdout, &rev_doc);

        revise_end(&revise);

        // rec_print(stdout, &rev_doc);

        rec_drop(&doc);
        rec_drop(&rev_doc);
}

TEST(CarbonTest, CarbonInsertMixedTypedBlobsWithOverflow) {
        rec doc, rev_doc;
        rev revise;
        arr_it it;
        insert in;

        rec_create_empty_ex(&doc, LIST_UNSORTED_MULTISET, KEY_NOKEY,20, 1);

        revise_begin(&revise, &rev_doc, &doc);
        revise_iterator_open(&it, &revise);
        arr_it_insert_begin(&in, &it);
        const char *data1 = "{ \"Message\":\"Hello World\" }";
        const char *data2 = "{ \"Blog-Header\":\"My Fancy Blog\" }";
        for (u32 i = 0; i < 100; i++) {
                bool status = insert_binary(&in, i % 2 == 0 ? data1 : data2,
                                            strlen(i % 2 == 0 ? data1 : data2), i % 3 == 0 ? "json" : NULL, i % 5 == 0 ? "user/app" : NULL);
                ASSERT_TRUE(status);
        }
        ////rec_hexdump_print(stdout, &rev_doc);
        revise_end(&revise);

        //rec_print(stdout, &rev_doc);

        rec_drop(&doc);
        rec_drop(&rev_doc);
}

TEST(CarbonTest, CarbonInsertArrayWithNoOverflow) {
        rec doc, rev_doc;
        rev revise;
        arr_it it;
        insert in;
        arr_state array_state;

        rec_create_empty_ex(&doc, LIST_UNSORTED_MULTISET, KEY_NOKEY,20, 1);

        revise_begin(&revise, &rev_doc, &doc);
        revise_iterator_open(&it, &revise);
        arr_it_insert_begin(&in, &it);
        //rec_hexdump_print(stdout, &rev_doc);

        insert *nested = insert_array_begin(&array_state, &in, 10);
        ASSERT_TRUE(nested != NULL);
        insert_array_end(&array_state);

        //rec_hexdump_print(stdout, &rev_doc);
        revise_end(&revise);

        //rec_hexdump_print(stdout, &rev_doc);
        // rec_print(stdout, &rev_doc);

        rec_drop(&doc);
        rec_drop(&rev_doc);
}

TEST(CarbonTest, CarbonInsertValuesIntoNestedArrayWithNoOverflow) {
        rec doc, rev_doc;
        rev revise;
        arr_it it;
        insert in;
        arr_state array_state;

        rec_create_empty_ex(&doc, LIST_UNSORTED_MULTISET, KEY_NOKEY,20, 1);

        revise_begin(&revise, &rev_doc, &doc);
        revise_iterator_open(&it, &revise);
        arr_it_insert_begin(&in, &it);

        insert_null(&in);
        insert_null(&in);
        insert_null(&in);

        insert *nested = insert_array_begin(&array_state, &in, 10);
        ASSERT_TRUE(nested != NULL);
        insert_true(nested);
        insert_true(nested);
        insert_true(nested);
        insert_array_end(&array_state);

        insert_false(&in);
        insert_false(&in);
        insert_false(&in);


        //rec_hexdump_print(stdout, &rev_doc);
        revise_end(&revise);

        // rec_print(stdout, &rev_doc);

        rec_drop(&doc);
        rec_drop(&rev_doc);
}

TEST(CarbonTest, CarbonInsert2xNestedArrayWithNoOverflow) {
        rec doc, rev_doc;
        rev revise;
        arr_it it;
        insert in;
        arr_state array_state_l1, array_state_l2;

        rec_create_empty_ex(&doc, LIST_UNSORTED_MULTISET, KEY_NOKEY,20, 1);

        revise_begin(&revise, &rev_doc, &doc);
        revise_iterator_open(&it, &revise);
        arr_it_insert_begin(&in, &it);

        insert_null(&in);
        insert_null(&in);
        insert_null(&in);

        insert *nested_inserter_l1 = insert_array_begin(&array_state_l1, &in, 10);
        ASSERT_TRUE(nested_inserter_l1 != NULL);
        insert_true(nested_inserter_l1);
        insert_true(nested_inserter_l1);
        insert_true(nested_inserter_l1);

        insert *nested_inserter_l2 = insert_array_begin(&array_state_l2, nested_inserter_l1, 10);
        ASSERT_TRUE(nested_inserter_l2 != NULL);
        insert_true(nested_inserter_l2);
        insert_false(nested_inserter_l2);
        insert_null(nested_inserter_l2);
        insert_array_end(&array_state_l2);

        insert_array_end(&array_state_l1);

        insert_false(&in);
        insert_false(&in);
        insert_false(&in);


        //rec_hexdump_print(stdout, &rev_doc);
        revise_end(&revise);

        // rec_print(stdout, &rev_doc);

        rec_drop(&doc);
        rec_drop(&rev_doc);
}

TEST(CarbonTest, CarbonInsertXxNestedArrayWithoutOverflow) {
        rec doc, rev_doc;
        rev revise;
        arr_it it;
        insert in;
        arr_state array_state_l1;

        rec_create_empty_ex(&doc, LIST_UNSORTED_MULTISET, KEY_NOKEY,20, 1);

        revise_begin(&revise, &rev_doc, &doc);
        revise_iterator_open(&it, &revise);
        arr_it_insert_begin(&in, &it);

        insert_null(&in);
        insert_null(&in);
        insert_null(&in);

        for (int i = 0; i < 10; i++) {
                insert *nested_inserter_l1 = insert_array_begin(&array_state_l1, &in, 10);
                ASSERT_TRUE(nested_inserter_l1 != NULL);
                insert_true(nested_inserter_l1);
                insert_true(nested_inserter_l1);
                insert_true(nested_inserter_l1);
                insert_array_end(&array_state_l1);
        }

        insert_false(&in);
        insert_false(&in);
        insert_false(&in);

        //rec_hexdump_print(stdout, &rev_doc);
        revise_end(&revise);

        //rec_print(stdout, &rev_doc);
        str_buf sb;
        str_buf_create(&sb);
        rec_to_json(&sb, &rev_doc);
        ASSERT_TRUE(0 == strcmp(str_buf_cstr(&sb), "[null, null, null, [true, true, true], [true, true, true], [true, true, true], [true, true, true], [true, true, true], [true, true, true], [true, true, true], [true, true, true], [true, true, true], [true, true, true], false, false, false]"));
        str_buf_drop(&sb);

        rec_drop(&doc);
        rec_drop(&rev_doc);
}

TEST(CarbonTest, CarbonInsertXxNestedArrayWithOverflow) {
        rec doc, rev_doc;
        rev revise;
        arr_it it;
        insert in;
        arr_state array_state_l1;

        rec_create_empty_ex(&doc, LIST_UNSORTED_MULTISET, KEY_NOKEY,20, 1);

        revise_begin(&revise, &rev_doc, &doc);
        revise_iterator_open(&it, &revise);
        arr_it_insert_begin(&in, &it);

        //rec_hexdump_print(stdout, &rev_doc);
        // printf("\n");

        insert_null(&in);

        //rec_hexdump_print(stdout, &rev_doc);
        // printf("\n");

        insert_null(&in);

        //rec_hexdump_print(stdout, &rev_doc);
        // printf("\n");

        insert_null(&in);

        for (int i = 0; i < 10; i++) {
                insert *nested_inserter_l1 = insert_array_begin(&array_state_l1, &in, 1);
                ASSERT_TRUE(nested_inserter_l1 != NULL);
                insert_true(nested_inserter_l1);
                insert_true(nested_inserter_l1);
                insert_true(nested_inserter_l1);
                insert_array_end(&array_state_l1);
        }

        insert_false(&in);
        insert_false(&in);
        insert_false(&in);

        revise_end(&revise);

        // rec_print(stdout, &rev_doc);
        str_buf sb;
        str_buf_create(&sb);
        rec_to_json(&sb, &rev_doc);
        ASSERT_TRUE(0 == strcmp(str_buf_cstr(&sb), "[null, null, null, [true, true, true], [true, true, true], [true, true, true], [true, true, true], [true, true, true], [true, true, true], [true, true, true], [true, true, true], [true, true, true], [true, true, true], false, false, false]"));
        str_buf_drop(&sb);

        rec_drop(&doc);
        rec_drop(&rev_doc);
}

TEST(CarbonTest, CarbonInsertInsertColumnWithoutOverflow) {
        rec doc, rev_doc;
        rev revise;
        arr_it it;
        insert in;
        col_state column_state;

        rec_create_empty_ex(&doc, LIST_UNSORTED_MULTISET, KEY_NOKEY,20, 1);

        revise_begin(&revise, &rev_doc, &doc);
        revise_iterator_open(&it, &revise);
        arr_it_insert_begin(&in, &it);

        insert *nested_inserter_l1 = insert_column_begin(&column_state, &in, COLUMN_U8, 10);

        //rec_hexdump_print(stdout, &rev_doc);

        ASSERT_TRUE(nested_inserter_l1 != NULL);
        insert_u8(nested_inserter_l1, 1);
        insert_u8(nested_inserter_l1, 2);
        insert_u8(nested_inserter_l1, 3);
        insert_column_end(&column_state);

        revise_end(&revise);

        //rec_hexdump_print(stdout, &rev_doc);

        // rec_print(stdout, &rev_doc);
        str_buf sb;
        str_buf_create(&sb);
        rec_to_json(&sb, &rev_doc);
        ASSERT_TRUE(0 == strcmp(str_buf_cstr(&sb), "[1, 2, 3]"));
        str_buf_drop(&sb);

        rec_drop(&doc);
        rec_drop(&rev_doc);
}

TEST(CarbonTest, CarbonInsertInsertColumnNumbersWithoutOverflow) {
        rec doc, rev_doc;
        rev revise;
        arr_it it;
        insert in;
        col_state column_state;

        rec_create_empty_ex(&doc, LIST_UNSORTED_MULTISET, KEY_NOKEY,20, 1);

        revise_begin(&revise, &rev_doc, &doc);
        revise_iterator_open(&it, &revise);
        arr_it_insert_begin(&in, &it);

        insert *nested_inserter_l1 = insert_column_begin(&column_state, &in, COLUMN_U8, 10);

        ASSERT_TRUE(nested_inserter_l1 != NULL);
        insert_u8(nested_inserter_l1, 42);
        insert_u8(nested_inserter_l1, 43);
        insert_u8(nested_inserter_l1, 44);
        insert_column_end(&column_state);

        revise_end(&revise);

        //rec_hexdump_print(stdout, &rev_doc);

        // rec_print(stdout, &rev_doc);
        str_buf sb;
        str_buf_create(&sb);
        rec_to_json(&sb, &rev_doc);
        ASSERT_TRUE(0 == strcmp(str_buf_cstr(&sb), "[42, 43, 44]"));
        str_buf_drop(&sb);

        rec_drop(&doc);
        rec_drop(&rev_doc);
}

TEST(CarbonTest, CarbonInsertInsertColumnNumbersZeroWithoutOverflow) {
        rec doc, rev_doc;
        rev revise;
        arr_it it;
        insert in;
        col_state column_state;

        rec_create_empty_ex(&doc, LIST_UNSORTED_MULTISET, KEY_NOKEY,20, 1);

        revise_begin(&revise, &rev_doc, &doc);
        revise_iterator_open(&it, &revise);
        arr_it_insert_begin(&in, &it);

        insert *nested_inserter_l1 = insert_column_begin(&column_state, &in, COLUMN_U8, 10);

        ASSERT_TRUE(nested_inserter_l1 != NULL);
        insert_u8(nested_inserter_l1, 0);
        insert_u8(nested_inserter_l1, 0);
        insert_u8(nested_inserter_l1, 0);
        insert_column_end(&column_state);

        revise_end(&revise);

        //rec_hexdump_print(stdout, &rev_doc);

        // rec_print(stdout, &rev_doc);
        str_buf sb;
        str_buf_create(&sb);
        rec_to_json(&sb, &rev_doc);
        ASSERT_TRUE(0 == strcmp(str_buf_cstr(&sb), "[0, 0, 0]"));
        str_buf_drop(&sb);

        rec_drop(&doc);
        rec_drop(&rev_doc);
}

TEST(CarbonTest, CarbonInsertInsertMultileTypedColumnsWithoutOverflow) {
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
        insert_u8(ins, 2);
        insert_u8(ins, 3);
        insert_column_end(&column_state);

        ins = insert_column_begin(&column_state, &in, COLUMN_U16, 10);
        insert_u16(ins, 4);
        insert_u16(ins, 5);
        insert_u16(ins, 6);
        insert_column_end(&column_state);

        ins = insert_column_begin(&column_state, &in, COLUMN_U32, 10);
        insert_u32(ins, 7);
        insert_u32(ins, 8);
        insert_u32(ins, 9);
        insert_column_end(&column_state);

        ins = insert_column_begin(&column_state, &in, COLUMN_U64, 10);
        insert_u64(ins, 10);
        insert_u64(ins, 11);
        insert_u64(ins, 12);
        insert_column_end(&column_state);

        ins = insert_column_begin(&column_state, &in, COLUMN_I8, 10);
        insert_i8(ins, -1);
        insert_i8(ins, -2);
        insert_i8(ins, -3);
        insert_column_end(&column_state);

        ins = insert_column_begin(&column_state, &in, COLUMN_I16, 10);
        insert_i16(ins, -4);
        insert_i16(ins, -5);
        insert_i16(ins, -6);
        insert_column_end(&column_state);

        ins = insert_column_begin(&column_state, &in, COLUMN_I32, 10);
        insert_i32(ins, -7);
        insert_i32(ins, -8);
        insert_i32(ins, -9);
        insert_column_end(&column_state);

        ins = insert_column_begin(&column_state, &in, COLUMN_I64, 10);
        insert_i64(ins, -10);
        insert_i64(ins, -11);
        insert_i64(ins, -12);
        insert_column_end(&column_state);

        ins = insert_column_begin(&column_state, &in, COLUMN_FLOAT, 10);
        insert_float(ins, 42.0f);
        insert_float(ins, 21.0f);
        insert_float(ins, 23.4221f);
        insert_column_end(&column_state);

        revise_end(&revise);

        //rec_hexdump_print(stdout, &rev_doc);

        //rec_print(stdout, &rev_doc);
        str_buf sb;
        str_buf_create(&sb);
        rec_to_json(&sb, &rev_doc);
        //string_buffer_print(&sb);
        ASSERT_TRUE(0 == strcmp(str_buf_cstr(&sb), "[[1, 2, 3], [true, true, true], [false, false, false], [1, 2, 3], [4, 5, 6], [7, 8, 9], [10, 11, 12], [-1, -2, -3], [-4, -5, -6], [-7, -8, -9], [-10, -11, -12], [42.00, 21.00, 23.42]]"));
        str_buf_drop(&sb);

        rec_drop(&doc);
        rec_drop(&rev_doc);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}