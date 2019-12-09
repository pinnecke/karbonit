#include <gtest/gtest.h>
#include <printf.h>

#include <jakson/jakson.h>




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

        rec_key_signed_value(&id_read, &rev_doc);
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
        rec_key_type(&type, &doc);
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
        rec_key_type(&type, &doc);
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

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}