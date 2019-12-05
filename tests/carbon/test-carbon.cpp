#include <gtest/gtest.h>

#include <fcntl.h>

#include <jakson/jakson.h>

TEST(CarbonTest, CreateCarbon) {
        rec doc;
        unique_id_t oid;
        u64 rev;
        str_buf buffer;
        bool status;

        str_buf_create(&buffer);

        rec_create_empty(&doc, LIST_UNSORTED_MULTISET, KEY_AUTOKEY);

        //rec_hexdump_print(stderr, &doc);

        status = key_unsigned_value(&oid, &doc);
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

        key_unsigned_value(&oid, &doc);
        EXPECT_NE(oid, 0U);

        rec_commit_hash(&commit_hash_old, &doc);

        revise_begin(&revise, &rev_doc, &doc);
        revise_key_generate(&new_oid, &revise);
        EXPECT_NE(oid, new_oid);
        revise_end(&revise);

        rec_commit_hash(&commit_hash_new, &rev_doc);
        EXPECT_NE(commit_hash_old, commit_hash_new);

        key_unsigned_value(&oid, &rev_doc);
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
        arr_it_drop(&it);

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
        arr_it_drop(&it);

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
        arr_it_drop(&it);
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
        arr_it_drop(&it);
        revise_end(&revise);

        revise_begin(&revise, &rev_doc2, &rev_doc);
        revise_iterator_open(&it, &revise);
        arr_it_insert_begin(&in, &it);
        for (i32 i = 0; i < 2; i++) {
                insert_true(&in);
        }
        arr_it_drop(&it);
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
        arr_it_drop(&it);
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
        arr_it_drop(&it);
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
        arr_it_drop(&it);
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
        arr_it_drop(&it);
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
        const char *data = "{ \"Message\": \"Hello World\" }";
        bool status = insert_binary(&in, data, strlen(data), "json", NULL);
        ASSERT_TRUE(status);

        arr_it_drop(&it);
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
        const char *data = "{ \"Message\": \"Hello World\" }";
        bool status = insert_binary(&in, data, strlen(data), NULL, "my data");
        ASSERT_TRUE(status);
        ////rec_hexdump_print(stdout, &rev_doc);

        arr_it_drop(&it);
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
        const char *data1 = "{ \"Message\": \"Hello World\" }";
        const char *data2 = "{ \"Blog-Header\": \"My Fancy Blog\" }";
        bool status = insert_binary(&in, data1, strlen(data1), "json", NULL);
        ASSERT_TRUE(status);
        status = insert_binary(&in, data2, strlen(data2), "txt", NULL);
        ASSERT_TRUE(status);
        ////rec_hexdump_print(stdout, &rev_doc);

        arr_it_drop(&it);
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
        const char *data1 = "{ \"Message\": \"Hello World\" }";
        const char *data2 = "{ \"Blog-Header\": \"My Fancy Blog\" }";
        for (u32 i = 0; i < 100; i++) {
                bool status = insert_binary(&in, i % 2 == 0 ? data1 : data2,
                        strlen(i % 2 == 0 ? data1 : data2), "json", NULL);
                ASSERT_TRUE(status);
        }
        //rec_hexdump_print(stdout, &rev_doc);

        arr_it_drop(&it);
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
        const char *data1 = "{ \"Message\": \"Hello World\" }";
        const char *data2 = "{ \"Blog-Header\": \"My Fancy Blog\" }";
        for (u32 i = 0; i < 100; i++) {
                bool status = insert_binary(&in, i % 2 == 0 ? data1 : data2,
                        strlen(i % 2 == 0 ? data1 : data2), i % 3 == 0 ? "json" : NULL, i % 5 == 0 ? "user/app" : NULL);
                ASSERT_TRUE(status);
        }
        ////rec_hexdump_print(stdout, &rev_doc);
        arr_it_drop(&it);
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
        arr_it_drop(&it);
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
        arr_it_drop(&it);
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
        arr_it_drop(&it);
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
        arr_it_drop(&it);
        revise_end(&revise);

        //rec_print(stdout, &rev_doc);
        str_buf sb;
        str_buf_create(&sb);
        rec_to_json(&sb, &rev_doc);
        ASSERT_TRUE(0 == strcmp(str_buf_cstr(&sb), "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [null, null, null, [true, true, true], [true, true, true], [true, true, true], [true, true, true], [true, true, true], [true, true, true], [true, true, true], [true, true, true], [true, true, true], [true, true, true], false, false, false]}"));
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

        arr_it_drop(&it);
        revise_end(&revise);

        // rec_print(stdout, &rev_doc);
        str_buf sb;
        str_buf_create(&sb);
        rec_to_json(&sb, &rev_doc);
        ASSERT_TRUE(0 == strcmp(str_buf_cstr(&sb), "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [null, null, null, [true, true, true], [true, true, true], [true, true, true], [true, true, true], [true, true, true], [true, true, true], [true, true, true], [true, true, true], [true, true, true], [true, true, true], false, false, false]}"));
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

        arr_it_drop(&it);
        revise_end(&revise);

        //rec_hexdump_print(stdout, &rev_doc);

        // rec_print(stdout, &rev_doc);
        str_buf sb;
        str_buf_create(&sb);
        rec_to_json(&sb, &rev_doc);
        ASSERT_TRUE(0 == strcmp(str_buf_cstr(&sb), "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [[1, 2, 3]]}"));
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

        arr_it_drop(&it);
        revise_end(&revise);

        //rec_hexdump_print(stdout, &rev_doc);

        // rec_print(stdout, &rev_doc);
        str_buf sb;
        str_buf_create(&sb);
        rec_to_json(&sb, &rev_doc);
        ASSERT_TRUE(0 == strcmp(str_buf_cstr(&sb), "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [[42, 43, 44]]}"));
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

        arr_it_drop(&it);
        revise_end(&revise);

        //rec_hexdump_print(stdout, &rev_doc);

        // rec_print(stdout, &rev_doc);
        str_buf sb;
        str_buf_create(&sb);
        rec_to_json(&sb, &rev_doc);
        ASSERT_TRUE(0 == strcmp(str_buf_cstr(&sb), "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [[0, 0, 0]]}"));
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

        arr_it_drop(&it);
        revise_end(&revise);

        //rec_hexdump_print(stdout, &rev_doc);

        //rec_print(stdout, &rev_doc);
        str_buf sb;
        str_buf_create(&sb);
        rec_to_json(&sb, &rev_doc);
        //string_buffer_print(&sb);
        ASSERT_TRUE(0 == strcmp(str_buf_cstr(&sb), "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [[1, 2, 3], [true, true, true], [false, false, false], [1, 2, 3], [4, 5, 6], [7, 8, 9], [10, 11, 12], [-1, -2, -3], [-4, -5, -6], [-7, -8, -9], [-10, -11, -12], [42.00, 21.00, 23.42]]}"));
        str_buf_drop(&sb);

        rec_drop(&doc);
        rec_drop(&rev_doc);
}

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
        revise_iterator_close(&rev_it);
        revise_end(&revise);
        // -------------------------------------------------------------------------------------------------------------

        char *json_2 = strdup(rec_to_json(&sb, &rev_doc));

        // -------------------------------------------------------------------------------------------------------------


        // printf("BEFORE\t'%s'\nAFTER\t'%s'\n", json_1, json_2);

        ASSERT_TRUE(strcmp(json_1, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [{ \"type\": \"123\", \"encoding\": \"base64\", \"binary-str_buf\": \"A=JDAA\" }]}") == 0);
        ASSERT_TRUE(strcmp(json_2, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": []}") == 0);

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
        revise_iterator_close(&rev_it);
        revise_end(&revise);
        // -------------------------------------------------------------------------------------------------------------

        char *json_2 = strdup(rec_to_json(&sb, &rev_doc));

        // -------------------------------------------------------------------------------------------------------------


        // printf("BEFORE\t'%s'\nAFTER\t'%s'\n", json_1, json_2);

        ASSERT_TRUE(strcmp(json_1, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [{ \"type\": \"my-fancy-format\", \"encoding\": \"base64\", \"binary-str_buf\": \"VGhpcyByZXBvcnQsIGJ5IGl0cyB2ZXJ5IGxlbmd0aCwgZGVmZW5kcyBpdHNlbGYgYWdhaW5zdCB0aGUgcmlzayBvZiBiZWluZyByZWFkLgAA\" }, { \"type\": \"application/something-json-like\", \"encoding\": \"base64\", \"binary-str_buf\": \"eyJrZXkiOiAidmFsdWUifQAA\" }]}") == 0);
        ASSERT_TRUE(strcmp(json_2, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [{ \"type\": \"application/something-json-like\", \"encoding\": \"base64\", \"binary-str_buf\": \"eyJrZXkiOiAidmFsdWUifQAA\" }]}") == 0);

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
        revise_iterator_close(&rev_it);
        revise_end(&revise);
        // -------------------------------------------------------------------------------------------------------------

        char *json_2 = strdup(rec_to_json(&sb, &rev_doc));

        // -------------------------------------------------------------------------------------------------------------


        // printf("BEFORE\t'%s'\nAFTER\t'%s'\n", json_1, json_2);

        ASSERT_TRUE(strcmp(json_1, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [{ \"type\": \"my-fancy-format\", \"encoding\": \"base64\", \"binary-str_buf\": \"VGhpcyByZXBvcnQsIGJ5IGl0cyB2ZXJ5IGxlbmd0aCwgZGVmZW5kcyBpdHNlbGYgYWdhaW5zdCB0aGUgcmlzayBvZiBiZWluZyByZWFkLgAA\" }, { \"type\": \"application/something-json-like\", \"encoding\": \"base64\", \"binary-str_buf\": \"eyJrZXkiOiAidmFsdWUifQAA\" }]}") == 0);
        ASSERT_TRUE(strcmp(json_2, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [{ \"type\": \"my-fancy-format\", \"encoding\": \"base64\", \"binary-str_buf\": \"VGhpcyByZXBvcnQsIGJ5IGl0cyB2ZXJ5IGxlbmd0aCwgZGVmZW5kcyBpdHNlbGYgYWdhaW5zdCB0aGUgcmlzayBvZiBiZWluZyByZWFkLgAA\" }]}") == 0);

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
        revise_iterator_close(&rev_it);
        revise_end(&revise);
        // -------------------------------------------------------------------------------------------------------------

        char *json_2 = strdup(rec_to_json(&sb, &rev_doc));

        // -------------------------------------------------------------------------------------------------------------

        // printf("BEFORE\t'%s'\nAFTER\t'%s'\n", json_1, json_2);

        ASSERT_TRUE(strcmp(json_1, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [{ \"type\": \"my-fancy-format\", \"encoding\": \"base64\", \"binary-str_buf\": \"VGhpcyByZXBvcnQsIGJ5IGl0cyB2ZXJ5IGxlbmd0aCwgZGVmZW5kcyBpdHNlbGYgYWdhaW5zdCB0aGUgcmlzayBvZiBiZWluZyByZWFkLgAA\" }, { \"type\": \"application/something-json-like\", \"encoding\": \"base64\", \"binary-str_buf\": \"eyJrZXkiOiAidmFsdWUifQAA\" }, { \"type\": \"my-other-nonstandard-format\", \"encoding\": \"base64\", \"binary-str_buf\": \"PGh0bWw+PGJvZHk+PHA+VGhlIHF1aWNrIGJyb3duIGZveCBqdW1wcyBvdmVyIHRoZSBsYXp5IGRvZzwvcD48L2JvZHk+PC9odG1sPgAA\" }]}") == 0);
        ASSERT_TRUE(strcmp(json_2, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [{ \"type\": \"my-fancy-format\", \"encoding\": \"base64\", \"binary-str_buf\": \"VGhpcyByZXBvcnQsIGJ5IGl0cyB2ZXJ5IGxlbmd0aCwgZGVmZW5kcyBpdHNlbGYgYWdhaW5zdCB0aGUgcmlzayBvZiBiZWluZyByZWFkLgAA\" }, { \"type\": \"my-other-nonstandard-format\", \"encoding\": \"base64\", \"binary-str_buf\": \"PGh0bWw+PGJvZHk+PHA+VGhlIHF1aWNrIGJyb3duIGZveCBqdW1wcyBvdmVyIHRoZSBsYXp5IGRvZzwvcD48L2JvZHk+PC9odG1sPgAA\" }]}") == 0);

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
        revise_iterator_close(&rev_it);
        revise_end(&revise);
        // -------------------------------------------------------------------------------------------------------------

        char *json_2 = strdup(rec_to_json(&sb, &rev_doc));

        // -------------------------------------------------------------------------------------------------------------


        // printf("BEFORE\t'%s'\nAFTER\t'%s'\n", json_1, json_2);

        ASSERT_TRUE(strcmp(json_1, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [[1, 2, 3]]}") == 0);
        ASSERT_TRUE(strcmp(json_2, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": []}") == 0);

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
        revise_iterator_close(&rev_it);
        revise_end(&revise);
        // -------------------------------------------------------------------------------------------------------------

        char *json_2 = strdup(rec_to_json(&sb, &rev_doc));

        // -------------------------------------------------------------------------------------------------------------


        // printf("BEFORE\t'%s'\nAFTER\t'%s'\n", json_1, json_2);

        ASSERT_TRUE(strcmp(json_1, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [[1, 2, 3], [4, 5, 6]]}") == 0);
        ASSERT_TRUE(strcmp(json_2, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [[4, 5, 6]]}") == 0);

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
        revise_iterator_close(&rev_it);
        revise_end(&revise);
        // -------------------------------------------------------------------------------------------------------------

        char *json_2 = strdup(rec_to_json(&sb, &rev_doc));

        // -------------------------------------------------------------------------------------------------------------


        // printf("BEFORE\t'%s'\nAFTER\t'%s'\n", json_1, json_2);

        ASSERT_TRUE(strcmp(json_1, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [[1, 2, 3], [4, 5, 6]]}") == 0);
        ASSERT_TRUE(strcmp(json_2, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [[1, 2, 3]]}") == 0);

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
        revise_iterator_close(&rev_it);
        revise_end(&revise);
        // -------------------------------------------------------------------------------------------------------------

        char *json_2 = strdup(rec_to_json(&sb, &rev_doc));

        // -------------------------------------------------------------------------------------------------------------

        // printf("BEFORE\t'%s'\nAFTER\t'%s'\n", json_1, json_2);

        ASSERT_TRUE(strcmp(json_1, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [[1, 2, 3], [4, 5, 6], [7, 8, 9]]}") == 0);
        ASSERT_TRUE(strcmp(json_2, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [[1, 2, 3], [7, 8, 9]]}") == 0);

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
        col_it *cit = item_get_column(&(rev_it.item));
        field_e type;
        u32 num_elems;
        col_it_values_info(&type, &num_elems, cit);
        ASSERT_EQ(type, FIELD_COLUMN_U16_UNSORTED_MULTISET);
        ASSERT_EQ(num_elems, 3u);

        status = col_it_remove(cit, 1);
        ASSERT_TRUE(status);
        col_it_values_info(&type, &num_elems, cit);
        ASSERT_EQ(type, FIELD_COLUMN_U16_UNSORTED_MULTISET);
        ASSERT_EQ(num_elems, 2u);
        values = col_it_u16_values(&num_elems, cit);
        ASSERT_EQ(values[0], 1);
        ASSERT_EQ(values[1], 3);

        char *json_2 = strdup(rec_to_json(&sb, &rev_doc));

        status = col_it_remove(cit, 0);
        ASSERT_TRUE(status);
        col_it_values_info(&type, &num_elems, cit);
        ASSERT_EQ(type, FIELD_COLUMN_U16_UNSORTED_MULTISET);
        ASSERT_EQ(num_elems, 1u);
        values = col_it_u16_values(&num_elems, cit);
        ASSERT_EQ(values[0], 3);

        char *json_3 = strdup(rec_to_json(&sb, &rev_doc));

        status = col_it_remove(cit, 0);
        ASSERT_TRUE(status);
        col_it_values_info(&type, &num_elems, cit);
        ASSERT_EQ(type, FIELD_COLUMN_U16_UNSORTED_MULTISET);
        ASSERT_EQ(num_elems, 0u);

        char *json_4 = strdup(rec_to_json(&sb, &rev_doc));

        revise_iterator_close(&rev_it);
        revise_end(&revise);

        // -------------------------------------------------------------------------------------------------------------


        // printf(">> %s\n", json_1);
        // printf(">> %s\n", json_2);
        // printf(">> %s\n", json_3);
        // printf(">> %s\n", json_4);

        ASSERT_TRUE(strcmp(json_1, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [[1, 2, 3]]}") == 0);
        ASSERT_TRUE(strcmp(json_2, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [[1, 3]]}") == 0);
        ASSERT_TRUE(strcmp(json_3, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [[3]]}") == 0);
        ASSERT_TRUE(strcmp(json_4, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [[]]}") == 0);

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
        ASSERT_TRUE(strcmp(json_1, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [1, \"Hello\", 2, 3, 3, \"World\", [], [4, \"Fox!\", 6], [[], [4], null, [\"Dog!\", [], [[41, 42, 43]], []]]]}") == 0);

        // -------------------------------------------------------------------------------------------------------------

        revise_remove_one("3", &rev_doc, &doc);
        char *json_2 = strdup(rec_to_json(&sb, &rev_doc));
        ASSERT_TRUE(strcmp(json_2, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [1, \"Hello\", 2, 3, \"World\", [], [4, \"Fox!\", 6], [[], [4], null, [\"Dog!\", [], [[41, 42, 43]], []]]]}") == 0);

        // -------------------------------------------------------------------------------------------------------------

        revise_remove_one("5", &rev_doc2, &rev_doc);
        char *json_3 = strdup(rec_to_json(&sb, &rev_doc2));
        ASSERT_TRUE(strcmp(json_3, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [1, \"Hello\", 2, 3, \"World\", [4, \"Fox!\", 6], [[], [4], null, [\"Dog!\", [], [[41, 42, 43]], []]]]}") == 0);

        // -------------------------------------------------------------------------------------------------------------

        revise_remove_one("5.1", &rev_doc3, &rev_doc2);
        char *json_4 = strdup(rec_to_json(&sb, &rev_doc3));
        ASSERT_TRUE(strcmp(json_4, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [1, \"Hello\", 2, 3, \"World\", [4, 6], [[], [4], null, [\"Dog!\", [], [[41, 42, 43]], []]]]}") == 0);

        // -------------------------------------------------------------------------------------------------------------

        revise_remove_one("6.0", &rev_doc4, &rev_doc3);
        char *json_5 = strdup(rec_to_json(&sb, &rev_doc4));
        ASSERT_TRUE(strcmp(json_5, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [1, \"Hello\", 2, 3, \"World\", [4, 6], [[4], null, [\"Dog!\", [], [[41, 42, 43]], []]]]}") == 0);

        // -------------------------------------------------------------------------------------------------------------

        revise_remove_one("5", &rev_doc5, &rev_doc4);
        char *json_6 = strdup(rec_to_json(&sb, &rev_doc5));
        ASSERT_TRUE(strcmp(json_6, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [1, \"Hello\", 2, 3, \"World\", [[4], null, [\"Dog!\", [], [[41, 42, 43]], []]]]}") == 0);

        // -------------------------------------------------------------------------------------------------------------

        revise_remove_one("0", &rev_doc6, &rev_doc5);
        revise_remove_one("1", &rev_doc7, &rev_doc6);
        revise_remove_one("0", &rev_doc8, &rev_doc7);
        revise_remove_one("1", &rev_doc9, &rev_doc8);
        revise_remove_one("0", &rev_doc10, &rev_doc9);
        char *json_11 = strdup(rec_to_json(&sb, &rev_doc10));
        ASSERT_TRUE(strcmp(json_11, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [[[4], null, [\"Dog!\", [], [[41, 42, 43]], []]]]}") == 0);

        // -------------------------------------------------------------------------------------------------------------

        revise_remove_one("0.2.2.0", &rev_doc11, &rev_doc10);

        char *json_12 = strdup(rec_to_json(&sb, &rev_doc11));
        ASSERT_TRUE(strcmp(json_12, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [[[4], null, [\"Dog!\", [], [], []]]]}") == 0);

        // -------------------------------------------------------------------------------------------------------------

        revise_remove_one("0.2.2", &rev_doc12, &rev_doc11);

        char *json_13 = strdup(rec_to_json(&sb, &rev_doc12));
        ASSERT_TRUE(strcmp(json_13, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [[[4], null, [\"Dog!\", [], []]]]}") == 0);

        // -------------------------------------------------------------------------------------------------------------

        revise_remove_one("0.2", &rev_doc13, &rev_doc12);

        char *json_14 = strdup(rec_to_json(&sb, &rev_doc13));
        ASSERT_TRUE(strcmp(json_14, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [[[4], null]]}") == 0);

        // -------------------------------------------------------------------------------------------------------------

        revise_remove_one("0", &rev_doc14, &rev_doc13);

        char *json_15 = strdup(rec_to_json(&sb, &rev_doc14));
        ASSERT_TRUE(strcmp(json_15, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": []}") == 0);


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
        revise_iterator_close(&it);
        revise_end(&revise);


        json = rec_to_json(&sb, &rev_doc);
        ASSERT_TRUE(strcmp(json, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [1, -42, 23.00]}") == 0);

        // -------------------------------------------------------------------------------------------------------------

        revise_begin(&revise, &rev_doc2, &rev_doc);
        update_set_u32(&revise, "1", 1024);
        revise_end(&revise);


        json = rec_to_json(&sb, &rev_doc2);
        ASSERT_TRUE(strcmp(json, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [1, 1024, 23.00]}") == 0);

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

        ASSERT_TRUE(strcmp(rec_to_json(&sb, &doc), "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [\"Hello\", \"World\"]}") == 0);

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
        ASSERT_TRUE(strcmp(rec_to_json(&sb, &doc), "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [\"Hello\", \"World\"]}") == 0);

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
        ASSERT_TRUE(strcmp(rec_to_json(&sb, &doc), "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [\"Hello\", \"World\"]}") == 0);

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

        key_unsigned_value(&id_read, &rev_doc);
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

        key_unsigned_value(&id_read, &rev_doc);
        ASSERT_EQ(id_read, 42U);

        rec_drop(&doc);
        rec_drop(&rev_doc);
        str_buf_drop(&sb);
}

TEST(CarbonTest, CarbonKeyTypeSignedKeyUpdate)
{
        rec doc, rev_doc;
        rec_new context;
        rev revise;
        str_buf sb;

        str_buf_create(&sb);

        i64 id_read;

        // -------------------------------------------------------------------------------------------------------------
        insert *ins = rec_create_begin(&context, &doc, KEY_IKEY, OPTIMIZE);

        insert_string(ins, "Hello");
        insert_string(ins, "World");

        rec_create_end(&context);

        // -------------------------------------------------------------------------------------------------------------


        revise_begin(&revise, &rev_doc, &doc);
        revise_key_set_signed(&revise, 42);
        revise_end(&revise);

        key_signed_value(&id_read, &rev_doc);
        ASSERT_EQ(id_read, 42U);

        rec_drop(&doc);
        rec_drop(&rev_doc);
        str_buf_drop(&sb);
}

TEST(CarbonTest, CarbonKeyTypeStringKeyUpdate)
{
        rec doc, rev_doc;
        rec_new context;
        rev revise;
        str_buf sb;

        str_buf_create(&sb);

        u64 key_len;

        // -------------------------------------------------------------------------------------------------------------
        insert *ins = rec_create_begin(&context, &doc, KEY_SKEY, OPTIMIZE);

        insert_string(ins, "Hello");
        insert_string(ins, "World");

        rec_create_end(&context);

        // -------------------------------------------------------------------------------------------------------------

        revise_begin(&revise, &rev_doc, &doc);
        revise_key_set_string(&revise, "my_unique_id");
        revise_end(&revise);

        const char *key = key_string_value(&key_len, &rev_doc);
        ASSERT_TRUE(strncmp(key, "my_unique_id", strlen("my_unique_id")) == 0);

        rec_drop(&doc);
        rec_drop(&rev_doc);
        str_buf_drop(&sb);
}

TEST(CarbonTest, CarbonKeyTypeUnsignedKey)
{
        rec doc;
        rec_new context;

        // -------------------------------------------------------------------------------------------------------------
        insert *ins = rec_create_begin(&context, &doc, KEY_UKEY, OPTIMIZE);

        insert_string(ins, "Hello");
        insert_string(ins, "World");

        rec_create_end(&context);

        // -------------------------------------------------------------------------------------------------------------

        key_e type;
        key_type(&type, &doc);
        ASSERT_EQ(type, KEY_UKEY);

        rec_drop(&doc);
}

TEST(CarbonTest, CarbonKeyTypeSignedKeyRevInc)
{
        rec doc;
        rec_new context;
        rev revise;
        u64 rev_old, rev_new;

        // -------------------------------------------------------------------------------------------------------------
        insert *ins = rec_create_begin(&context, &doc, KEY_IKEY, OPTIMIZE);

        insert_string(ins, "Hello");
        insert_string(ins, "World");

        rec_create_end(&context);

        // -------------------------------------------------------------------------------------------------------------

        u64 test_max = 10000;

        vec ofType(carbon) files;
        vector_create(&files, sizeof(rec), test_max);
        rec* old_f = &doc;


        for (unsigned i = 0; i < test_max; i++) {

                rec_commit_hash(&rev_old, old_f);

                rec* new_f = VECTOR_NEW_AND_GET(&files, rec);

                revise_begin(&revise, new_f, old_f);
                revise_end(&revise);

                rec_commit_hash(&rev_new, new_f);

                ASSERT_NE(rev_old, rev_new);

                old_f = new_f;
        }


}

TEST(CarbonTest, CarbonKeyTypeUnsignedKeyRevInc)
{
        rec doc, rev_doc;
        rec_new context;
        rev revise;
        u64 rev_old, rev_new;

        // -------------------------------------------------------------------------------------------------------------
        insert *ins = rec_create_begin(&context, &doc, KEY_UKEY, OPTIMIZE);

        insert_string(ins, "Hello");
        insert_string(ins, "World");

        rec_create_end(&context);

        // -------------------------------------------------------------------------------------------------------------

        rec_commit_hash(&rev_old, &doc);

        revise_begin(&revise, &rev_doc, &doc);
        revise_end(&revise);

        rec_commit_hash(&rev_new, &rev_doc);

        ASSERT_NE(rev_old, rev_new);

        rec_drop(&doc);
        rec_drop(&rev_doc);
}

TEST(CarbonTest, CarbonKeyTypeSignedKey)
{
        rec doc;
        rec_new context;

        // -------------------------------------------------------------------------------------------------------------
        insert *ins = rec_create_begin(&context, &doc, KEY_IKEY, OPTIMIZE);

        insert_string(ins, "Hello");
        insert_string(ins, "World");

        rec_create_end(&context);

        // -------------------------------------------------------------------------------------------------------------

        key_e type;
        key_type(&type, &doc);
        ASSERT_EQ(type, KEY_IKEY);

        rec_drop(&doc);
}

TEST(CarbonTest, CarbonKeyTypeStringKey)
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
        ASSERT_TRUE(strcmp(rec_to_json(&sb, &doc), "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [\"Hello\", \"World\"]}") == 0);

        str_buf_drop(&sb);
        rec_drop(&doc);
}

TEST(CarbonTest, CarbonObjectInsertEmpty)
{
        rec doc;
        rec_new context;
        obj_state state;

        // -------------------------------------------------------------------------------------------------------------

        insert *ins = rec_create_begin(&context, &doc, KEY_NOKEY, OPTIMIZE);

        insert_object_begin(&state, ins, 1024);
        insert_object_end(&state);

        rec_create_end(&context);

        // -------------------------------------------------------------------------------------------------------------

        str_buf sb;
        str_buf_create(&sb);

        // rec_print(stdout, &doc);
        ASSERT_TRUE(strcmp(rec_to_json(&sb, &doc), "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [{}]}") == 0);

        str_buf_drop(&sb);
        rec_drop(&doc);
}

TEST(CarbonTest, CarbonObjectInsertNull)
{
        rec doc;
        rec_new context;
        obj_state state;

        // -------------------------------------------------------------------------------------------------------------

        insert *ins = rec_create_begin(&context, &doc, KEY_NOKEY, OPTIMIZE);

        insert *obj_ins = insert_object_begin(&state, ins, 1024);
        insert_prop_null(obj_ins, "My Key");
        insert_object_end(&state);

        rec_create_end(&context);

        // -------------------------------------------------------------------------------------------------------------

        str_buf sb;
        str_buf_create(&sb);

        // rec_print(stdout, &doc);
        ASSERT_TRUE(strcmp(rec_to_json(&sb, &doc), "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [{\"My Key\": null}]}") == 0);

        str_buf_drop(&sb);
        rec_drop(&doc);
}

TEST(CarbonTest, CarbonObjectInsertMultipleNulls)
{
        rec doc;
        rec_new context;
        obj_state state;

        // -------------------------------------------------------------------------------------------------------------

        insert *ins = rec_create_begin(&context, &doc, KEY_NOKEY, OPTIMIZE);

        insert *obj_ins = insert_object_begin(&state, ins, 1);
        insert_prop_null(obj_ins, "My Key 1");
        insert_prop_null(obj_ins, "My Key 2");
        insert_prop_null(obj_ins, "My Key 3");
        insert_object_end(&state);

        rec_create_end(&context);

        // -------------------------------------------------------------------------------------------------------------

        str_buf sb;
        str_buf_create(&sb);

        ASSERT_TRUE(strcmp(rec_to_json(&sb, &doc), "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [{\"My Key 1\": null, \"My Key 2\": null, \"My Key 3\": null}]}") == 0);

        str_buf_drop(&sb);
        rec_drop(&doc);
}

TEST(CarbonTest, CarbonObjectInsertU8)
{
        rec doc;
        rec_new context;
        obj_state state;

        // -------------------------------------------------------------------------------------------------------------

        insert *ins = rec_create_begin(&context, &doc, KEY_NOKEY, OPTIMIZE);

        insert *obj_ins = insert_object_begin(&state, ins, 1024);
        insert_prop_u8(obj_ins, "My Key", 123);
        insert_object_end(&state);

        rec_create_end(&context);

        // -------------------------------------------------------------------------------------------------------------

        str_buf sb;
        str_buf_create(&sb);

        // rec_print(stdout, &doc);
        ASSERT_TRUE(strcmp(rec_to_json(&sb, &doc), "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [{\"My Key\": 123}]}") == 0);

        str_buf_drop(&sb);
        rec_drop(&doc);
}

TEST(CarbonTest, CarbonObjectInsertMultipleU8s)
{
        rec doc;
        rec_new context;
        obj_state state;

        // -------------------------------------------------------------------------------------------------------------

        insert *ins = rec_create_begin(&context, &doc, KEY_NOKEY, OPTIMIZE);

        insert *obj_ins = insert_object_begin(&state, ins, 1);
        insert_prop_u8(obj_ins, "My Key 1", 1);
        insert_prop_u8(obj_ins, "My Key 2", 2);
        insert_prop_u8(obj_ins, "My Key 3", 3);
        insert_object_end(&state);

        rec_create_end(&context);

        // -------------------------------------------------------------------------------------------------------------

        str_buf sb;
        str_buf_create(&sb);

        // rec_print(stdout, &doc);
        ASSERT_TRUE(strcmp(rec_to_json(&sb, &doc), "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [{\"My Key 1\": 1, \"My Key 2\": 2, \"My Key 3\": 3}]}") == 0);

        str_buf_drop(&sb);
        rec_drop(&doc);
}

TEST(CarbonTest, CarbonObjectInsertU16)
{
        rec doc;
        rec_new context;
        obj_state state;

        // -------------------------------------------------------------------------------------------------------------

        insert *ins = rec_create_begin(&context, &doc, KEY_NOKEY, OPTIMIZE);

        insert *obj_ins = insert_object_begin(&state, ins, 1024);
        insert_prop_u16(obj_ins, "My Key", 123);
        insert_object_end(&state);

        rec_create_end(&context);

        // -------------------------------------------------------------------------------------------------------------

        str_buf sb;
        str_buf_create(&sb);

        // rec_print(stdout, &doc);
        ASSERT_TRUE(strcmp(rec_to_json(&sb, &doc), "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [{\"My Key\": 123}]}") == 0);

        str_buf_drop(&sb);
        rec_drop(&doc);
}

TEST(CarbonTest, CarbonObjectInsertMultipleU16s)
{
        rec doc;
        rec_new context;
        obj_state state;

        // -------------------------------------------------------------------------------------------------------------

        insert *ins = rec_create_begin(&context, &doc, KEY_NOKEY, OPTIMIZE);

        insert *obj_ins = insert_object_begin(&state, ins, 1);
        insert_prop_u16(obj_ins, "My Key 1", 1);
        insert_prop_u16(obj_ins, "My Key 2", 2);
        insert_prop_u16(obj_ins, "My Key 3", 3);
        insert_object_end(&state);

        rec_create_end(&context);

        // -------------------------------------------------------------------------------------------------------------

        str_buf sb;
        str_buf_create(&sb);

        // rec_print(stdout, &doc);
        ASSERT_TRUE(strcmp(rec_to_json(&sb, &doc), "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [{\"My Key 1\": 1, \"My Key 2\": 2, \"My Key 3\": 3}]}") == 0);

        str_buf_drop(&sb);
        rec_drop(&doc);
}

TEST(CarbonTest, CarbonObjectInsertU32)
{
        rec doc;
        rec_new context;
        obj_state state;

        // -------------------------------------------------------------------------------------------------------------

        insert *ins = rec_create_begin(&context, &doc, KEY_NOKEY, OPTIMIZE);

        insert *obj_ins = insert_object_begin(&state, ins, 1024);
        insert_prop_u32(obj_ins, "My Key", 123);
        insert_object_end(&state);

        rec_create_end(&context);

        // -------------------------------------------------------------------------------------------------------------

        str_buf sb;
        str_buf_create(&sb);

        // rec_print(stdout, &doc);
        ASSERT_TRUE(strcmp(rec_to_json(&sb, &doc), "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [{\"My Key\": 123}]}") == 0);

        str_buf_drop(&sb);
        rec_drop(&doc);
}

TEST(CarbonTest, CarbonObjectInsertMultipleU32s)
{
        rec doc;
        rec_new context;
        obj_state state;

        // -------------------------------------------------------------------------------------------------------------

        insert *ins = rec_create_begin(&context, &doc, KEY_NOKEY, OPTIMIZE);

        insert *obj_ins = insert_object_begin(&state, ins, 1);
        insert_prop_u32(obj_ins, "My Key 1", 1);
        insert_prop_u32(obj_ins, "My Key 2", 2);
        insert_prop_u32(obj_ins, "My Key 3", 3);
        insert_object_end(&state);

        rec_create_end(&context);

        // -------------------------------------------------------------------------------------------------------------

        str_buf sb;
        str_buf_create(&sb);

        // rec_print(stdout, &doc);
        ASSERT_TRUE(strcmp(rec_to_json(&sb, &doc), "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [{\"My Key 1\": 1, \"My Key 2\": 2, \"My Key 3\": 3}]}") == 0);

        str_buf_drop(&sb);
        rec_drop(&doc);
}

TEST(CarbonTest, CarbonObjectInsertU64)
{
        rec doc;
        rec_new context;
        obj_state state;

        // -------------------------------------------------------------------------------------------------------------

        insert *ins = rec_create_begin(&context, &doc, KEY_NOKEY, OPTIMIZE);

        insert *obj_ins = insert_object_begin(&state, ins, 1024);
        insert_prop_u64(obj_ins, "My Key", 123);
        insert_object_end(&state);

        rec_create_end(&context);

        // -------------------------------------------------------------------------------------------------------------

        str_buf sb;
        str_buf_create(&sb);

        // rec_print(stdout, &doc);
        ASSERT_TRUE(strcmp(rec_to_json(&sb, &doc), "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [{\"My Key\": 123}]}") == 0);

        str_buf_drop(&sb);
        rec_drop(&doc);
}

TEST(CarbonTest, CarbonObjectInsertMultipleU64s)
{
        rec doc;
        rec_new context;
        obj_state state;

        // -------------------------------------------------------------------------------------------------------------

        insert *ins = rec_create_begin(&context, &doc, KEY_NOKEY, OPTIMIZE);

        insert *obj_ins = insert_object_begin(&state, ins, 1);
        insert_prop_u64(obj_ins, "My Key 1", 1);
        insert_prop_u64(obj_ins, "My Key 2", 2);
        insert_prop_u64(obj_ins, "My Key 3", 3);
        insert_object_end(&state);

        rec_create_end(&context);

        // -------------------------------------------------------------------------------------------------------------

        str_buf sb;
        str_buf_create(&sb);

        //rec_print(stdout, &doc);
        ASSERT_TRUE(strcmp(rec_to_json(&sb, &doc), "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [{\"My Key 1\": 1, \"My Key 2\": 2, \"My Key 3\": 3}]}") == 0);

        str_buf_drop(&sb);
        rec_drop(&doc);
}

TEST(CarbonTest, CarbonObjectInsertI8)
{
        rec doc;
        rec_new context;
        obj_state state;

        // -------------------------------------------------------------------------------------------------------------

        insert *ins = rec_create_begin(&context, &doc, KEY_NOKEY, OPTIMIZE);

        insert *obj_ins = insert_object_begin(&state, ins, 1024);
        insert_prop_i8(obj_ins, "My Key", -123);
        insert_object_end(&state);

        rec_create_end(&context);

        // -------------------------------------------------------------------------------------------------------------

        str_buf sb;
        str_buf_create(&sb);

        // rec_print(stdout, &doc);
        ASSERT_TRUE(strcmp(rec_to_json(&sb, &doc), "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [{\"My Key\": -123}]}") == 0);

        str_buf_drop(&sb);
        rec_drop(&doc);
}

TEST(CarbonTest, CarbonObjectInsertMultipleI8s)
{
        rec doc;
        rec_new context;
        obj_state state;

        // -------------------------------------------------------------------------------------------------------------

        insert *ins = rec_create_begin(&context, &doc, KEY_NOKEY, OPTIMIZE);

        insert *obj_ins = insert_object_begin(&state, ins, 1);
        insert_prop_i8(obj_ins, "My Key 1", -1);
        insert_prop_i8(obj_ins, "My Key 2", -2);
        insert_prop_i8(obj_ins, "My Key 3", -3);
        insert_object_end(&state);

        rec_create_end(&context);

        // -------------------------------------------------------------------------------------------------------------

        str_buf sb;
        str_buf_create(&sb);

        // rec_print(stdout, &doc);
        ASSERT_TRUE(strcmp(rec_to_json(&sb, &doc), "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [{\"My Key 1\": -1, \"My Key 2\": -2, \"My Key 3\": -3}]}") == 0);

        str_buf_drop(&sb);
        rec_drop(&doc);
}

TEST(CarbonTest, CarbonObjectInsertI16)
{
        rec doc;
        rec_new context;
        obj_state state;

        // -------------------------------------------------------------------------------------------------------------

        insert *ins = rec_create_begin(&context, &doc, KEY_NOKEY, OPTIMIZE);

        insert *obj_ins = insert_object_begin(&state, ins, 1024);
        insert_prop_i16(obj_ins, "My Key", -123);
        insert_object_end(&state);

        rec_create_end(&context);

        // -------------------------------------------------------------------------------------------------------------

        str_buf sb;
        str_buf_create(&sb);

        // rec_print(stdout, &doc);
        ASSERT_TRUE(strcmp(rec_to_json(&sb, &doc), "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [{\"My Key\": -123}]}") == 0);

        str_buf_drop(&sb);
        rec_drop(&doc);
}

TEST(CarbonTest, CarbonObjectInsertMultipleI16s)
{
        rec doc;
        rec_new context;
        obj_state state;

        // -------------------------------------------------------------------------------------------------------------

        insert *ins = rec_create_begin(&context, &doc, KEY_NOKEY, OPTIMIZE);

        insert *obj_ins = insert_object_begin(&state, ins, 1);
        insert_prop_i16(obj_ins, "My Key 1", -1);
        insert_prop_i16(obj_ins, "My Key 2", -2);
        insert_prop_i16(obj_ins, "My Key 3", -3);
        insert_object_end(&state);

        rec_create_end(&context);

        // -------------------------------------------------------------------------------------------------------------

        str_buf sb;
        str_buf_create(&sb);

        // rec_print(stdout, &doc);
        ASSERT_TRUE(strcmp(rec_to_json(&sb, &doc), "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [{\"My Key 1\": -1, \"My Key 2\": -2, \"My Key 3\": -3}]}") == 0);

        str_buf_drop(&sb);
        rec_drop(&doc);
}

TEST(CarbonTest, CarbonObjectInsertI32)
{
        rec doc;
        rec_new context;
        obj_state state;

        // -------------------------------------------------------------------------------------------------------------

        insert *ins = rec_create_begin(&context, &doc, KEY_NOKEY, OPTIMIZE);

        insert *obj_ins = insert_object_begin(&state, ins, 1024);
        insert_prop_i32(obj_ins, "My Key", -123);
        insert_object_end(&state);

        rec_create_end(&context);

        // -------------------------------------------------------------------------------------------------------------

        str_buf sb;
        str_buf_create(&sb);

        // rec_print(stdout, &doc);
        ASSERT_TRUE(strcmp(rec_to_json(&sb, &doc), "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [{\"My Key\": -123}]}") == 0);

        str_buf_drop(&sb);
        rec_drop(&doc);
}

TEST(CarbonTest, CarbonObjectInsertMultipleI32s)
{
        rec doc;
        rec_new context;
        obj_state state;

        // -------------------------------------------------------------------------------------------------------------

        insert *ins = rec_create_begin(&context, &doc, KEY_NOKEY, OPTIMIZE);

        insert *obj_ins = insert_object_begin(&state, ins, 1);
        insert_prop_i32(obj_ins, "My Key 1", -1);
        insert_prop_i32(obj_ins, "My Key 2", -2);
        insert_prop_i32(obj_ins, "My Key 3", -3);
        insert_object_end(&state);

        rec_create_end(&context);

        // -------------------------------------------------------------------------------------------------------------

        str_buf sb;
        str_buf_create(&sb);

        // rec_print(stdout, &doc);
        ASSERT_TRUE(strcmp(rec_to_json(&sb, &doc), "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [{\"My Key 1\": -1, \"My Key 2\": -2, \"My Key 3\": -3}]}") == 0);

        str_buf_drop(&sb);
        rec_drop(&doc);
}

TEST(CarbonTest, CarbonObjectInsertI64)
{
        rec doc;
        rec_new context;
        obj_state state;

        // -------------------------------------------------------------------------------------------------------------

        insert *ins = rec_create_begin(&context, &doc, KEY_NOKEY, OPTIMIZE);

        insert *obj_ins = insert_object_begin(&state, ins, 1024);
        insert_prop_i64(obj_ins, "My Key", -123);
        insert_object_end(&state);

        rec_create_end(&context);

        // -------------------------------------------------------------------------------------------------------------

        str_buf sb;
        str_buf_create(&sb);

        // rec_print(stdout, &doc);
        ASSERT_TRUE(strcmp(rec_to_json(&sb, &doc), "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [{\"My Key\": -123}]}") == 0);

        str_buf_drop(&sb);
        rec_drop(&doc);
}

TEST(CarbonTest, CarbonObjectInsertMultipleI64s)
{
        rec doc;
        rec_new context;
        obj_state state;

        // -------------------------------------------------------------------------------------------------------------

        insert *ins = rec_create_begin(&context, &doc, KEY_NOKEY, OPTIMIZE);

        insert *obj_ins = insert_object_begin(&state, ins, 1);
        insert_prop_i64(obj_ins, "My Key 1", -1);
        insert_prop_i64(obj_ins, "My Key 2", -2);
        insert_prop_i64(obj_ins, "My Key 3", -3);
        insert_object_end(&state);

        rec_create_end(&context);

        // -------------------------------------------------------------------------------------------------------------

        str_buf sb;
        str_buf_create(&sb);

        // rec_print(stdout, &doc);
        ASSERT_TRUE(strcmp(rec_to_json(&sb, &doc), "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [{\"My Key 1\": -1, \"My Key 2\": -2, \"My Key 3\": -3}]}") == 0);

        str_buf_drop(&sb);
        rec_drop(&doc);
}

TEST(CarbonTest, CarbonObjectInsertFloat)
{
        rec doc;
        rec_new context;
        obj_state state;

        // -------------------------------------------------------------------------------------------------------------

        insert *ins = rec_create_begin(&context, &doc, KEY_NOKEY, OPTIMIZE);

        insert *obj_ins = insert_object_begin(&state, ins, 1024);
        insert_prop_float(obj_ins, "My Key", -123.32);
        insert_object_end(&state);

        rec_create_end(&context);

        // -------------------------------------------------------------------------------------------------------------

        str_buf sb;
        str_buf_create(&sb);

        // rec_print(stdout, &doc);
        ASSERT_TRUE(strcmp(rec_to_json(&sb, &doc), "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [{\"My Key\": -123.32}]}") == 0);

        str_buf_drop(&sb);
        rec_drop(&doc);
}

TEST(CarbonTest, CarbonObjectInsertMultipleFloats)
{
        rec doc;
        rec_new context;
        obj_state state;

        // -------------------------------------------------------------------------------------------------------------

        insert *ins = rec_create_begin(&context, &doc, KEY_NOKEY, OPTIMIZE);

        insert *obj_ins = insert_object_begin(&state, ins, 1);
        insert_prop_float(obj_ins, "My Key 1", -1.23);
        insert_prop_float(obj_ins, "My Key 2", -2.42);
        insert_prop_float(obj_ins, "My Key 3", 3.21);
        insert_object_end(&state);

        rec_create_end(&context);

        // -------------------------------------------------------------------------------------------------------------

        str_buf sb;
        str_buf_create(&sb);

        // rec_print(stdout, &doc);
        ASSERT_TRUE(strcmp(rec_to_json(&sb, &doc), "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [{\"My Key 1\": -1.23, \"My Key 2\": -2.42, \"My Key 3\": 3.21}]}") == 0);

        str_buf_drop(&sb);
        rec_drop(&doc);
}

TEST(CarbonTest, CarbonObjectInsertTrue)
{
        rec doc;
        rec_new context;
        obj_state state;

        // -------------------------------------------------------------------------------------------------------------

        insert *ins = rec_create_begin(&context, &doc, KEY_NOKEY, OPTIMIZE);

        insert *obj_ins = insert_object_begin(&state, ins, 1024);
        insert_prop_true(obj_ins, "My Key");
        insert_object_end(&state);

        rec_create_end(&context);

        // -------------------------------------------------------------------------------------------------------------

        str_buf sb;
        str_buf_create(&sb);

        // rec_print(stdout, &doc);
        ASSERT_TRUE(strcmp(rec_to_json(&sb, &doc), "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [{\"My Key\": true}]}") == 0);

        str_buf_drop(&sb);
        rec_drop(&doc);
}

TEST(CarbonTest, CarbonObjectInsertFalse)
{
        rec doc;
        rec_new context;
        obj_state state;

        // -------------------------------------------------------------------------------------------------------------

        insert *ins = rec_create_begin(&context, &doc, KEY_NOKEY, OPTIMIZE);

        insert *obj_ins = insert_object_begin(&state, ins, 1024);
        insert_prop_false(obj_ins, "My Key");
        insert_object_end(&state);

        rec_create_end(&context);

        // -------------------------------------------------------------------------------------------------------------

        str_buf sb;
        str_buf_create(&sb);

        // rec_print(stdout, &doc);
        ASSERT_TRUE(strcmp(rec_to_json(&sb, &doc), "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [{\"My Key\": false}]}") == 0);

        str_buf_drop(&sb);
        rec_drop(&doc);
}

TEST(CarbonTest, CarbonObjectInsertMultipleBooleans)
{
        rec doc;
        rec_new context;
        obj_state state;

        // -------------------------------------------------------------------------------------------------------------

        insert *ins = rec_create_begin(&context, &doc, KEY_NOKEY, OPTIMIZE);

        insert *obj_ins = insert_object_begin(&state, ins, 1);
        insert_prop_true(obj_ins, "My Key 1");
        insert_prop_false(obj_ins, "My Key 2");
        insert_prop_true(obj_ins, "My Key 3");
        insert_object_end(&state);

        rec_create_end(&context);

        // -------------------------------------------------------------------------------------------------------------

        str_buf sb;
        str_buf_create(&sb);

        // rec_print(stdout, &doc);
        ASSERT_TRUE(strcmp(rec_to_json(&sb, &doc), "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [{\"My Key 1\": true, \"My Key 2\": false, \"My Key 3\": true}]}") == 0);

        str_buf_drop(&sb);
        rec_drop(&doc);
}

TEST(CarbonTest, CarbonObjectInsertMixed)
{
        rec doc;
        rec_new context;
        obj_state state;

        // -------------------------------------------------------------------------------------------------------------

        insert *ins = rec_create_begin(&context, &doc, KEY_NOKEY, OPTIMIZE);

        insert *obj_ins = insert_object_begin(&state, ins, 1);
        insert_prop_true(obj_ins, "k1");
        insert_prop_false(obj_ins, "k2");
        insert_prop_null(obj_ins, "k3");
        insert_prop_u8(obj_ins, "k4", 1);
        insert_prop_u16(obj_ins, "k5", 2);
        insert_prop_u32(obj_ins, "k6", 3);
        insert_prop_u64(obj_ins, "k7", 4);
        insert_prop_i8(obj_ins, "k8", -1);
        insert_prop_i16(obj_ins, "k9", -2);
        insert_prop_i32(obj_ins, "k10", -3);
        insert_prop_i64(obj_ins, "k11", -4);
        insert_prop_float(obj_ins, "k12", 42.23);

        insert_object_end(&state);

        rec_create_end(&context);

        // -------------------------------------------------------------------------------------------------------------

        str_buf sb;
        str_buf_create(&sb);

        // rec_print(stdout, &doc);
        ASSERT_TRUE(strcmp(rec_to_json(&sb, &doc), "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [{\"k1\": true, \"k2\": false, \"k3\": null, \"k4\": 1, \"k5\": 2, \"k6\": 3, \"k7\": 4, \"k8\": -1, \"k9\": -2, \"k10\": -3, \"k11\": -4, \"k12\": 42.23}]}") == 0);

        str_buf_drop(&sb);
        rec_drop(&doc);
}

TEST(CarbonTest, CarbonObjectInsertString)
{
        rec doc;
        rec_new context;
        obj_state state;

        // -------------------------------------------------------------------------------------------------------------

        insert *ins = rec_create_begin(&context, &doc, KEY_NOKEY, OPTIMIZE);

        insert *obj_ins = insert_object_begin(&state, ins, 1);

        insert_prop_string(obj_ins, "hello", "world");

        insert_object_end(&state);

        rec_create_end(&context);

        // -------------------------------------------------------------------------------------------------------------

        str_buf sb;
        str_buf_create(&sb);

        // rec_print(stdout, &doc);
        ASSERT_TRUE(strcmp(rec_to_json(&sb, &doc), "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [{\"hello\": \"world\"}]}") == 0);

        str_buf_drop(&sb);
        rec_drop(&doc);
}

TEST(CarbonTest, CarbonObjectInsertMultipleString)
{
        rec doc;
        rec_new context;
        obj_state state;

        // -------------------------------------------------------------------------------------------------------------

        insert *ins = rec_create_begin(&context, &doc, KEY_NOKEY, OPTIMIZE);

        insert *obj_ins = insert_object_begin(&state, ins, 1);

        insert_prop_string(obj_ins, "k1", "v1");
        insert_prop_string(obj_ins, "hello", "world");
        insert_prop_string(obj_ins, "k3", "there");

        insert_object_end(&state);

        rec_create_end(&context);

        // -------------------------------------------------------------------------------------------------------------

        str_buf sb;
        str_buf_create(&sb);

        // rec_print(stdout, &doc);
        ASSERT_TRUE(strcmp(rec_to_json(&sb, &doc), "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [{\"k1\": \"v1\", \"hello\": \"world\", \"k3\": \"there\"}]}") == 0);

        str_buf_drop(&sb);
        rec_drop(&doc);
}

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
        ASSERT_TRUE(strcmp(rec_to_json(&sb, &doc), "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [{\"k2\": false, \"k3\": null, \"k4\": 1, \"s1\": \"v1\", \"k5\": 2, \"s2-longer\": \"world\", \"k6\": 3, \"k7\": 4, \"k8\": -1, \"s3\": \"there\", \"k9\": -2, \"k10\": -3, \"k11\": -4, \"k12\": 42.23, \"k1\": true}]}") == 0);

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
        ASSERT_TRUE(strcmp(rec_to_json(&sb, &doc), "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [{\"my binary\": { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"TXkgUGxhaW4tVGV4dAAA\" }}]}") == 0);

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
        ASSERT_TRUE(strcmp(rec_to_json(&sb, &doc), "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [{\"k12\": 42.23, \"k1\": true, \"b1\": { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"A==sbG8AA\" }, \"my binary\": { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"LAAA\" }, \"k2\": false, \"k3\": null, \"k4\": 1, \"s1\": \"v1\", \"k5\": 2, \"b2\": { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"A==ybGQAA\" }, \"s2-longer\": \"world\", \"k6\": 3, \"k7\": 4, \"k8\": -1, \"s3\": \"there\", \"k9\": -2, \"k10\": -3, \"k11\": -4}]}") == 0);

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
        ASSERT_TRUE(strcmp(rec_to_json(&sb, &doc), "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [{\"b1\": { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"A==sbG8AA\" }, \"my binary\": { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"LAAA\" }, \"b2\": { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"A==ybGQAA\" }}]}") == 0);

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
        ASSERT_TRUE(strcmp(rec_to_json(&sb, &doc), "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [{\"my nested\": {}}]}") == 0);

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
        ASSERT_TRUE(strcmp(rec_to_json(&sb, &doc), "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [{\"1\": 42.23, \"2\": true, \"3\": { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"A==sbG8AA\" }, \"4\": { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"LAAA\" }, \"5\": { \"type\": \"text/plain\", \"encoding\": \"base64\", \"binary-str_buf\": \"A==ybGQAA\" }, \"6\": \"world\", \"my nested\": {\"7\": false, \"8\": null, \"9\": 1, \"10\": \"v1\", \"11\": 2}, \"12\": 3, \"13\": 4, \"14\": -1, \"15\": \"there\", \"16\": -2, \"17\": -3, \"18\": -4}]}") == 0);

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
        ASSERT_TRUE(strcmp(rec_to_json(&sb, &doc), "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [{\"my array\": []}]}") == 0);

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
        ASSERT_TRUE(strcmp(rec_to_json(&sb, &doc), "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [{\"my array\": [[88, 89, 90], [\"Hello\", [65, 66, 67], \"World\"], 1, 1, [23, 24, 25], 1]}]}") == 0);

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
        ASSERT_TRUE(strcmp(rec_to_json(&sb, &doc), "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [{\"my column\": [1, 2, 3]}]}") == 0);

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

        ASSERT_TRUE(strcmp(json_1, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [{\"1\": false, \"2\": null, \"3\": 1, \"4\": \"v1\", \"5\": 2}, {\"6\": false, \"7\": null, \"8\": 1, \"9\": \"v1\", \"10\": 2}, {\"11\": false, \"12\": null, \"13\": 1, \"14\": \"v1\", \"15\": 2}]}") == 0);
        ASSERT_TRUE(strcmp(json_2, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": []}") == 0);

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

        ASSERT_TRUE(strcmp(json_1, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [{\"1\": false, \"2\": null, \"3\": 1, \"4\": \"v1\", \"5\": 2}, {\"6\": false, \"7\": null, \"8\": 1, \"9\": \"v1\", \"10\": 2}, {\"11\": false, \"12\": null, \"13\": 1, \"14\": \"v1\", \"15\": 2}]}") == 0);
        ASSERT_TRUE(strcmp(json_2, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [{\"1\": false, \"2\": null, \"3\": 1, \"4\": \"v1\", \"5\": 2}]}") == 0);

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

        ASSERT_TRUE(strcmp(json_1, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [{\"1\": false, \"2\": null, \"3\": 1, \"4\": \"v1\", \"5\": 2}]}") == 0);
        ASSERT_TRUE(strcmp(json_2, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [{\"Hello Long Key\": \"Hello Long Value\", \"1\": false, \"2\": null, \"3\": 1, \"4\": \"v1\", \"5\": 2}]}") == 0);

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

        ASSERT_TRUE(strcmp(json_1, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [{\"1\": false, \"2\": null, \"3\": 1, \"4\": \"v1\", \"5\": 2}]}") == 0);
        ASSERT_TRUE(strcmp(json_2, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [{\"1\": false, \"Hello Long Key\": \"Hello Long Value\", \"2\": null, \"3\": 1, \"4\": \"v1\", \"5\": 2}]}") == 0);

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

        ASSERT_TRUE(strcmp(json_1, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [{\"1\": false, \"2\": null, \"3\": 1, \"4\": \"v1\", \"5\": 2}]}") == 0);
        ASSERT_TRUE(strcmp(json_2, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [{\"1\": false, \"2\": null, \"Hello Long Key\": \"Hello Long Value\", \"3\": 1, \"4\": \"v1\", \"5\": 2}]}") == 0);

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

        ASSERT_TRUE(strcmp(json_1, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [{\"1\": false, \"2\": null, \"3\": 1, \"4\": \"v1\", \"5\": 2}]}") == 0);
        ASSERT_TRUE(strcmp(json_2, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [{\"1\": false, \"2\": null, \"3\": 1, \"Hello Long Key\": \"Hello Long Value\", \"4\": \"v1\", \"5\": 2}]}") == 0);

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

        ASSERT_TRUE(strcmp(json_1, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [{\"1\": false, \"2\": null, \"3\": 1, \"4\": \"v1\", \"5\": 2}]}") == 0);
        ASSERT_TRUE(strcmp(json_2, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [{\"1\": false, \"2\": null, \"3\": 1, \"4\": \"v1\", \"Hello Long Key\": \"Hello Long Value\", \"5\": 2}]}") == 0);

        str_buf_drop(&sb);
        rec_drop(&doc);
        rec_drop(&rev_doc);
        free(json_1);
        free(json_2);
}

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

TEST(CarbonTest, CarbonFromEmptyJson)
{
        rec doc;

        const char *json_in;
        const char *json_out_compact, *json_out_extended;

        json_in = "{}";
        rec_from_json(&doc, json_in, KEY_NOKEY, NULL);
        str_buf sb1, sb2;
        str_buf_create(&sb1);
        str_buf_create(&sb2);
        json_out_extended = rec_to_json(&sb1, &doc);
        json_out_compact = rec_to_json(&sb2, &doc);

        //printf("INS:\t%s\n", json_in);
        //printf("EXT:\t%s\n", json_out_extended);
        //printf("SRT:\t%s\n", json_out_compact);

        rec_drop(&doc);

        ASSERT_TRUE(strcmp(json_out_compact, json_in) == 0);
        ASSERT_TRUE(strcmp(json_out_extended, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [{}]}") == 0);

        str_buf_drop(&sb1);
        str_buf_drop(&sb2);
}

TEST(CarbonTest, CarbonFromEmptyArray)
{
        rec doc;

        const char *json_in;
        const char *json_out_compact, *json_out_extended;

        json_in = "[]";
        rec_from_json(&doc, json_in, KEY_NOKEY, NULL);
        str_buf sb1, sb2;
        str_buf_create(&sb1);
        str_buf_create(&sb2);
        json_out_extended = rec_to_json(&sb1, &doc);
        json_out_compact = rec_to_json(&sb2, &doc);

//        printf("INS:\t%s\n", json_in);
//        printf("EXT:\t%s\n", json_out_extended);
//        printf("SRT:\t%s\n", json_out_compact);

        rec_drop(&doc);

        ASSERT_TRUE(strcmp(json_out_compact, "{}") == 0);
        ASSERT_TRUE(strcmp(json_out_extended, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": []}") == 0);

        str_buf_drop(&sb1);
        str_buf_drop(&sb2);
}

TEST(CarbonTest, CarbonFromJsonNull)
{
        rec doc;

        const char *json_in;
        const char *json_out_compact, *json_out_extended;

        json_in = "null";

        rec_from_json(&doc, json_in, KEY_NOKEY, NULL);
        str_buf sb1, sb2;
        str_buf_create(&sb1);
        str_buf_create(&sb2);
        json_out_extended = rec_to_json(&sb1, &doc);  // shall be '{"meta": {"key": {"type": "nokey", "value": null}, "rev": 0}, "doc": [null]}'
        json_out_compact = rec_to_json(&sb2, &doc);    // shall be 'null'

        //printf("EXT:\t%s\n", json_out_extended);
        //printf("SRT:\t%s\n", json_out_compact);

        rec_drop(&doc);

        ASSERT_TRUE(strcmp(json_out_compact, json_in) == 0);
        ASSERT_TRUE(strcmp(json_out_extended, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [null]}") == 0);

        str_buf_drop(&sb1);
        str_buf_drop(&sb2);
}

TEST(CarbonTest, CarbonFromJsonTrue)
{
        rec doc;

        const char *json_in;
        const char *json_out_compact, *json_out_extended;

        json_in = "true";

        rec_from_json(&doc, json_in, KEY_NOKEY, NULL);

        str_buf sb1, sb2;
        str_buf_create(&sb1);
        str_buf_create(&sb2);
        json_out_extended = rec_to_json(&sb1, &doc);  // shall be '{"meta": {"key": {"type": "nokey", "value": null}, "rev": 0}, "doc": [true]}'
        json_out_compact = rec_to_json(&sb2, &doc);    // shall be 'true'

        //printf("EXT:\t%s\n", json_out_extended);
        //printf("SRT:\t%s\n", json_out_compact);

        rec_drop(&doc);

        ASSERT_TRUE(strcmp(json_out_compact, json_in) == 0);
        ASSERT_TRUE(strcmp(json_out_extended, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [true]}") == 0);

        str_buf_drop(&sb1);
        str_buf_drop(&sb2);
}

TEST(CarbonTest, CarbonFromJsonFalse)
{
        rec doc;

        const char *json_in;
        const char *json_out_compact, *json_out_extended;

        json_in = "false";

        rec_from_json(&doc, json_in, KEY_NOKEY, NULL);

        str_buf sb1, sb2;
        str_buf_create(&sb1);
        str_buf_create(&sb2);
        json_out_extended = rec_to_json(&sb1, &doc);  // shall be '{"meta": {"key": {"type": "nokey", "value": null}, "rev": 0}, "doc": [false]}'
        json_out_compact = rec_to_json(&sb2, &doc);    // shall be 'false'

        //printf("EXT:\t%s\n", json_out_extended);
        //printf("SRT:\t%s\n", json_out_compact);

        rec_drop(&doc);

        ASSERT_TRUE(strcmp(json_out_compact, json_in) == 0);
        ASSERT_TRUE(strcmp(json_out_extended, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [false]}") == 0);

        str_buf_drop(&sb1);
        str_buf_drop(&sb2);
}

TEST(CarbonTest, CarbonFromJsonNumberSigned)
{
        rec doc;

        const char *json_in;
        const char *json_out_compact, *json_out_extended;

        json_in = "42";

        rec_from_json(&doc, json_in, KEY_NOKEY, NULL);
        str_buf sb1, sb2;
        str_buf_create(&sb1);
        str_buf_create(&sb2);
        json_out_extended = rec_to_json(&sb1, &doc);  // shall be '{"meta": {"key": {"type": "nokey", "value": null}, "rev": 0}, "doc": [42]}'
        json_out_compact = rec_to_json(&sb2, &doc);    // shall be '42'

        //printf("EXT:\t%s\n", json_out_extended);
        //printf("SRT:\t%s\n", json_out_compact);

        rec_drop(&doc);

        ASSERT_TRUE(strcmp(json_out_compact, json_in) == 0);
        ASSERT_TRUE(strcmp(json_out_extended, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [42]}") == 0);

        str_buf_drop(&sb1);
        str_buf_drop(&sb2);
}

TEST(CarbonTest, CarbonFromJsonNumberUnsigned)
{
        rec doc;

        const char *json_in;
        const char *json_out_compact, *json_out_extended;

        json_in = "-42";

        rec_from_json(&doc, json_in, KEY_NOKEY, NULL);

        str_buf sb1, sb2;
        str_buf_create(&sb1);
        str_buf_create(&sb2);
        json_out_extended = rec_to_json(&sb1, &doc);  // shall be '{"meta": {"key": {"type": "nokey", "value": null}, "rev": 0}, "doc": [-42]}'
        json_out_compact = rec_to_json(&sb2, &doc);    // shall be '-42'

        //printf("EXT:\t%s\n", json_out_extended);
        //printf("SRT:\t%s\n", json_out_compact);

        rec_drop(&doc);

        ASSERT_TRUE(strcmp(json_out_compact, json_in) == 0);
        ASSERT_TRUE(strcmp(json_out_extended, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [-42]}") == 0);

        str_buf_drop(&sb1);
        str_buf_drop(&sb2);
}

TEST(CarbonTest, CarbonFromJsonNumberFloat)
{
        rec doc;

        const char *json_in;
        const char *json_out_compact, *json_out_extended;

        json_in = "-42.23";

        rec_from_json(&doc, json_in, KEY_NOKEY, NULL);

        str_buf sb1, sb2;
        str_buf_create(&sb1);
        str_buf_create(&sb2);
        json_out_extended = rec_to_json(&sb1, &doc);  // shall be '{"meta": {"key": {"type": "nokey", "value": null}, "rev": 0}, "doc": [-42.23]}'
        json_out_compact = rec_to_json(&sb2, &doc);    // shall be '-42.23'

        //printf("EXT:\t%s\n", json_out_extended);
        //printf("SRT:\t%s\n", json_out_compact);

        rec_drop(&doc);

        ASSERT_TRUE(strcmp(json_out_compact, json_in) == 0);
        ASSERT_TRUE(strcmp(json_out_extended, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [-42.23]}") == 0);

        str_buf_drop(&sb1);
        str_buf_drop(&sb2);
}

TEST(CarbonTest, CarbonFromJsonString)
{
        rec doc;

        const char *json_in;
        const char *json_out_compact, *json_out_extended;

        json_in = "\"Hello, World!\"";

        rec_from_json(&doc, json_in, KEY_NOKEY, NULL);

        str_buf sb1, sb2;
        str_buf_create(&sb1);
        str_buf_create(&sb2);
        json_out_extended = rec_to_json(&sb1, &doc);  // shall be '{"meta": {"key": {"type": "nokey", "value": null}, "rev": 0}, "doc": ["Hello, World!"]}'
        json_out_compact = rec_to_json(&sb2, &doc);    // shall be '"Hello, World!"'

        //printf("EXT:\t%s\n", json_out_extended);
        //printf("SRT:\t%s\n", json_out_compact);

        rec_drop(&doc);

        ASSERT_TRUE(strcmp(json_out_compact, json_in) == 0);
        ASSERT_TRUE(strcmp(json_out_extended, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [\"Hello, World!\"]}") == 0);

        str_buf_drop(&sb1);
        str_buf_drop(&sb2);
}

TEST(CarbonTest, CarbonFromJsonObjectSingle)
{
        rec doc;

        const char *json_in;
        const char *json_out_compact, *json_out_extended;

        json_in = "{\"k\": \"v\"}";

        rec_from_json(&doc, json_in, KEY_NOKEY, NULL);

        str_buf sb1, sb2;
        str_buf_create(&sb1);
        str_buf_create(&sb2);
        json_out_extended = rec_to_json(&sb1, &doc);  // shall be '{"meta": {"key": {"type": "nokey", "value": null}, "rev": 0}, "doc": [{"k":"v"}]}'
        json_out_compact = rec_to_json(&sb2, &doc);    // shall be '{"k":"v"}'

        //printf("EXT:\t%s\n", json_out_extended);
        //printf("SRT:\t%s\n", json_out_compact);

        rec_drop(&doc);

        ASSERT_TRUE(strcmp(json_out_compact, json_in) == 0);
        ASSERT_TRUE(strcmp(json_out_extended, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [{\"k\": \"v\"}]}") == 0);

        str_buf_drop(&sb1);
        str_buf_drop(&sb2);
}


TEST(CarbonTest, CarbonFromJsonObjectEmptyArrayProp)
{
        rec doc;

        const char *json_in;
        const char *json_out_compact, *json_out_extended;

        json_in = "{\"k\": []}";

        rec_from_json(&doc, json_in, KEY_NOKEY, NULL);

        str_buf sb1, sb2;
        str_buf_create(&sb1);
        str_buf_create(&sb2);
        json_out_extended = rec_to_json(&sb1, &doc);  // shall be '{"meta": {"key": {"type": "nokey", "value": null}, "rev": 0}, "doc": [{"k":[]}]}'
        json_out_compact = rec_to_json(&sb2, &doc);    // shall be '{"k":[]}'

        //printf("EXT:\t%s\n", json_out_extended);
        //printf("SRT:\t%s\n", json_out_compact);

        rec_drop(&doc);

        ASSERT_TRUE(strcmp(json_out_compact, json_in) == 0);
        ASSERT_TRUE(strcmp(json_out_extended, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [{\"k\": []}]}") == 0);

        str_buf_drop(&sb1);
        str_buf_drop(&sb2);
}

TEST(CarbonTest, CarbonFromJsonObjectEmptyObjectProp)
{
        rec doc;
        
        const char *json_in;
        const char *json_out_compact, *json_out_extended;

        json_in = "{\"k\": {}}";

        rec_from_json(&doc, json_in, KEY_NOKEY, NULL);

        str_buf sb1, sb2;
        str_buf_create(&sb1);
        str_buf_create(&sb2);
        json_out_extended = rec_to_json(&sb1, &doc);  // shall be '{"meta": {"key": {"type": "nokey", "value": null}, "rev": 0}, "doc": [{"k":{}}]}'
        json_out_compact = rec_to_json(&sb2, &doc);    // shall be '{"k":{}}'

        //printf("EXT:\t%s\n", json_out_extended);
        //printf("SRT:\t%s\n", json_out_compact);

        rec_drop(&doc);

        ASSERT_TRUE(strcmp(json_out_compact, json_in) == 0);
        ASSERT_TRUE(strcmp(json_out_extended, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [{\"k\": {}}]}") == 0);

        str_buf_drop(&sb1);
        str_buf_drop(&sb2);
}

TEST(CarbonTest, CarbonFromJsonObjectTrueProp)
{
        rec doc;
        
        const char *json_in;
        const char *json_out_compact, *json_out_extended;

        json_in = "{\"k\": true}";

        rec_from_json(&doc, json_in, KEY_NOKEY, NULL);

        str_buf sb1, sb2;
        str_buf_create(&sb1);
        str_buf_create(&sb2);
        json_out_extended = rec_to_json(&sb1, &doc);  // shall be '{"meta": {"key": {"type": "nokey", "value": null}, "rev": 0}, "doc": [{"k":true}]}'
        json_out_compact = rec_to_json(&sb2, &doc);    // shall be '{"k":true}'

        //printf("EXT:\t%s\n", json_out_extended);
        //printf("SRT:\t%s\n", json_out_compact);

        rec_drop(&doc);

        ASSERT_TRUE(strcmp(json_out_compact, json_in) == 0);
        ASSERT_TRUE(strcmp(json_out_extended, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [{\"k\": true}]}") == 0);

        str_buf_drop(&sb1);
        str_buf_drop(&sb2);
}

TEST(CarbonTest, CarbonFromJsonObjectFalseProp)
{
        rec doc;
        
        const char *json_in;
        const char *json_out_compact, *json_out_extended;

        json_in = "{\"k\": false}";

        rec_from_json(&doc, json_in, KEY_NOKEY, NULL);

        str_buf sb1, sb2;
        str_buf_create(&sb1);
        str_buf_create(&sb2);
        json_out_extended = rec_to_json(&sb1, &doc);  // shall be '{"meta": {"key": {"type": "nokey", "value": null}, "rev": 0}, "doc": [{"k":false}]}'
        json_out_compact = rec_to_json(&sb2, &doc);    // shall be '{"k":false}'

        //printf("EXT:\t%s\n", json_out_extended);
        //printf("SRT:\t%s\n", json_out_compact);

        rec_drop(&doc);

        ASSERT_TRUE(strcmp(json_out_compact, json_in) == 0);
        ASSERT_TRUE(strcmp(json_out_extended, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [{\"k\": false}]}") == 0);

        str_buf_drop(&sb1);
        str_buf_drop(&sb2);
}

TEST(CarbonTest, CarbonFromJsonObjectNullProp)
{
        rec doc;
        
        const char *json_in;
        const char *json_out_compact, *json_out_extended;

        json_in = "{\"k\": null}";

        rec_from_json(&doc, json_in, KEY_NOKEY, NULL);

        str_buf sb1, sb2;
        str_buf_create(&sb1);
        str_buf_create(&sb2);
        json_out_extended = rec_to_json(&sb1, &doc);  // shall be '{"meta": {"key": {"type": "nokey", "value": null}, "rev": 0}, "doc": [{"k":null}]}'
        json_out_compact = rec_to_json(&sb2, &doc);    // shall be '{"k":null}'

        //printf("EXT:\t%s\n", json_out_extended);
        //printf("SRT:\t%s\n", json_out_compact);

        rec_drop(&doc);

        ASSERT_TRUE(strcmp(json_out_compact, json_in) == 0);
        ASSERT_TRUE(strcmp(json_out_extended, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [{\"k\": null}]}") == 0);

        str_buf_drop(&sb1);
        str_buf_drop(&sb2);
}

TEST(CarbonTest, CarbonFromJsonObjectUnsignedProp)
{
        rec doc;
        
        const char *json_in;
        const char *json_out_compact, *json_out_extended;

        json_in = "{\"k\": 42}";

        rec_from_json(&doc, json_in, KEY_NOKEY, NULL);

        str_buf sb1, sb2;
        str_buf_create(&sb1);
        str_buf_create(&sb2);
        json_out_extended = rec_to_json(&sb1, &doc);  // shall be '{"meta": {"key": {"type": "nokey", "value": null}, "rev": 0}, "doc": [{"k":42}]}'
        json_out_compact = rec_to_json(&sb2, &doc);    // shall be '{"k":42}'

        //printf("EXT:\t%s\n", json_out_extended);
        //printf("SRT:\t%s\n", json_out_compact);

        rec_drop(&doc);

        ASSERT_TRUE(strcmp(json_out_compact, json_in) == 0);
        ASSERT_TRUE(strcmp(json_out_extended, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [{\"k\": 42}]}") == 0);

        str_buf_drop(&sb1);
        str_buf_drop(&sb2);
}

TEST(CarbonTest, CarbonFromJsonObjectSignedProp)
{
        rec doc;
        
        const char *json_in;
        const char *json_out_compact, *json_out_extended;

        json_in = "{\"k\": -42}";

        rec_from_json(&doc, json_in, KEY_NOKEY, NULL);

        str_buf sb1, sb2;
        str_buf_create(&sb1);
        str_buf_create(&sb2);
        json_out_extended = rec_to_json(&sb1, &doc);  // shall be '{"meta": {"key": {"type": "nokey", "value": null}, "rev": 0}, "doc": [{"k":-42}]}'
        json_out_compact = rec_to_json(&sb2, &doc);    // shall be '{"k":-42}'

        //printf("EXT:\t%s\n", json_out_extended);
        //printf("SRT:\t%s\n", json_out_compact);

        rec_drop(&doc);

        ASSERT_TRUE(strcmp(json_out_compact, json_in) == 0);
        ASSERT_TRUE(strcmp(json_out_extended, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [{\"k\": -42}]}") == 0);

        str_buf_drop(&sb1);
        str_buf_drop(&sb2);
}

TEST(CarbonTest, CarbonFromJsonObjectFloatProp)
{
        rec doc;
        
        const char *json_in;
        const char *json_out_compact, *json_out_extended;

        json_in = "{\"k\": -42.23}";

        rec_from_json(&doc, json_in, KEY_NOKEY, NULL);

        str_buf sb1, sb2;
        str_buf_create(&sb1);
        str_buf_create(&sb2);
        json_out_extended = rec_to_json(&sb1, &doc);  // shall be '{"meta": {"key": {"type": "nokey", "value": null}, "rev": 0}, "doc": [{"k":-42.23}]}'
        json_out_compact = rec_to_json(&sb2, &doc);    // shall be '{"k":-42.23}'

        //printf("EXT:\t%s\n", json_out_extended);
        //printf("SRT:\t%s\n", json_out_compact);

        rec_drop(&doc);

        ASSERT_TRUE(strcmp(json_out_compact, json_in) == 0);
        ASSERT_TRUE(strcmp(json_out_extended, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [{\"k\": -42.23}]}") == 0);

        str_buf_drop(&sb1);
        str_buf_drop(&sb2);
}

TEST(CarbonTest, CarbonFromJsonColumnNumber)
{
        rec doc;
        
        const char *json_in;
        const char *json_out_compact, *json_out_extended;

        json_in = "{\"x\": [1, 2, 3]}";

        rec_from_json(&doc, json_in, KEY_NOKEY, NULL);

        arr_it it;
        field_e field_type;
        rec_read_begin(&it, &doc);
        ASSERT_TRUE(arr_it_next(&it));
        arr_it_field_type(&field_type, &it);
        ASSERT_TRUE(field_type == FIELD_OBJECT_UNSORTED_MULTIMAP);
        obj_it *oit = item_get_object(&(it.item));
        ASSERT_TRUE(obj_it_next(oit));
        internal_obj_it_prop_type(&field_type, oit);
        ASSERT_TRUE(field_is_column_or_subtype(field_type));
        ASSERT_TRUE(field_type == FIELD_COLUMN_U8_UNSORTED_MULTISET);
        obj_it_drop(oit);
        rec_read_end(&it);

        str_buf sb1, sb2;
        str_buf_create(&sb1);
        str_buf_create(&sb2);
        json_out_extended = rec_to_json(&sb1, &doc);  // shall be '{"meta": {"key": {"type": "nokey", "value": null}, "rev": 0}, "doc": [{\"x\": [1, 2, 3]}}'
        json_out_compact = rec_to_json(&sb2, &doc);    // shall be '[1, 2, 3]'

        //printf("EXT:\t%s\n", json_out_extended);
        //printf("SRT:\t%s\n", json_out_compact);

        rec_drop(&doc);

        ASSERT_TRUE(strcmp(json_out_compact, json_in) == 0);
        ASSERT_TRUE(strcmp(json_out_extended, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [{\"x\": [1, 2, 3]}]}") == 0);

        str_buf_drop(&sb1);
        str_buf_drop(&sb2);
}

TEST(CarbonTest, CarbonFromJsonColumnNullableNumber)
{
        rec doc;
        
        const char *json_in;
        const char *json_out_compact, *json_out_extended;

        json_in = "{\"x\": [1, null, 3]}";

        rec_from_json(&doc, json_in, KEY_NOKEY, NULL);

        arr_it it;
        field_e field_type;
        rec_read_begin(&it, &doc);
        ASSERT_TRUE(arr_it_next(&it));
        arr_it_field_type(&field_type, &it);
        ASSERT_TRUE(field_type == FIELD_OBJECT_UNSORTED_MULTIMAP);
        obj_it *oit = item_get_object(&(it.item));
        ASSERT_TRUE(obj_it_next(oit));
        internal_obj_it_prop_type(&field_type, oit);
        ASSERT_TRUE(field_is_column_or_subtype(field_type));
        ASSERT_TRUE(field_type == FIELD_COLUMN_U8_UNSORTED_MULTISET);
        obj_it_drop(oit);
        rec_read_end(&it);

        str_buf sb1, sb2;
        str_buf_create(&sb1);
        str_buf_create(&sb2);
        json_out_extended = rec_to_json(&sb1, &doc);  // shall be '{"meta": {"key": {"type": "nokey", "value": null}, "rev": 0}, "doc": [{"x": [1, null, 3]}]}'
        json_out_compact = rec_to_json(&sb2, &doc);    // shall be '{"x": [1, null, 3]}'

        //printf("INS:\t%s\n", json_in);
        //printf("EXT:\t%s\n", json_out_extended);
        //printf("SRT:\t%s\n", json_out_compact);

        rec_drop(&doc);

        ASSERT_TRUE(strcmp(json_out_compact, json_in) == 0);
        ASSERT_TRUE(strcmp(json_out_extended, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [{\"x\": [1, null, 3]}]}") == 0);

        str_buf_drop(&sb1);
        str_buf_drop(&sb2);
}

TEST(CarbonTest, CarbonFromJsonNonColumn)
{
        rec doc;
        
        const char *json_in;
        const char *json_out_compact, *json_out_extended;

        json_in = "[1, null, 3, \"a\"]";

        rec_from_json(&doc, json_in, KEY_NOKEY, NULL);

        arr_it it;
        field_e field_type;
        rec_read_begin(&it, &doc);
        ASSERT_TRUE(arr_it_next(&it));
        arr_it_field_type(&field_type, &it);
        ASSERT_TRUE(field_is_number(field_type));
        rec_read_end(&it);

        str_buf sb1, sb2;
        str_buf_create(&sb1);
        str_buf_create(&sb2);
        json_out_extended = rec_to_json(&sb1, &doc);  // shall be '{"meta": {"key": {"type": "nokey", "value": null}, "rev": 0}, "doc": [1, null, 3, \"a\"]}'
        json_out_compact = rec_to_json(&sb2, &doc);    // shall be '[1, null, 3, \"a\"]'

        //printf("INS:\t%s\n", json_in);
        //printf("EXT:\t%s\n", json_out_extended);
        //printf("SRT:\t%s\n", json_out_compact);

        rec_drop(&doc);

        ASSERT_TRUE(strcmp(json_out_compact, json_in) == 0);
        ASSERT_TRUE(strcmp(json_out_extended, "{\"meta\": {\"key\": {\"type\": \"nokey\", \"value\": null}, \"commit\": null}, \"doc\": [1, null, 3, \"a\"]}") == 0);

        str_buf_drop(&sb1);
        str_buf_drop(&sb2);
}

TEST(CarbonTest, CarbonColumnOptimizeFix)
{
        rec_new context;
        rec doc;
        col_state state_out;

        insert *ins = rec_create_begin(&context, &doc, KEY_NOKEY, OPTIMIZE);
        insert *cins = insert_column_begin(&state_out, ins, COLUMN_U8, 4);
        insert_u8(cins, 3);
        insert_u8(cins, 4);
        insert_u8(cins, 5);
        insert_column_end(&state_out);
        rec_create_end(&context);

        str_buf sb1;
        str_buf_create(&sb1);
        const char *json = rec_to_json(&sb1, &doc);
        ASSERT_TRUE(strcmp(json, "[3, 4, 5]") == 0);
        rec_drop(&doc);
        str_buf_drop(&sb1);
}

TEST(CarbonTest, CarbonFromJsonExample)
{
        rec doc;
        
        const char *json_in;
        const char *json_out_compact;

        /* example json taken from 'https://json.org/example.html' */
        json_in = "{\"web-app\": {\"servlet\": [{\"servlet-name\": \"cofaxCDS\", \"servlet-class\": \"org.cofax.cds.CDSServlet\", \"init-param\": {\"configGlossary: installationAt\": \"Philadelphia, PA\", \"configGlossary: adminEmail\": \"ksm@pobox.com\", \"configGlossary: poweredBy\": \"Cofax\", \"configGlossary: poweredByIcon\": \"/images/cofax.gif\", \"configGlossary: staticPath\": \"/content/static\", \"templateProcessorClass\": \"org.cofax.WysiwygTemplate\", \"templateLoaderClass\": \"org.cofax.FilesTemplateLoader\", \"templatePath\": \"templates\", \"templateOverridePath\": \"\", \"defaultListTemplate\": \"listTemplate.htm\", \"defaultFileTemplate\": \"articleTemplate.htm\", \"useJSP\": false, \"jspListTemplate\": \"listTemplate.jsp\", \"jspFileTemplate\": \"articleTemplate.jsp\", \"cachePackageTagsTrack\": 200, \"cachePackageTagsStore\": 200, \"cachePackageTagsRefresh\": 60, \"cacheTemplatesTrack\": 100, \"cacheTemplatesStore\": 50, \"cacheTemplatesRefresh\": 15, \"cachePagesTrack\": 200, \"cachePagesStore\": 100, \"cachePagesRefresh\": 10, \"cachePagesDirtyRead\": 10, \"searchEngineListTemplate\": \"forSearchEnginesList.htm\", \"searchEngineFileTemplate\": \"forSearchEngines.htm\", \"searchEngineRobotsDb\": \"WEB-INF/robots.db\", \"useDataStore\": true, \"dataStoreClass\": \"org.cofax.SqlDataStore\", \"redirectionClass\": \"org.cofax.SqlRedirection\", \"dataStoreName\": \"cofax\", \"dataStoreDriver\": \"com.microsoft.jdbc.sqlserver.SQLServerDriver\", \"dataStoreUrl\": \"jdbc: microsoft: sqlserver: //LOCALHOST: 1433;DatabaseName=goon\", \"dataStoreUser\": \"sa\", \"dataStorePassword\": \"dataStoreTestQuery\", \"dataStoreTestQuery\": \"SET NOCOUNT ON;select test='test';\", \"dataStoreLogFile\": \"/usr/local/tomcat/logs/datastore.log\", \"dataStoreInitConns\": 10, \"dataStoreMaxConns\": 100, \"dataStoreConnUsageLimit\": 100, \"dataStoreLogLevel\": \"debug\", \"maxUrlLength\": 500}}, {\"servlet-name\": \"cofaxEmail\", \"servlet-class\": \"org.cofax.cds.EmailServlet\", \"init-param\": {\"mailHost\": \"mail1\", \"mailHostOverride\": \"mail2\"}}, {\"servlet-name\": \"cofaxAdmin\", \"servlet-class\": \"org.cofax.cds.AdminServlet\"}, {\"servlet-name\": \"fileServlet\", \"servlet-class\": \"org.cofax.cds.FileServlet\"}, {\"servlet-name\": \"cofaxTools\", \"servlet-class\": \"org.cofax.cms.CofaxToolsServlet\", \"init-param\": {\"templatePath\": \"toolstemplates/\", \"log\": 1, \"logLocation\": \"/usr/local/tomcat/logs/CofaxTools.log\", \"logMaxSize\": \"\", \"dataLog\": 1, \"dataLogLocation\": \"/usr/local/tomcat/logs/dataLog.log\", \"dataLogMaxSize\": \"\", \"removePageCache\": \"/content/admin/remove?cache=pages&id=\", \"removeTemplateCache\": \"/content/admin/remove?cache=templates&id=\", \"fileTransferFolder\": \"/usr/local/tomcat/webapps/content/fileTransferFolder\", \"lookInContext\": 1, \"adminGroupID\": 4, \"betaServer\": true}}], \"servlet-mapping\": {\"cofaxCDS\": \"/\", \"cofaxEmail\": \"/cofaxutil/aemail/*\", \"cofaxAdmin\": \"/admin/*\", \"fileServlet\": \"/static/*\", \"cofaxTools\": \"/tools/*\"}, \"taglib\": {\"taglib-uri\": \"cofax.tld\", \"taglib-location\": \"/WEB-INF/tlds/cofax.tld\"}}}";

        rec_from_json(&doc, json_in, KEY_NOKEY, NULL);

        u32 max = 10000;
        timestamp t1 = wallclock();
        for (u32 i = 0; i < max; i++) {
                rec d;
                rec_from_json(&d, json_in, KEY_NOKEY, NULL);
                rec_drop(&d);
        }
        timestamp t2 = wallclock();
        printf("%.2fmsec/opp, %.4f ops/sec\n", (t2-t1)/(float)max, 1.0f/((t2-t1)/(float)max/1000.0f));


        str_buf sb1;
        str_buf_create(&sb1);
        json_out_compact = rec_to_json(&sb1, &doc);    // shall be '[1, null, 3, \"a\"]'

        //printf("INS:\t%s\n", json_in);
        //printf("SRT:\t%s\n", json_out_compact);

        //rec_hexdump_print(stdout, &doc);
        //u64 carbon_len = 0;
        //rec_raw_data(&carbon_len, &doc);
        //printf("INS len: %zu\n", strlen(json_in));
        //printf("SRT len: %zu\n", carbon_len);
        //printf("%0.2f%% space saving\n", 100 * (1 - (carbon_len / (float) strlen(json_in))));

        rec_drop(&doc);

        ASSERT_TRUE(strcmp(json_out_compact, json_in) == 0);

        str_buf_drop(&sb1);
}

TEST(CarbonTest, CarbonFromJsonUnitArrayPrimitive)
{
        rec doc;

        const char *json_in;
        const char *json_out_compact;

        json_in = "{\"x\": [1]}";

        rec_from_json(&doc, json_in, KEY_NOKEY, NULL);

        str_buf sb1;
        str_buf_create(&sb1);
        json_out_compact = rec_to_json(&sb1, &doc);    // shall be '{"x":[1]}'

        //printf("INS:\t%s\n", json_in);
        //printf("SRT:\t%s\n", json_out_compact);

        rec_drop(&doc);

        ASSERT_TRUE(strcmp(json_out_compact, json_in) == 0);

        str_buf_drop(&sb1);
}

TEST(CarbonTest, CarbonFromJsonUnitArrayObject)
{
        rec doc;

        const char *json_in;
        const char *json_out_compact;

        json_in = "{\"x\": [{\"y\": 1}]}";

        rec_from_json(&doc, json_in, KEY_NOKEY, NULL);

        str_buf sb1;
        str_buf_create(&sb1);
        json_out_compact = rec_to_json(&sb1, &doc);    // shall be '{"x":[{"y":1}]}'

        //printf("INS:\t%s\n", json_in);
        //printf("SRT:\t%s\n", json_out_compact);

        rec_drop(&doc);

        ASSERT_TRUE(strcmp(json_out_compact, json_in) == 0);

        str_buf_drop(&sb1);
}

TEST(CarbonTest, CarbonFromJsonSimpleExample)
{
        rec doc;

        const char *json_in = "{\"k\": {\"x\": [1,2,3], \"y\": \"z\"}}";
        rec_from_json(&doc, json_in, KEY_NOKEY, NULL);
        //rec_hexdump_print(stdout, &doc);
        //rec_print(stdout, &doc);
        rec_drop(&doc);
}

TEST(CarbonTest, CarbonFromJsonFromExcerpt)
{
        rec doc;

        /* the working directory must be 'tests/carbon' */
        int fd = open("./assets/ms-academic-graph.json", O_RDONLY);
        ASSERT_NE(fd, -1);
        u64 json_in_len = lseek(fd, 0, SEEK_END);
        const char *json_in = (const char *) mmap(0, json_in_len, PROT_READ, MAP_PRIVATE, fd, 0);

        rec_from_json(&doc, json_in, KEY_NOKEY, NULL);

        u64 carbon_out_len = 0;
        rec_raw_data(&carbon_out_len, &doc);

        ASSERT_LT(carbon_out_len, json_in_len);
        //printf("%0.2f%% space saving\n", 100 * (1 - (carbon_out_len / (float) json_in_len)));

        str_buf sb1;
        str_buf_create(&sb1);
        const char *json_out = rec_to_json(&sb1, &doc);
        ASSERT_TRUE(strcmp(json_in, json_out) == 0);

        rec_drop(&doc);
        str_buf_drop(&sb1);
}

TEST(CarbonTest, CarbonResolveDotPathForObjects)
{
        rec doc;
        find find;
        field_e result_type;
        u64 number;

        const char *json_in = "{\"a\": 1, \"b\": {\"c\": [1,2,3], \"d\": [\"Hello\", \"World\"], \"e\": [4], \"f\": [\"!\"], \"the key\": \"x\"}}";
        rec_from_json(&doc, json_in, KEY_NOKEY, NULL);

        ASSERT_TRUE(find_begin(&find, "0", &doc));
        ASSERT_TRUE(find_has_result(&find));
        ASSERT_TRUE(find_result_type(&result_type, &find));
        ASSERT_EQ(result_type, FIELD_OBJECT_UNSORTED_MULTIMAP);
        ASSERT_TRUE(find_end(&find));

        ASSERT_TRUE(find_begin(&find, "1", &doc));
        ASSERT_FALSE(find_has_result(&find));
        ASSERT_TRUE(find_end(&find));

        ASSERT_TRUE(find_begin(&find, "0.a", &doc));
        ASSERT_TRUE(find_has_result(&find));
        ASSERT_TRUE(find_result_type(&result_type, &find));
        ASSERT_EQ(result_type, FIELD_NUMBER_U8);
        ASSERT_TRUE(find_end(&find));

        ASSERT_TRUE(find_begin(&find, "0.b", &doc));
        ASSERT_TRUE(find_has_result(&find));
        ASSERT_TRUE(find_result_type(&result_type, &find));
        ASSERT_EQ(result_type, FIELD_OBJECT_UNSORTED_MULTIMAP);
        ASSERT_TRUE(find_end(&find));

        ASSERT_TRUE(find_begin(&find, "0.c", &doc));
        ASSERT_FALSE(find_has_result(&find));
        ASSERT_TRUE(find_end(&find));

        ASSERT_TRUE(find_begin(&find, "0.b.c", &doc));
        ASSERT_TRUE(find_has_result(&find));
        ASSERT_TRUE(find_result_type(&result_type, &find));
        ASSERT_EQ(result_type, FIELD_COLUMN_U8_UNSORTED_MULTISET);
        ASSERT_TRUE(find_end(&find));

        ASSERT_TRUE(find_begin(&find, "0.b.c.0", &doc));
        ASSERT_TRUE(find_has_result(&find));
        ASSERT_TRUE(find_result_type(&result_type, &find));
        ASSERT_EQ(result_type, FIELD_NUMBER_U8);
        ASSERT_TRUE(find_result_unsigned(&number, &find));
        ASSERT_EQ(number, 1U);
        ASSERT_TRUE(find_end(&find));

        ASSERT_TRUE(find_begin(&find, "0.b.c.1", &doc));
        ASSERT_TRUE(find_has_result(&find));
        ASSERT_TRUE(find_result_type(&result_type, &find));
        ASSERT_EQ(result_type, FIELD_NUMBER_U8);
        ASSERT_TRUE(find_result_unsigned(&number, &find));
        ASSERT_EQ(number, 2U);
        ASSERT_TRUE(find_end(&find));

        ASSERT_TRUE(find_begin(&find, "0.b.c.2", &doc));
        ASSERT_TRUE(find_has_result(&find));
        ASSERT_TRUE(find_result_type(&result_type, &find));
        ASSERT_EQ(result_type, FIELD_NUMBER_U8);
        ASSERT_TRUE(find_result_unsigned(&number, &find));
        ASSERT_EQ(number, 3U);
        ASSERT_TRUE(find_end(&find));

        ASSERT_TRUE(find_begin(&find, "0.b.c.3", &doc));
        ASSERT_FALSE(find_has_result(&find));
        ASSERT_TRUE(find_end(&find));

        ASSERT_TRUE(find_begin(&find, "0.b.d", &doc));
        ASSERT_TRUE(find_has_result(&find));
        ASSERT_TRUE(find_result_type(&result_type, &find));
        ASSERT_EQ(result_type, FIELD_ARRAY_UNSORTED_MULTISET);
        ASSERT_TRUE(find_end(&find));

        ASSERT_TRUE(find_begin(&find, "0.b.d.0", &doc));
        ASSERT_TRUE(find_has_result(&find));
        ASSERT_TRUE(find_result_type(&result_type, &find));
        ASSERT_EQ(result_type, FIELD_STRING);
        ASSERT_TRUE(strncmp(find_result_string(&number, &find), "Hello", number) == 0);
        ASSERT_TRUE(find_end(&find));

        ASSERT_TRUE(find_begin(&find, "0.b.d.1", &doc));
        ASSERT_TRUE(find_has_result(&find));
        ASSERT_TRUE(find_result_type(&result_type, &find));
        ASSERT_EQ(result_type, FIELD_STRING);
        ASSERT_TRUE(strncmp(find_result_string(&number, &find), "World", number) == 0);
        ASSERT_TRUE(find_end(&find));

        ASSERT_TRUE(find_begin(&find, "0.b.d.2", &doc));
        ASSERT_FALSE(find_has_result(&find));
        ASSERT_TRUE(find_end(&find));

        ASSERT_TRUE(find_begin(&find, "0.b.e", &doc));
        ASSERT_TRUE(find_has_result(&find));
        ASSERT_TRUE(find_result_type(&result_type, &find));
        ASSERT_EQ(result_type, FIELD_COLUMN_U8_UNSORTED_MULTISET);
        ASSERT_TRUE(find_end(&find));

        ASSERT_TRUE(find_begin(&find, "0.b.e.0", &doc));
        ASSERT_TRUE(find_has_result(&find));
        ASSERT_TRUE(find_result_type(&result_type, &find));
        ASSERT_EQ(result_type, FIELD_NUMBER_U8);
        ASSERT_TRUE(find_result_unsigned(&number, &find));
        ASSERT_EQ(number, 4U);
        ASSERT_TRUE(find_end(&find));

        ASSERT_TRUE(find_begin(&find, "0.b.e.1", &doc));
        ASSERT_FALSE(find_has_result(&find));
        ASSERT_TRUE(find_end(&find));

        ASSERT_TRUE(find_begin(&find, "0.b.f", &doc));
        ASSERT_TRUE(find_has_result(&find));
        ASSERT_TRUE(find_result_type(&result_type, &find));
        ASSERT_EQ(result_type, FIELD_ARRAY_UNSORTED_MULTISET);
        ASSERT_TRUE(find_end(&find));

        ASSERT_TRUE(find_begin(&find, "0.b.f.0", &doc));
        ASSERT_TRUE(find_has_result(&find));
        ASSERT_TRUE(find_result_type(&result_type, &find));
        ASSERT_EQ(result_type, FIELD_STRING);

        ASSERT_TRUE(strncmp(find_result_string(&number, &find), "!", number) == 0);
        ASSERT_TRUE(find_end(&find));

        ASSERT_TRUE(find_begin(&find, "0.b.f.1", &doc));
        ASSERT_FALSE(find_has_result(&find));
        ASSERT_TRUE(find_end(&find));

        ASSERT_TRUE(find_begin(&find, "0.b.\"the key\"", &doc));
        ASSERT_TRUE(find_has_result(&find));
        ASSERT_TRUE(find_result_type(&result_type, &find));
        ASSERT_EQ(result_type, FIELD_STRING);
        ASSERT_TRUE(strncmp(find_result_string(&number, &find), "x", number) == 0);
        ASSERT_TRUE(find_end(&find));

        rec_drop(&doc);
}

TEST(CarbonTest, CarbonResolveDotPathForObjectsBench)
{
        rec doc;
        find find;

        const char *json_in = "{\"a\": 1, \"b\": {\"c\": [1,2,3], \"d\": [\"Hello\", \"World\"], \"e\": [4], \"f\": [\"!\"], \"the key\": \"x\"}, \"array\": [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255, 256, 257, 258, 259, 260, 261, 262, 263, 264, 265, 266, 267, 268, 269, 270, 271, 272, 273, 274, 275, 276, 277, 278, 279, 280, 281, 282, 283, 284, 285, 286, 287, 288, 289, 290, 291, 292, 293, 294, 295, 296, 297, 298, 299, 300, 301, 302, 303, 304, 305, 306, 307, 308, 309, 310, 311, 312, 313, 314, 315, 316, 317, 318, 319, 320, 321, 322, 323, 324, 325, 326, 327, 328, 329, 330, 331, 332, 333, 334, 335, 336, 337, 338, 339, 340, 341, 342, 343, 344, 345, 346, 347, 348, 349, 350, 351, 352, 353, 354, 355, 356, 357, 358, 359, 360, 361, 362, 363, 364, 365, 366, 367, 368, 369, 370, 371, 372, 373, 374, 375, 376, 377, 378, 379, 380, 381, 382, 383, 384, 385, 386, 387, 388, 389, 390, 391, 392, 393, 394, 395, 396, 397, 398, 399, 400, 401, 402, 403, 404, 405, 406, 407, 408, 409, 410, 411, 412, 413, 414, 415, 416, 417, 418, 419, 420, 421, 422, 423, 424, 425, 426, 427, 428, 429, 430, 431, 432, 433, 434, 435, 436, 437, 438, 439, 440, 441, 442, 443, 444, 445, 446, 447, 448, 449, 450, 451, 452, 453, 454, 455, 456, 457, 458, 459, 460, 461, 462, 463, 464, 465, 466, 467, 468, 469, 470, 471, 472, 473, 474, 475, 476, 477, 478, 479, 480, 481, 482, 483, 484, 485, 486, 487, 488, 489, 490, 491, 492, 493, 494, 495, 496, 497, 498, 499, 500, 501, 502, 503, 504, 505, 506, 507, 508, 509, 510, 511, 512, 513, 514, 515, 516, 517, 518, 519, 520, 521, 522, 523, 524, 525, 526, 527, 528, 529, 530, 531, 532, 533, 534, 535, 536, 537, 538, 539, 540, 541, 542, 543, 544, 545, 546, 547, 548, 549, 550, 551, 552, 553, 554, 555, 556, 557, 558, 559, 560, 561, 562, 563, 564, 565, 566, 567, 568, 569, 570, 571, 572, 573, 574, 575, 576, 577, 578, 579, 580, 581, 582, 583, 584, 585, 586, 587, 588, 589, 590, 591, 592, 593, 594, 595, 596, 597, 598, 599, 600, 601, 602, 603, 604, 605, 606, 607, 608, 609, 610, 611, 612, 613, 614, 615, 616, 617, 618, 619, 620, 621, 622, 623, 624, 625, 626, 627, 628, 629, 630, 631, 632, 633, 634, 635, 636, 637, 638, 639, 640, 641, 642, 643, 644, 645, 646, 647, 648, 649, 650, 651, 652, 653, 654, 655, 656, 657, 658, 659, 660, 661, 662, 663, 664, 665, 666, 667, 668, 669, 670, 671, 672, 673, 674, 675, 676, 677, 678, 679, 680, 681, 682, 683, 684, 685, 686, 687, 688, 689, 690, 691, 692, 693, 694, 695, 696, 697, 698, 699, 700, 701, 702, 703, 704, 705, 706, 707, 708, 709, 710, 711, 712, 713, 714, 715, 716, 717, 718, 719, 720, 721, 722, 723, 724, 725, 726, 727, 728, 729, 730, 731, 732, 733, 734, 735, 736, 737, 738, 739, 740, 741, 742, 743, 744, 745, 746, 747, 748, 749, 750, 751, 752, 753, 754, 755, 756, 757, 758, 759, 760, 761, 762, 763, 764, 765, 766, 767, 768, 769, 770, 771, 772, 773, 774, 775, 776, 777, 778, 779, 780, 781, 782, 783, 784, 785, 786, 787, 788, 789, 790, 791, 792, 793, 794, 795, 796, 797, 798, 799, 800, 801, 802, 803, 804, 805, 806, 807, 808, 809, 810, 811, 812, 813, 814, 815, 816, 817, 818, 819, 820, 821, 822, 823, 824, 825, 826, 827, 828, 829, 830, 831, 832, 833, 834, 835, 836, 837, 838, 839, 840, 841, 842, 843, 844, 845, 846, 847, 848, 849, 850, 851, 852, 853, 854, 855, 856, 857, 858, 859, 860, 861, 862, 863, 864, 865, 866, 867, 868, 869, 870, 871, 872, 873, 874, 875, 876, 877, 878, 879, 880, 881, 882, 883, 884, 885, 886, 887, 888, 889, 890, 891, 892, 893, 894, 895, 896, 897, 898, 899, 900, 901, 902, 903, 904, 905, 906, 907, 908, 909, 910, 911, 912, 913, 914, 915, 916, 917, 918, 919, 920, 921, 922, 923, 924, 925, 926, 927, 928, 929, 930, 931, 932, 933, 934, 935, 936, 937, 938, 939, 940, 941, 942, 943, 944, 945, 946, 947, 948, 949, 950, 951, 952, 953, 954, 955, 956, 957, 958, 959, 960, 961, 962, 963, 964, 965, 966, 967, 968, 969, 970, 971, 972, 973, 974, 975, 976, 977, 978, 979, 980, 981, 982, 983, 984, 985, 986, 987, 988, 989, 990, 991, 992, 993, 994, 995, 996, 997, 998, 999]}";
        rec_from_json(&doc, json_in, KEY_NOKEY, NULL);

        dot path1, path2, path3, path4, path5, path6, path7, path8, path9, path10, path11, path12,
                path13, path14, path15, path16, path17, path18, path19, path20, path21, path22,
                path23, path24, path25;

        ASSERT_TRUE(dot_from_string(&path1, "0"));
        ASSERT_TRUE(dot_from_string(&path2, "1"));
        ASSERT_TRUE(dot_from_string(&path3, "0.a"));
        ASSERT_TRUE(dot_from_string(&path4, "0.b"));
        ASSERT_TRUE(dot_from_string(&path5, "0.c"));
        ASSERT_TRUE(dot_from_string(&path6, "0.b.c"));
        ASSERT_TRUE(dot_from_string(&path7, "0.b.c.0"));
        ASSERT_TRUE(dot_from_string(&path8, "0.b.c.1"));
        ASSERT_TRUE(dot_from_string(&path9, "0.b.c.2"));
        ASSERT_TRUE(dot_from_string(&path10, "0.b.c.3"));
        ASSERT_TRUE(dot_from_string(&path11, "0.b.d"));
        ASSERT_TRUE(dot_from_string(&path12, "0.b.d.0"));
        ASSERT_TRUE(dot_from_string(&path13, "0.b.d.1"));
        ASSERT_TRUE(dot_from_string(&path14, "0.b.d.2"));
        ASSERT_TRUE(dot_from_string(&path15, "0.b.e"));
        ASSERT_TRUE(dot_from_string(&path16, "0.b.e.0"));
        ASSERT_TRUE(dot_from_string(&path17, "0.b.e.1"));
        ASSERT_TRUE(dot_from_string(&path18, "0.b.f"));
        ASSERT_TRUE(dot_from_string(&path19, "0.b.f.0"));
        ASSERT_TRUE(dot_from_string(&path20, "0.b.f.1"));
        ASSERT_TRUE(dot_from_string(&path21, "0.b.\"the key\""));

        ASSERT_TRUE(dot_from_string(&path22, "0.array.0"));
        ASSERT_TRUE(dot_from_string(&path23, "0.array.999"));
        ASSERT_TRUE(dot_from_string(&path24, "0.array.250"));
        ASSERT_TRUE(dot_from_string(&path25, "0.array.750"));

        printf("ops/sec\n");
        for (int j = 0; j < 1; j++) {
            u32 max = 1000;
            timestamp t1 = wallclock();
            for (u32 i = 0; i < max; i++) {
                ASSERT_TRUE(find_create(&find, &path1, &doc));
                ASSERT_TRUE(find_has_result(&find));
                ASSERT_TRUE(find_end(&find));

                ASSERT_TRUE(find_create(&find, &path2, &doc));
                ASSERT_FALSE(find_has_result(&find));
                ASSERT_TRUE(find_end(&find));

                ASSERT_TRUE(find_create(&find, &path3, &doc));
                ASSERT_TRUE(find_has_result(&find));
                ASSERT_TRUE(find_end(&find));

                ASSERT_TRUE(find_create(&find, &path4, &doc));
                ASSERT_TRUE(find_has_result(&find));
                ASSERT_TRUE(find_end(&find));

                ASSERT_TRUE(find_create(&find, &path5, &doc));
                ASSERT_FALSE(find_has_result(&find));
                ASSERT_TRUE(find_end(&find));

                ASSERT_TRUE(find_create(&find, &path6, &doc));
                ASSERT_TRUE(find_has_result(&find));
                ASSERT_TRUE(find_end(&find));

                ASSERT_TRUE(find_create(&find, &path7, &doc));
                ASSERT_TRUE(find_has_result(&find));
                ASSERT_TRUE(find_end(&find));

                ASSERT_TRUE(find_create(&find, &path8, &doc));
                ASSERT_TRUE(find_has_result(&find));
                ASSERT_TRUE(find_end(&find));

                ASSERT_TRUE(find_create(&find, &path9, &doc));
                ASSERT_TRUE(find_has_result(&find));
                ASSERT_TRUE(find_end(&find));

                ASSERT_TRUE(find_create(&find, &path10, &doc));
                ASSERT_FALSE(find_has_result(&find));
                ASSERT_TRUE(find_end(&find));

                ASSERT_TRUE(find_create(&find, &path11, &doc));
                ASSERT_TRUE(find_has_result(&find));
                ASSERT_TRUE(find_end(&find));

                ASSERT_TRUE(find_create(&find, &path12, &doc));
                ASSERT_TRUE(find_has_result(&find));
                ASSERT_TRUE(find_end(&find));

                ASSERT_TRUE(find_create(&find, &path13, &doc));
                ASSERT_TRUE(find_has_result(&find));
                ASSERT_TRUE(find_end(&find));

                ASSERT_TRUE(find_create(&find, &path14, &doc));
                ASSERT_FALSE(find_has_result(&find));
                ASSERT_TRUE(find_end(&find));

                ASSERT_TRUE(find_create(&find, &path15, &doc));
                ASSERT_TRUE(find_has_result(&find));
                ASSERT_TRUE(find_end(&find));

                ASSERT_TRUE(find_create(&find, &path16, &doc));
                ASSERT_TRUE(find_has_result(&find));
                ASSERT_TRUE(find_end(&find));

                ASSERT_TRUE(find_create(&find, &path17, &doc));
                ASSERT_FALSE(find_has_result(&find));
                ASSERT_TRUE(find_end(&find));

                ASSERT_TRUE(find_create(&find, &path18, &doc));
                ASSERT_TRUE(find_has_result(&find));
                ASSERT_TRUE(find_end(&find));

                ASSERT_TRUE(find_create(&find, &path19, &doc));
                ASSERT_TRUE(find_has_result(&find));
                ASSERT_TRUE(find_end(&find));

                ASSERT_TRUE(find_create(&find, &path20, &doc));
                ASSERT_FALSE(find_has_result(&find));
                ASSERT_TRUE(find_end(&find));

                ASSERT_TRUE(find_create(&find, &path21, &doc));
                ASSERT_TRUE(find_has_result(&find));
                ASSERT_TRUE(find_end(&find));

                ASSERT_TRUE(find_create(&find, &path22, &doc));
                ASSERT_TRUE(find_has_result(&find));
                ASSERT_TRUE(find_end(&find));

                ASSERT_TRUE(find_create(&find, &path23, &doc));
                ASSERT_TRUE(find_has_result(&find));
                ASSERT_TRUE(find_end(&find));

                ASSERT_TRUE(find_create(&find, &path24, &doc));
                ASSERT_TRUE(find_has_result(&find));
                ASSERT_TRUE(find_end(&find));

                ASSERT_TRUE(find_create(&find, &path25, &doc));
                ASSERT_TRUE(find_has_result(&find));
                ASSERT_TRUE(find_end(&find));
            }
            timestamp t2 = wallclock();
            printf("%.4f\n", 1.0f/((t2-t1)/(float)max/(26*1000.0f)));
            fflush(stdout);
        }


        dot_drop(&path1);
        dot_drop(&path2);
        dot_drop(&path3);
        dot_drop(&path4);
        dot_drop(&path5);
        dot_drop(&path6);
        dot_drop(&path7);
        dot_drop(&path8);
        dot_drop(&path9);
        dot_drop(&path10);
        dot_drop(&path11);
        dot_drop(&path12);
        dot_drop(&path13);
        dot_drop(&path14);
        dot_drop(&path15);
        dot_drop(&path16);
        dot_drop(&path17);
        dot_drop(&path18);
        dot_drop(&path19);
        dot_drop(&path20);
        dot_drop(&path21);

        dot_drop(&path22);
        dot_drop(&path23);
        dot_drop(&path24);
        dot_drop(&path25);

        rec_drop(&doc);
}

TEST(CarbonTest, CarbonFromJsonShortenedDotPath)
{
        rec doc;
        find find;
        field_e result_type;

        const char *json_in = "{\"x\": \"y\"}";
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

        json_in = "[{\"x\": \"y\"},{\"x\": [{\"z\": 42}]}]";
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
        ASSERT_TRUE(strcmp(result, "_nil") == 0);
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
        ASSERT_TRUE(strcmp(result, "_nil") == 0);
        find_end(&find);
        rec_drop(&doc);

        const char *complex = "{\n"
                              "   \"m\": {\n"
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
                              "            \"a\": null\n"
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

        ASSERT_TRUE(strcmp(result, "{\"n\": 8, \"o\": -8, \"p\": \"A\", \"q\": 32.40, \"r\": null, \"s\": true, \"t\": false, \"u\": [1, 2, 3, null], \"v\": [\"A\", \"B\", null], \"w\": \"Hello, World!\", \"x\": {\"a\": null}, \"y\": [], \"z\": {}}") == 0);
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
        ASSERT_TRUE(strcmp(result, "_nil") == 0);
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
        ASSERT_TRUE(strcmp(result, "{\"a\": null}") == 0);
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

        const char *json = "{\"x\": {\"y\": [{\"z\": 23}, {\"z\": null}]} }";

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
        const char *json = "[{\"col\": [true, null, false]}]";

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
