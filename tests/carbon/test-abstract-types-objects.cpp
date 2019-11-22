#include <gtest/gtest.h>
#include <printf.h>

#include <jakson/jakson.h>

TEST(TestAbstractTypes, ColumnSetAbstractType) {

        rec_new context;
        insert *ins;
        rec doc, doc2;
        obj_state s1;
        arr_it it;
        obj_it *obj_it;
        field_e ft;
        rev rev_context;

        ins = carbon_create_begin(&context, &doc, CARBON_KEY_NOKEY, CARBON_OPTIMIZE);

        insert_object_begin(&s1, ins, 100);
        insert_object_end(&s1);

        insert_object_map_begin(&s1, ins, MAP_UNSORTED_MULTIMAP, 100);
        insert_object_map_end(&s1);

        insert_object_map_begin(&s1, ins, MAP_SORTED_MULTIMAP, 100);
        insert_object_map_end(&s1);

        insert_object_map_begin(&s1, ins, MAP_UNSORTED_MAP, 100);
        insert_object_map_end(&s1);

        insert_object_map_begin(&s1, ins, MAP_SORTED_MAP, 100);
        insert_object_map_end(&s1);

        carbon_create_end(&context);

        {
                carbon_read_begin(&it, &doc);

                arr_it_next(&it);
                arr_it_field_type(&ft, &it);
                ASSERT_TRUE(field_is_object_or_subtype(ft));
                obj_it = item_get_object(&(it.item));
                ASSERT_TRUE(obj_it_is_multimap(obj_it));
                ASSERT_FALSE(obj_it_is_sorted(obj_it));

                arr_it_next(&it);
                arr_it_field_type(&ft, &it);
                ASSERT_TRUE(field_is_object_or_subtype(ft));
                obj_it = item_get_object(&(it.item));
                ASSERT_TRUE(obj_it_is_multimap(obj_it));
                ASSERT_FALSE(obj_it_is_sorted(obj_it));

                arr_it_next(&it);
                arr_it_field_type(&ft, &it);
                ASSERT_TRUE(field_is_object_or_subtype(ft));
                obj_it = item_get_object(&(it.item));
                ASSERT_TRUE(obj_it_is_multimap(obj_it));
                ASSERT_TRUE(obj_it_is_sorted(obj_it));

                arr_it_next(&it);
                arr_it_field_type(&ft, &it);
                ASSERT_TRUE(field_is_object_or_subtype(ft));
                obj_it = item_get_object(&(it.item));
                ASSERT_FALSE(obj_it_is_multimap(obj_it));
                ASSERT_FALSE(obj_it_is_sorted(obj_it));

                arr_it_next(&it);
                arr_it_field_type(&ft, &it);
                ASSERT_TRUE(field_is_object_or_subtype(ft));
                obj_it = item_get_object(&(it.item));
                ASSERT_FALSE(obj_it_is_multimap(obj_it));
                ASSERT_TRUE(obj_it_is_sorted(obj_it));

                carbon_read_end(&it);
        }

        {
                carbon_revise_begin(&rev_context, &doc2, &doc);
                carbon_revise_iterator_open(&it, &rev_context);

                arr_it_next(&it);
                obj_it = item_get_object(&(it.item));
                obj_it_update_type(obj_it, MAP_SORTED_MULTIMAP);

                arr_it_next(&it);
                obj_it = item_get_object(&(it.item));
                obj_it_update_type(obj_it, MAP_UNSORTED_MAP);

                arr_it_next(&it);
                obj_it = item_get_object(&(it.item));
                obj_it_update_type(obj_it, MAP_SORTED_MAP);

                arr_it_next(&it);
                obj_it = item_get_object(&(it.item));
                obj_it_update_type(obj_it, MAP_UNSORTED_MULTIMAP);

                carbon_revise_iterator_close(&it);
                carbon_revise_end(&rev_context);
        }

        {
                carbon_read_begin(&it, &doc2);

                arr_it_next(&it);
                arr_it_field_type(&ft, &it);
                ASSERT_TRUE(field_is_object_or_subtype(ft));
                obj_it = item_get_object(&(it.item));
                ASSERT_TRUE(obj_it_is_multimap(obj_it));
                ASSERT_TRUE(obj_it_is_sorted(obj_it));

                arr_it_next(&it);
                arr_it_field_type(&ft, &it);
                ASSERT_TRUE(field_is_object_or_subtype(ft));
                obj_it = item_get_object(&(it.item));
                ASSERT_FALSE(obj_it_is_multimap(obj_it));
                ASSERT_FALSE(obj_it_is_sorted(obj_it));

                arr_it_next(&it);
                arr_it_field_type(&ft, &it);
                ASSERT_TRUE(field_is_object_or_subtype(ft));
                obj_it = item_get_object(&(it.item));
                ASSERT_FALSE(obj_it_is_multimap(obj_it));
                ASSERT_TRUE(obj_it_is_sorted(obj_it));

                arr_it_next(&it);
                arr_it_field_type(&ft, &it);
                ASSERT_TRUE(field_is_object_or_subtype(ft));
                obj_it = item_get_object(&(it.item));
                ASSERT_TRUE(obj_it_is_multimap(obj_it));
                ASSERT_FALSE(obj_it_is_sorted(obj_it));

                carbon_read_end(&it);
        }

        carbon_drop(&doc);
        carbon_drop(&doc2);
}


TEST(TestAbstractTypes, ObjectSetNestedAbstractType) {

        rec doc, doc2, doc3, doc4, doc5;
        find find;
        field_e ft;
        rev revise;

        carbon_from_json(&doc, "{ x: [ { y: {\"a\": 1, \"b\": 2, \"c\": 3 } } ] }", CARBON_KEY_NOKEY, NULL);

        {
                carbon_revise_begin(&revise, &doc2, &doc);
                carbon_revise_find_begin(&find, "x.0.y", &revise);
                find_result_type(&ft, &find);
                ASSERT_TRUE(field_is_object_or_subtype(ft));
                obj_it *find_result = find_result_object(&find);
                ASSERT_TRUE(find_result != NULL);
                find_update_object_type(&find, MAP_SORTED_MAP);
                carbon_revise_find_end(&find);
                carbon_revise_end(&revise);

                find_begin(&find, "x.0.y", &doc2);
                ASSERT_FALSE(find_object_is_multimap(&find));
                ASSERT_TRUE(find_object_is_sorted(&find));
                find_end(&find);
        }

        {
                carbon_revise_begin(&revise, &doc3, &doc2);
                carbon_revise_find_begin(&find, "x.0.y", &revise);
                find_result_type(&ft, &find);
                ASSERT_TRUE(field_is_object_or_subtype(ft));
                obj_it *find_result = find_result_object(&find);
                ASSERT_TRUE(find_result != NULL);
                find_update_object_type(&find, MAP_SORTED_MULTIMAP);
                carbon_revise_find_end(&find);
                carbon_revise_end(&revise);

                find_begin(&find, "x.0.y", &doc3);
                ASSERT_TRUE(find_object_is_multimap(&find));
                ASSERT_TRUE(find_object_is_sorted(&find));
                find_end(&find);
        }

        {
                carbon_revise_begin(&revise, &doc4, &doc3);
                carbon_revise_find_begin(&find, "x.0.y", &revise);
                find_result_type(&ft, &find);
                ASSERT_TRUE(field_is_object_or_subtype(ft));
                obj_it *find_result = find_result_object(&find);
                ASSERT_TRUE(find_result != NULL);
                find_update_object_type(&find, MAP_UNSORTED_MULTIMAP);
                carbon_revise_find_end(&find);
                carbon_revise_end(&revise);

                find_begin(&find, "x.0.y", &doc4);
                ASSERT_TRUE(find_object_is_multimap(&find));
                ASSERT_FALSE(find_object_is_sorted(&find));
                find_end(&find);
        }

        {
                carbon_revise_begin(&revise, &doc5, &doc4);
                carbon_revise_find_begin(&find, "x.0.y", &revise);
                find_result_type(&ft, &find);
                ASSERT_TRUE(field_is_object_or_subtype(ft));
                obj_it *find_result = find_result_object(&find);
                ASSERT_TRUE(find_result != NULL);
                find_update_object_type(&find, MAP_UNSORTED_MAP);
                carbon_revise_find_end(&find);
                carbon_revise_end(&revise);

                find_begin(&find, "x.0.y", &doc5);
                ASSERT_FALSE(find_object_is_multimap(&find));
                ASSERT_FALSE(find_object_is_sorted(&find));
                find_end(&find);
        }

        carbon_drop(&doc);
        carbon_drop(&doc2);
        carbon_drop(&doc3);
        carbon_drop(&doc4);
        carbon_drop(&doc5);
}

int main(int argc, char **argv) {
        ::testing::InitGoogleTest(&argc, argv);
        return RUN_ALL_TESTS();
}