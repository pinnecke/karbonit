#include <gtest/gtest.h>
#include <printf.h>

#include <jakson/jakson.h>

TEST(TestAbstractTypes, ColumnSetAbstractType) {

        rec_new context;
        insert *ins;
        rec doc, doc2;
        col_state s1;
        arr_it it;
        col_it *col_it;
        field_e ft;
        rev rev_context;

        ins = carbon_create_begin(&context, &doc, KEY_NOKEY, CARBON_OPTIMIZE);

        insert_column_begin(&s1, ins, COLUMN_I64, 100);
        insert_column_end(&s1);

        insert_column_list_begin(&s1, ins, LIST_UNSORTED_MULTISET, COLUMN_I64, 100);
        insert_column_list_end(&s1);

        insert_column_list_begin(&s1, ins, LIST_SORTED_MULTISET, COLUMN_I64, 100);
        insert_column_list_end(&s1);

        insert_column_list_begin(&s1, ins, LIST_UNSORTED_SET, COLUMN_I64, 100);
        insert_column_list_end(&s1);

        insert_column_list_begin(&s1, ins, LIST_SORTED_SET, COLUMN_I64, 100);
        insert_column_list_end(&s1);

        carbon_create_end(&context);

        {
                carbon_read_begin(&it, &doc);

                arr_it_next(&it);
                arr_it_field_type(&ft, &it);
                ASSERT_TRUE(field_is_column_or_subtype(ft));
                col_it = item_get_column(&(it.item));
                ASSERT_TRUE(col_it_is_multiset(col_it));
                ASSERT_FALSE(col_it_is_sorted(col_it));

                arr_it_next(&it);
                arr_it_field_type(&ft, &it);
                ASSERT_TRUE(field_is_column_or_subtype(ft));
                col_it = item_get_column(&(it.item));
                ASSERT_TRUE(col_it_is_multiset(col_it));
                ASSERT_FALSE(col_it_is_sorted(col_it));

                arr_it_next(&it);
                arr_it_field_type(&ft, &it);
                ASSERT_TRUE(field_is_column_or_subtype(ft));
                col_it = item_get_column(&(it.item));
                ASSERT_TRUE(col_it_is_multiset(col_it));
                ASSERT_TRUE(col_it_is_sorted(col_it));

                arr_it_next(&it);
                arr_it_field_type(&ft, &it);
                ASSERT_TRUE(field_is_column_or_subtype(ft));
                col_it = item_get_column(&(it.item));
                ASSERT_FALSE(col_it_is_multiset(col_it));
                ASSERT_FALSE(col_it_is_sorted(col_it));

                arr_it_next(&it);
                arr_it_field_type(&ft, &it);
                ASSERT_TRUE(field_is_column_or_subtype(ft));
                col_it = item_get_column(&(it.item));
                ASSERT_FALSE(col_it_is_multiset(col_it));
                ASSERT_TRUE(col_it_is_sorted(col_it));

                carbon_read_end(&it);
        }

        {
                struct col_it *column_it;

                revise_begin(&rev_context, &doc2, &doc);
                revise_iterator_open(&it, &rev_context);

                arr_it_next(&it);
                column_it = item_get_column(&(it.item));
                col_it_update_type(column_it, LIST_SORTED_MULTISET);

                arr_it_next(&it);
                column_it = item_get_column(&(it.item));
                col_it_update_type(column_it, LIST_UNSORTED_SET);

                arr_it_next(&it);
                column_it = item_get_column(&(it.item));
                col_it_update_type(column_it, LIST_SORTED_SET);

                arr_it_next(&it);
                column_it = item_get_column(&(it.item));
                col_it_update_type(column_it, LIST_UNSORTED_MULTISET);

                revise_iterator_close(&it);
                revise_end(&rev_context);
        }

        {
                carbon_read_begin(&it, &doc2);

                arr_it_next(&it);
                arr_it_field_type(&ft, &it);
                ASSERT_TRUE(field_is_column_or_subtype(ft));
                col_it = item_get_column(&(it.item));
                ASSERT_TRUE(col_it_is_multiset(col_it));
                ASSERT_TRUE(col_it_is_sorted(col_it));

                arr_it_next(&it);
                arr_it_field_type(&ft, &it);
                ASSERT_TRUE(field_is_column_or_subtype(ft));
                col_it = item_get_column(&(it.item));
                ASSERT_FALSE(col_it_is_multiset(col_it));
                ASSERT_FALSE(col_it_is_sorted(col_it));

                arr_it_next(&it);
                arr_it_field_type(&ft, &it);
                ASSERT_TRUE(field_is_column_or_subtype(ft));
                col_it = item_get_column(&(it.item));
                ASSERT_FALSE(col_it_is_multiset(col_it));
                ASSERT_TRUE(col_it_is_sorted(col_it));

                arr_it_next(&it);
                arr_it_field_type(&ft, &it);
                ASSERT_TRUE(field_is_column_or_subtype(ft));
                col_it = item_get_column(&(it.item));
                ASSERT_TRUE(col_it_is_multiset(col_it));
                ASSERT_FALSE(col_it_is_sorted(col_it));

                carbon_read_end(&it);
        }

        carbon_drop(&doc);
        carbon_drop(&doc2);
}


TEST(TestAbstractTypes, ColumnSetNestedAbstractType) {

        rec doc, doc2, doc3, doc4, doc5;
        find find;
        field_e ft;
        rev revise;

        carbon_from_json(&doc, "{ x: [ { y: [1, 2, 3] } ] }", KEY_NOKEY, NULL);

        {
                revise_begin(&revise, &doc2, &doc);
                revise_find_begin(&find, "x.0.y", &revise);
                find_result_type(&ft, &find);
                ASSERT_TRUE(field_is_column_or_subtype(ft));
                col_it *find_result = find_result_column(&find);
                ASSERT_TRUE(find_result != NULL);
                find_update_column_type(&find, LIST_SORTED_SET);
                revise_find_end(&find);
                revise_end(&revise);

                find_begin(&find, "x.0.y", &doc2);
                ASSERT_FALSE(find_column_is_multiset(&find));
                ASSERT_TRUE(find_column_is_sorted(&find));
                find_end(&find);
        }

        {
                revise_begin(&revise, &doc3, &doc2);
                revise_find_begin(&find, "x.0.y", &revise);
                find_result_type(&ft, &find);
                ASSERT_TRUE(field_is_column_or_subtype(ft));
                col_it *find_result = find_result_column(&find);
                ASSERT_TRUE(find_result != NULL);
                find_update_column_type(&find, LIST_SORTED_MULTISET);
                revise_find_end(&find);
                revise_end(&revise);

                find_begin(&find, "x.0.y", &doc3);
                ASSERT_TRUE(find_column_is_multiset(&find));
                ASSERT_TRUE(find_column_is_sorted(&find));
                find_end(&find);
        }

        {
                revise_begin(&revise, &doc4, &doc3);
                revise_find_begin(&find, "x.0.y", &revise);
                find_result_type(&ft, &find);
                ASSERT_TRUE(field_is_column_or_subtype(ft));
                col_it * find_result = find_result_column(&find);
                ASSERT_TRUE(find_result != NULL);
                find_update_column_type(&find, LIST_UNSORTED_MULTISET);
                revise_find_end(&find);
                revise_end(&revise);

                find_begin(&find, "x.0.y", &doc4);
                ASSERT_TRUE(find_column_is_multiset(&find));
                ASSERT_FALSE(find_column_is_sorted(&find));
                find_end(&find);
        }

        {
                revise_begin(&revise, &doc5, &doc4);
                revise_find_begin(&find, "x.0.y", &revise);
                find_result_type(&ft, &find);
                ASSERT_TRUE(field_is_column_or_subtype(ft));
                col_it *find_result = find_result_column(&find);
                ASSERT_TRUE(find_result != NULL);
                find_update_column_type(&find, LIST_UNSORTED_SET);
                revise_find_end(&find);
                revise_end(&revise);

                find_begin(&find, "x.0.y", &doc5);
                ASSERT_FALSE(find_column_is_multiset(&find));
                ASSERT_FALSE(find_column_is_sorted(&find));
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