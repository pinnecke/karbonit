#include <gtest/gtest.h>

#include <jakson/jakson.h>

TEST(TestCarbonPatch, CreatePatch) {

        rec doc;
        u64 hash_original, hash_patch_1, hash_patch_2;
        arr_it it, *arr;
        obj_it *obj;
        field_type_e type;

        carbon_from_json(&doc, "{ \"x\": [1, \"y\", 3] }", CARBON_KEY_AUTOKEY, NULL);
        carbon_commit_hash(&hash_original, &doc);

        char *json_original = carbon_to_json_compact_dup(&doc);

        /* patching via patch iterators */
        {
                carbon_patch_begin(&it, &doc);
                arr_it_next(&it);
                {
                        arr_it_field_type(&type, &it);
                        EXPECT_TRUE(type == CARBON_FIELD_OBJECT_UNSORTED_MULTIMAP);
                        obj = carbon_item_get_object(&(it.item));
                        {
                                carbon_object_next(obj);
                                internal_carbon_object_prop_type(&type, obj);
                                EXPECT_EQ(type, CARBON_FIELD_ARRAY_UNSORTED_MULTISET);
                                arr = carbon_item_get_array(&obj->prop.value);
                                {
                                        arr_it_next(arr); /* { ...: [1,...] } */
                                        internal_arr_it_update_u8(arr, 42);
                                        arr_it_next(arr); /* { ...: [..., "y",...] } */
                                        arr_it_next(arr); /* { ...: [..., ..., 3] } */
                                        internal_arr_it_update_u8(arr, 23);
                                }
                        }
                }
                carbon_patch_end(&it);
        }

        char *json_patch_1 = carbon_to_json_compact_dup(&doc);
        carbon_commit_hash(&hash_patch_1, &doc);

        /* patching via patch find */
        {
                carbon_find find;
                carbon_patch_find_begin(&find, "x", &doc);
                arr_it *sub_it = carbon_find_result_array(&find);
                arr_it_next(sub_it); /* { ...: [42,...] } */
                internal_arr_it_update_u8(sub_it, 102);
                carbon_patch_find_end(&find);
        }

        char *json_patch_2 = carbon_to_json_compact_dup(&doc);
        carbon_commit_hash(&hash_patch_2, &doc);

        EXPECT_TRUE(strcmp(json_original, "{\"x\": [1, \"y\", 3]}") == 0);
        EXPECT_TRUE(strcmp(json_patch_1, "{\"x\": [42, \"y\", 23]}") == 0);
        EXPECT_TRUE(strcmp(json_patch_2, "{\"x\": [102, \"y\", 23]}") == 0);

        /* note especially that the commit hash has not changed; so a patch is not a new revision */
        EXPECT_EQ(hash_patch_1, hash_original);
        EXPECT_EQ(hash_patch_2, hash_original);

        carbon_drop(&doc);

        EXPECT_TRUE(true);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}