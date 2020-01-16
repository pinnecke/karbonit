#include <gtest/gtest.h>

#include <karbonit/karbonit.h>

TEST(TestCarbonPatch, CreatePatch) {

        rec doc;
        u64 hash_original, hash_patch_1, hash_patch_2;
        arr_it it, arr;
        obj_it obj;
        field_e type;
        str_buf buffer1, buffer2, buffer3;

        rec_from_json(&doc, "{ \"x\":[1, \"y\", 3] }", KEY_AUTOKEY, NULL, OPTIMIZE);
        rec_commit_hash(&hash_original, &doc);

        str_buf_create(&buffer1);
        str_buf_create(&buffer2);
        str_buf_create(&buffer3);

        const char *json_original = rec_to_json(&buffer1, &doc);

        /* patching via patch iterators */
        {
                patch(&it, &doc);
                arr_it_next(&it);
                {
                        arr_it_field_type(&type, &it);
                        EXPECT_TRUE(type == FIELD_OBJECT_UNSORTED_MULTIMAP);
                        ITEM_GET_OBJECT(&obj, &(it.item))
                        {
                                obj_it_next(&obj);
                                internal_obj_it_prop_type(&type, &obj);
                                EXPECT_EQ(type, FIELD_ARRAY_UNSORTED_MULTISET);
                                ITEM_GET_ARRAY(&arr, &obj.prop.value);
                                {
                                        arr_it_next(&arr); /* { ...: [1,...] } */
                                        internal_arr_it_update_u8(&arr, 42);
                                        arr_it_next(&arr); /* { ...: [..., "y",...] } */
                                        arr_it_next(&arr); /* { ...: [..., ..., 3] } */
                                        internal_arr_it_update_u8(&arr, 23);
                                }
                        }
                }
        }

        const char *json_patch_1 = rec_to_json(&buffer2, &doc);
        rec_commit_hash(&hash_patch_1, &doc);

        /* patching via patch find */
        {
                find find;
                patch_find_begin(&find, "x", &doc);
                arr_it *sub_it = find_result_array(&find);
                arr_it_next(sub_it); /* { ...: [42,...] } */
                internal_arr_it_update_u8(sub_it, 102);
                patch_find_end(&find);
        }

        const char *json_patch_2 = rec_to_json(&buffer3, &doc);
        rec_commit_hash(&hash_patch_2, &doc);

        EXPECT_TRUE(strcmp(json_original, "{\"x\":[1, \"y\", 3]}") == 0);
        EXPECT_TRUE(strcmp(json_patch_1, "{\"x\":[42, \"y\", 23]}") == 0);
        EXPECT_TRUE(strcmp(json_patch_2, "{\"x\":[102, \"y\", 23]}") == 0);

        /* note especially that the commit hash has not changed; so a patch is not a new revision */
        EXPECT_EQ(hash_patch_1, hash_original);
        EXPECT_EQ(hash_patch_2, hash_original);

        rec_drop(&doc);

        str_buf_drop(&buffer1);
        str_buf_drop(&buffer2);
        str_buf_drop(&buffer3);

        EXPECT_TRUE(true);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}