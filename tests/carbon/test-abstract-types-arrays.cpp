#include <gtest/gtest.h>
#include <printf.h>

#include <jakson/jakson.h>

TEST(TestAbstractTypes, CreateRecordDefaultJsonArray) {

        rec_new context;
        rec doc;

        carbon_create_begin(&context, &doc, CARBON_KEY_NOKEY, 0);
        carbon_create_end(&context);

        ASSERT_TRUE(carbon_is_multiset(&doc));
        ASSERT_FALSE(carbon_is_sorted(&doc));

        carbon_drop(&doc);
}

TEST(TestAbstractTypes, CreateRecordUnsortedMultiset) {

        rec_new context;
        rec doc;

        carbon_create_begin(&context, &doc, CARBON_KEY_NOKEY, UNSORTED_MULTISET);
        carbon_create_end(&context);

        ASSERT_TRUE(carbon_is_multiset(&doc));
        ASSERT_FALSE(carbon_is_sorted(&doc));

        carbon_drop(&doc);
}

TEST(TestAbstractTypes, CreateRecordUnsortedSet) {

        rec_new context;
        rec doc;

        carbon_create_begin(&context, &doc, CARBON_KEY_NOKEY, UNSORTED_SET);
        carbon_create_end(&context);

        ASSERT_FALSE(carbon_is_multiset(&doc));
        ASSERT_FALSE(carbon_is_sorted(&doc));

        carbon_drop(&doc);
}

TEST(TestAbstractTypes, CreateRecordSortedSet) {

        rec_new context;
        rec doc;

        carbon_create_begin(&context, &doc, CARBON_KEY_NOKEY, SORTED_SET);
        carbon_create_end(&context);

        ASSERT_FALSE(carbon_is_multiset(&doc));
        ASSERT_TRUE(carbon_is_sorted(&doc));

        carbon_drop(&doc);
}

TEST(TestAbstractTypes, CreateRecordSortedMultiset) {

        rec_new context;
        rec doc;

        carbon_create_begin(&context, &doc, CARBON_KEY_NOKEY, SORTED_MULTISET);
        carbon_create_end(&context);

        ASSERT_TRUE(carbon_is_multiset(&doc));
        ASSERT_TRUE(carbon_is_sorted(&doc));

        carbon_drop(&doc);
}

TEST(TestAbstractTypes, CreateRecordDeriveToDifferentTypes) {

        rec_new new_context;
        rec doc, doc2, doc3, doc4, doc5;

        carbon_create_begin(&new_context, &doc, CARBON_KEY_NOKEY, UNSORTED_MULTISET);
        carbon_create_end(&new_context);

        {
                carbon_update_list_type(&doc2, &doc, LIST_UNSORTED_MULTISET);
                ASSERT_TRUE(carbon_is_multiset(&doc2));
                ASSERT_FALSE(carbon_is_sorted(&doc2));
        }

        {
                carbon_update_list_type(&doc3, &doc2, LIST_UNSORTED_SET);
                ASSERT_FALSE(carbon_is_multiset(&doc3));
                ASSERT_FALSE(carbon_is_sorted(&doc3));
        }

        {
                carbon_update_list_type(&doc4, &doc3, LIST_SORTED_MULTISET);
                ASSERT_TRUE(carbon_is_multiset(&doc4));
                ASSERT_TRUE(carbon_is_sorted(&doc4));
        }

        {
                carbon_update_list_type(&doc5, &doc4, LIST_SORTED_SET);
                ASSERT_FALSE(carbon_is_multiset(&doc5));
                ASSERT_TRUE(carbon_is_sorted(&doc5));
        }

        carbon_drop(&doc);
        carbon_drop(&doc2);
        carbon_drop(&doc3);
        carbon_drop(&doc4);
        carbon_drop(&doc5);
}

TEST(TestAbstractTypes, ArraySetAbstractType) {

        rec_new context;
        insert *ins;
        rec doc, doc2;
        arr_state s1;
        arr_it it, *sub_it;
        field_e ft;
        rev rev_context;

        ins = carbon_create_begin(&context, &doc, CARBON_KEY_NOKEY, CARBON_OPTIMIZE);

        insert_array_begin(&s1, ins, 100);
        insert_array_end(&s1);

        insert_array_list_begin(&s1, ins, LIST_UNSORTED_MULTISET, 100);
        insert_array_list_end(&s1);

        insert_array_list_begin(&s1, ins, LIST_SORTED_MULTISET, 100);
        insert_array_list_end(&s1);

        insert_array_list_begin(&s1, ins, LIST_UNSORTED_SET, 100);
        insert_array_list_end(&s1);

        insert_array_list_begin(&s1, ins, LIST_SORTED_SET, 100);
        insert_array_list_end(&s1);

        carbon_create_end(&context);

        {
                carbon_read_begin(&it, &doc);

                arr_it_next(&it);
                arr_it_field_type(&ft, &it);
                ASSERT_TRUE(field_is_array_or_subtype(ft));
                sub_it = item_get_array(&(it.item));
                ASSERT_TRUE(arr_it_is_multiset(sub_it));
                ASSERT_FALSE(arr_it_is_sorted(sub_it));

                arr_it_next(&it);
                arr_it_field_type(&ft, &it);
                ASSERT_TRUE(field_is_array_or_subtype(ft));
                sub_it = item_get_array(&(it.item));
                ASSERT_TRUE(arr_it_is_multiset(sub_it));
                ASSERT_FALSE(arr_it_is_sorted(sub_it));

                arr_it_next(&it);
                arr_it_field_type(&ft, &it);
                ASSERT_TRUE(field_is_array_or_subtype(ft));
                sub_it = item_get_array(&(it.item));
                ASSERT_TRUE(arr_it_is_multiset(sub_it));
                ASSERT_TRUE(arr_it_is_sorted(sub_it));

                arr_it_next(&it);
                arr_it_field_type(&ft, &it);
                ASSERT_TRUE(field_is_array_or_subtype(ft));
                sub_it = item_get_array(&(it.item));
                ASSERT_FALSE(arr_it_is_multiset(sub_it));
                ASSERT_FALSE(arr_it_is_sorted(sub_it));

                arr_it_next(&it);
                arr_it_field_type(&ft, &it);
                ASSERT_TRUE(field_is_array_or_subtype(ft));
                sub_it = item_get_array(&(it.item));
                ASSERT_FALSE(arr_it_is_multiset(sub_it));
                ASSERT_TRUE(arr_it_is_sorted(sub_it));

                carbon_read_end(&it);
        }

        {
                revise_begin(&rev_context, &doc2, &doc);
                revise_iterator_open(&it, &rev_context);

                arr_it_next(&it);
                sub_it = item_get_array(&(it.item));
                arr_it_update_type(sub_it, LIST_SORTED_MULTISET);

                arr_it_next(&it);
                sub_it = item_get_array(&(it.item));
                arr_it_update_type(sub_it, LIST_UNSORTED_SET);

                arr_it_next(&it);
                sub_it = item_get_array(&(it.item));
                arr_it_update_type(sub_it, LIST_SORTED_SET);

                arr_it_next(&it);
                sub_it = item_get_array(&(it.item));
                arr_it_update_type(sub_it, LIST_UNSORTED_MULTISET);

                revise_iterator_close(&it);
                revise_end(&rev_context);
        }

        {
                carbon_read_begin(&it, &doc2);

                arr_it_next(&it);
                arr_it_field_type(&ft, &it);
                ASSERT_TRUE(field_is_array_or_subtype(ft));
                sub_it = item_get_array(&(it.item));
                ASSERT_TRUE(arr_it_is_multiset(sub_it));
                ASSERT_TRUE(arr_it_is_sorted(sub_it));

                arr_it_next(&it);
                arr_it_field_type(&ft, &it);
                ASSERT_TRUE(field_is_array_or_subtype(ft));
                sub_it = item_get_array(&(it.item));
                ASSERT_FALSE(arr_it_is_multiset(sub_it));
                ASSERT_FALSE(arr_it_is_sorted(sub_it));

                arr_it_next(&it);
                arr_it_field_type(&ft, &it);
                ASSERT_TRUE(field_is_array_or_subtype(ft));
                sub_it = item_get_array(&(it.item));
                ASSERT_FALSE(arr_it_is_multiset(sub_it));
                ASSERT_TRUE(arr_it_is_sorted(sub_it));

                arr_it_next(&it);
                arr_it_field_type(&ft, &it);
                ASSERT_TRUE(field_is_array_or_subtype(ft));
                sub_it = item_get_array(&(it.item));
                ASSERT_TRUE(arr_it_is_multiset(sub_it));
                ASSERT_FALSE(arr_it_is_sorted(sub_it));

                carbon_read_end(&it);
        }

        carbon_drop(&doc);
        carbon_drop(&doc2);
}

TEST(TestAbstractTypes, ArraySetNestedAbstractType) {

        rec doc, doc2, doc3, doc4, doc5;
        find find;
        field_e ft;
        rev revise;

        carbon_from_json(&doc, "{ x: [ { y: [1,\"b\",3] } ] }", CARBON_KEY_NOKEY, NULL);

        {
                revise_begin(&revise, &doc2, &doc);
                revise_find_begin(&find, "x.0.y", &revise);
                find_result_type(&ft, &find);
                ASSERT_TRUE(field_is_array_or_subtype(ft));
                arr_it * find_result = find_result_array(&find);
                ASSERT_TRUE(find_result != NULL);
                find_update_array_type(&find, LIST_SORTED_SET);
                revise_find_end(&find);
                revise_end(&revise);

                find_begin(&find, "x.0.y", &doc2);
                ASSERT_FALSE(find_array_is_multiset(&find));
                ASSERT_TRUE(find_array_is_sorted(&find));
                find_end(&find);
        }

        {
                revise_begin(&revise, &doc3, &doc2);
                revise_find_begin(&find, "x.0.y", &revise);
                find_result_type(&ft, &find);
                ASSERT_TRUE(field_is_array_or_subtype(ft));
                arr_it * find_result = find_result_array(&find);
                ASSERT_TRUE(find_result != NULL);
                find_update_array_type(&find, LIST_SORTED_MULTISET);
                revise_find_end(&find);
                revise_end(&revise);

                find_begin(&find, "x.0.y", &doc3);
                ASSERT_TRUE(find_array_is_multiset(&find));
                ASSERT_TRUE(find_array_is_sorted(&find));
                find_end(&find);
        }

        {
                revise_begin(&revise, &doc4, &doc3);
                revise_find_begin(&find, "x.0.y", &revise);
                find_result_type(&ft, &find);
                ASSERT_TRUE(field_is_array_or_subtype(ft));
                arr_it * find_result = find_result_array(&find);
                ASSERT_TRUE(find_result != NULL);
                find_update_array_type(&find, LIST_UNSORTED_MULTISET);
                revise_find_end(&find);
                revise_end(&revise);

                find_begin(&find, "x.0.y", &doc4);
                ASSERT_TRUE(find_array_is_multiset(&find));
                ASSERT_FALSE(find_array_is_sorted(&find));
                find_end(&find);
        }

        {
                revise_begin(&revise, &doc5, &doc4);
                revise_find_begin(&find, "x.0.y", &revise);
                find_result_type(&ft, &find);
                ASSERT_TRUE(field_is_array_or_subtype(ft));
                arr_it * find_result = find_result_array(&find);
                ASSERT_TRUE(find_result != NULL);
                find_update_array_type(&find, LIST_UNSORTED_SET);
                revise_find_end(&find);
                revise_end(&revise);

                find_begin(&find, "x.0.y", &doc5);
                ASSERT_FALSE(find_array_is_multiset(&find));
                ASSERT_FALSE(find_array_is_sorted(&find));
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