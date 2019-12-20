#include <gtest/gtest.h>
#include <printf.h>

#include <jakson/jakson.h>

TEST(TestAbstractTypes, ColumnSetAbstractType) {

        rec_new context;
        insert *ins;
        rec doc, doc2;
        obj_state s1;
        arr_it it;
        obj_it obj_it;
        field_e ft;
        rev rev_context;

        ins = rec_create_begin(&context, &doc, KEY_NOKEY, OPTIMIZE);

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

        rec_create_end(&context);

        {
                rec_read(&it, &doc);

                arr_it_next(&it);
                arr_it_field_type(&ft, &it);
                ASSERT_TRUE(FIELD_IS_OBJECT_OR_SUBTYPE(ft));
                ITEM_GET_OBJECT(&obj_it, &(it.item));
                ASSERT_TRUE(obj_it_is_multimap(&obj_it));
                ASSERT_FALSE(obj_it_is_sorted(&obj_it));

                arr_it_next(&it);
                arr_it_field_type(&ft, &it);
                ASSERT_TRUE(FIELD_IS_OBJECT_OR_SUBTYPE(ft));
                ITEM_GET_OBJECT(&obj_it, &(it.item));
                ASSERT_TRUE(obj_it_is_multimap(&obj_it));
                ASSERT_FALSE(obj_it_is_sorted(&obj_it));

                arr_it_next(&it);
                arr_it_field_type(&ft, &it);
                ASSERT_TRUE(FIELD_IS_OBJECT_OR_SUBTYPE(ft));
                ITEM_GET_OBJECT(&obj_it, &(it.item));
                ASSERT_TRUE(obj_it_is_multimap(&obj_it));
                ASSERT_TRUE(obj_it_is_sorted(&obj_it));

                arr_it_next(&it);
                arr_it_field_type(&ft, &it);
                ASSERT_TRUE(FIELD_IS_OBJECT_OR_SUBTYPE(ft));
                ITEM_GET_OBJECT(&obj_it, &(it.item));
                ASSERT_FALSE(obj_it_is_multimap(&obj_it));
                ASSERT_FALSE(obj_it_is_sorted(&obj_it));

                arr_it_next(&it);
                arr_it_field_type(&ft, &it);
                ASSERT_TRUE(FIELD_IS_OBJECT_OR_SUBTYPE(ft));
                ITEM_GET_OBJECT(&obj_it, &(it.item));
                ASSERT_FALSE(obj_it_is_multimap(&obj_it));
                ASSERT_TRUE(obj_it_is_sorted(&obj_it));
        }

        {
                revise_begin(&rev_context, &doc2, &doc);
                revise_iterator_open(&it, &rev_context);

                arr_it_next(&it);
                ITEM_GET_OBJECT(&obj_it, &(it.item));
                obj_it_update_type(&obj_it, MAP_SORTED_MULTIMAP);

                arr_it_next(&it);
                ITEM_GET_OBJECT(&obj_it, &(it.item));
                obj_it_update_type(&obj_it, MAP_UNSORTED_MAP);

                arr_it_next(&it);
                ITEM_GET_OBJECT(&obj_it, &(it.item));
                obj_it_update_type(&obj_it, MAP_SORTED_MAP);

                arr_it_next(&it);
                ITEM_GET_OBJECT(&obj_it, &(it.item));
                obj_it_update_type(&obj_it, MAP_UNSORTED_MULTIMAP);

                revise_end(&rev_context);
        }

        {
                rec_read(&it, &doc2);

                arr_it_next(&it);
                arr_it_field_type(&ft, &it);
                ASSERT_TRUE(FIELD_IS_OBJECT_OR_SUBTYPE(ft));
                ITEM_GET_OBJECT(&obj_it, &(it.item));
                ASSERT_TRUE(obj_it_is_multimap(&obj_it));
                ASSERT_TRUE(obj_it_is_sorted(&obj_it));

                arr_it_next(&it);
                arr_it_field_type(&ft, &it);
                ASSERT_TRUE(FIELD_IS_OBJECT_OR_SUBTYPE(ft));
                ITEM_GET_OBJECT(&obj_it, &(it.item));
                ASSERT_FALSE(obj_it_is_multimap(&obj_it));
                ASSERT_FALSE(obj_it_is_sorted(&obj_it));

                arr_it_next(&it);
                arr_it_field_type(&ft, &it);
                ASSERT_TRUE(FIELD_IS_OBJECT_OR_SUBTYPE(ft));
                ITEM_GET_OBJECT(&obj_it, &(it.item));
                ASSERT_FALSE(obj_it_is_multimap(&obj_it));
                ASSERT_TRUE(obj_it_is_sorted(&obj_it));

                arr_it_next(&it);
                arr_it_field_type(&ft, &it);
                ASSERT_TRUE(FIELD_IS_OBJECT_OR_SUBTYPE(ft));
                ITEM_GET_OBJECT(&obj_it, &(it.item));
                ASSERT_TRUE(obj_it_is_multimap(&obj_it));
                ASSERT_FALSE(obj_it_is_sorted(&obj_it));
        }

        rec_drop(&doc);
        rec_drop(&doc2);
}


TEST(TestAbstractTypes, ObjectSetNestedAbstractType) {

        rec doc, doc2, doc3, doc4, doc5;
        find find;
        field_e ft;
        rev revise;

        rec_from_json(&doc, "{ x: [ { y: {\"a\": 1, \"b\": 2, \"c\": 3 } } ] }", KEY_NOKEY, NULL);

        {
                revise_begin(&revise, &doc2, &doc);
                revise_find_begin(&find, "x.0.y", &revise);
                find_result_type(&ft, &find);
                ASSERT_TRUE(FIELD_IS_OBJECT_OR_SUBTYPE(ft));
                obj_it *find_result = find_result_object(&find);
                ASSERT_TRUE(find_result != NULL);
                find_update_object_type(&find, MAP_SORTED_MAP);
                revise_end(&revise);

                find_from_string(&find, "x.0.y", &doc2);
                ASSERT_FALSE(find_object_is_multimap(&find));
                ASSERT_TRUE(find_object_is_sorted(&find));
        }

        {
                revise_begin(&revise, &doc3, &doc2);
                revise_find_begin(&find, "x.0.y", &revise);
                find_result_type(&ft, &find);
                ASSERT_TRUE(FIELD_IS_OBJECT_OR_SUBTYPE(ft));
                obj_it *find_result = find_result_object(&find);
                ASSERT_TRUE(find_result != NULL);
                find_update_object_type(&find, MAP_SORTED_MULTIMAP);
                revise_end(&revise);

                find_from_string(&find, "x.0.y", &doc3);
                ASSERT_TRUE(find_object_is_multimap(&find));
                ASSERT_TRUE(find_object_is_sorted(&find));
        }

        {
                revise_begin(&revise, &doc4, &doc3);
                revise_find_begin(&find, "x.0.y", &revise);
                find_result_type(&ft, &find);
                ASSERT_TRUE(FIELD_IS_OBJECT_OR_SUBTYPE(ft));
                obj_it *find_result = find_result_object(&find);
                ASSERT_TRUE(find_result != NULL);
                find_update_object_type(&find, MAP_UNSORTED_MULTIMAP);
                revise_end(&revise);

                find_from_string(&find, "x.0.y", &doc4);
                ASSERT_TRUE(find_object_is_multimap(&find));
                ASSERT_FALSE(find_object_is_sorted(&find));
        }

        {
                revise_begin(&revise, &doc5, &doc4);
                revise_find_begin(&find, "x.0.y", &revise);
                find_result_type(&ft, &find);
                ASSERT_TRUE(FIELD_IS_OBJECT_OR_SUBTYPE(ft));
                obj_it *find_result = find_result_object(&find);
                ASSERT_TRUE(find_result != NULL);
                find_update_object_type(&find, MAP_UNSORTED_MAP);
                revise_end(&revise);

                find_from_string(&find, "x.0.y", &doc5);
                ASSERT_FALSE(find_object_is_multimap(&find));
                ASSERT_FALSE(find_object_is_sorted(&find));
        }

        rec_drop(&doc);
        rec_drop(&doc2);
        rec_drop(&doc3);
        rec_drop(&doc4);
        rec_drop(&doc5);
}

int main(int argc, char **argv) {
        ::testing::InitGoogleTest(&argc, argv);
        return RUN_ALL_TESTS();
}