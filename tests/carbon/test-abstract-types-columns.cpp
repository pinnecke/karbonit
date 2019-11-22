#include <gtest/gtest.h>
#include <printf.h>

#include <jakson/jakson.h>

TEST(TestAbstractTypes, ColumnSetAbstractType) {

        rec_new context;
        carbon_insert *ins;
        rec doc, doc2;
        carbon_insert_column_state s1;
        arr_it it;
        col_it *col_it;
        field_type_e ft;
        rev rev_context;

        ins = carbon_create_begin(&context, &doc, CARBON_KEY_NOKEY, CARBON_OPTIMIZE);

        carbon_insert_column_begin(&s1, ins, COLUMN_I64, 100);
        carbon_insert_column_end(&s1);

        carbon_insert_column_list_begin(&s1, ins, LIST_UNSORTED_MULTISET, COLUMN_I64, 100);
        carbon_insert_column_list_end(&s1);

        carbon_insert_column_list_begin(&s1, ins, LIST_SORTED_MULTISET, COLUMN_I64, 100);
        carbon_insert_column_list_end(&s1);

        carbon_insert_column_list_begin(&s1, ins, LIST_UNSORTED_SET, COLUMN_I64, 100);
        carbon_insert_column_list_end(&s1);

        carbon_insert_column_list_begin(&s1, ins, LIST_SORTED_SET, COLUMN_I64, 100);
        carbon_insert_column_list_end(&s1);

        carbon_create_end(&context);

        {
                carbon_read_begin(&it, &doc);

                arr_it_next(&it);
                arr_it_field_type(&ft, &it);
                ASSERT_TRUE(field_type_is_column_or_subtype(ft));
                col_it = carbon_item_get_column(&(it.item));
                ASSERT_TRUE(col_it_is_multiset(col_it));
                ASSERT_FALSE(col_it_is_sorted(col_it));

                arr_it_next(&it);
                arr_it_field_type(&ft, &it);
                ASSERT_TRUE(field_type_is_column_or_subtype(ft));
                col_it = carbon_item_get_column(&(it.item));
                ASSERT_TRUE(col_it_is_multiset(col_it));
                ASSERT_FALSE(col_it_is_sorted(col_it));

                arr_it_next(&it);
                arr_it_field_type(&ft, &it);
                ASSERT_TRUE(field_type_is_column_or_subtype(ft));
                col_it = carbon_item_get_column(&(it.item));
                ASSERT_TRUE(col_it_is_multiset(col_it));
                ASSERT_TRUE(col_it_is_sorted(col_it));

                arr_it_next(&it);
                arr_it_field_type(&ft, &it);
                ASSERT_TRUE(field_type_is_column_or_subtype(ft));
                col_it = carbon_item_get_column(&(it.item));
                ASSERT_FALSE(col_it_is_multiset(col_it));
                ASSERT_FALSE(col_it_is_sorted(col_it));

                arr_it_next(&it);
                arr_it_field_type(&ft, &it);
                ASSERT_TRUE(field_type_is_column_or_subtype(ft));
                col_it = carbon_item_get_column(&(it.item));
                ASSERT_FALSE(col_it_is_multiset(col_it));
                ASSERT_TRUE(col_it_is_sorted(col_it));

                carbon_read_end(&it);
        }

        {
                struct col_it *column_it;

                carbon_revise_begin(&rev_context, &doc2, &doc);
                carbon_revise_iterator_open(&it, &rev_context);

                arr_it_next(&it);
                column_it = carbon_item_get_column(&(it.item));
                col_it_update_type(column_it, LIST_SORTED_MULTISET);

                arr_it_next(&it);
                column_it = carbon_item_get_column(&(it.item));
                col_it_update_type(column_it, LIST_UNSORTED_SET);

                arr_it_next(&it);
                column_it = carbon_item_get_column(&(it.item));
                col_it_update_type(column_it, LIST_SORTED_SET);

                arr_it_next(&it);
                column_it = carbon_item_get_column(&(it.item));
                col_it_update_type(column_it, LIST_UNSORTED_MULTISET);

                carbon_revise_iterator_close(&it);
                carbon_revise_end(&rev_context);
        }

        {
                carbon_read_begin(&it, &doc2);

                arr_it_next(&it);
                arr_it_field_type(&ft, &it);
                ASSERT_TRUE(field_type_is_column_or_subtype(ft));
                col_it = carbon_item_get_column(&(it.item));
                ASSERT_TRUE(col_it_is_multiset(col_it));
                ASSERT_TRUE(col_it_is_sorted(col_it));

                arr_it_next(&it);
                arr_it_field_type(&ft, &it);
                ASSERT_TRUE(field_type_is_column_or_subtype(ft));
                col_it = carbon_item_get_column(&(it.item));
                ASSERT_FALSE(col_it_is_multiset(col_it));
                ASSERT_FALSE(col_it_is_sorted(col_it));

                arr_it_next(&it);
                arr_it_field_type(&ft, &it);
                ASSERT_TRUE(field_type_is_column_or_subtype(ft));
                col_it = carbon_item_get_column(&(it.item));
                ASSERT_FALSE(col_it_is_multiset(col_it));
                ASSERT_TRUE(col_it_is_sorted(col_it));

                arr_it_next(&it);
                arr_it_field_type(&ft, &it);
                ASSERT_TRUE(field_type_is_column_or_subtype(ft));
                col_it = carbon_item_get_column(&(it.item));
                ASSERT_TRUE(col_it_is_multiset(col_it));
                ASSERT_FALSE(col_it_is_sorted(col_it));

                carbon_read_end(&it);
        }

        carbon_drop(&doc);
        carbon_drop(&doc2);
}


TEST(TestAbstractTypes, ColumnSetNestedAbstractType) {

        rec doc, doc2, doc3, doc4, doc5;
        carbon_find find;
        field_type_e ft;
        rev revise;

        carbon_from_json(&doc, "{ x: [ { y: [1, 2, 3] } ] }", CARBON_KEY_NOKEY, NULL);

        {
                carbon_revise_begin(&revise, &doc2, &doc);
                carbon_revise_find_begin(&find, "x.0.y", &revise);
                carbon_find_result_type(&ft, &find);
                ASSERT_TRUE(field_type_is_column_or_subtype(ft));
                col_it *find_result = carbon_find_result_column(&find);
                ASSERT_TRUE(find_result != NULL);
                carbon_find_update_column_type(&find, LIST_SORTED_SET);
                carbon_revise_find_end(&find);
                carbon_revise_end(&revise);

                carbon_find_begin(&find, "x.0.y", &doc2);
                ASSERT_FALSE(carbon_find_column_is_multiset(&find));
                ASSERT_TRUE(carbon_find_column_is_sorted(&find));
                carbon_find_end(&find);
        }

        {
                carbon_revise_begin(&revise, &doc3, &doc2);
                carbon_revise_find_begin(&find, "x.0.y", &revise);
                carbon_find_result_type(&ft, &find);
                ASSERT_TRUE(field_type_is_column_or_subtype(ft));
                col_it *find_result = carbon_find_result_column(&find);
                ASSERT_TRUE(find_result != NULL);
                carbon_find_update_column_type(&find, LIST_SORTED_MULTISET);
                carbon_revise_find_end(&find);
                carbon_revise_end(&revise);

                carbon_find_begin(&find, "x.0.y", &doc3);
                ASSERT_TRUE(carbon_find_column_is_multiset(&find));
                ASSERT_TRUE(carbon_find_column_is_sorted(&find));
                carbon_find_end(&find);
        }

        {
                carbon_revise_begin(&revise, &doc4, &doc3);
                carbon_revise_find_begin(&find, "x.0.y", &revise);
                carbon_find_result_type(&ft, &find);
                ASSERT_TRUE(field_type_is_column_or_subtype(ft));
                col_it * find_result = carbon_find_result_column(&find);
                ASSERT_TRUE(find_result != NULL);
                carbon_find_update_column_type(&find, LIST_UNSORTED_MULTISET);
                carbon_revise_find_end(&find);
                carbon_revise_end(&revise);

                carbon_find_begin(&find, "x.0.y", &doc4);
                ASSERT_TRUE(carbon_find_column_is_multiset(&find));
                ASSERT_FALSE(carbon_find_column_is_sorted(&find));
                carbon_find_end(&find);
        }

        {
                carbon_revise_begin(&revise, &doc5, &doc4);
                carbon_revise_find_begin(&find, "x.0.y", &revise);
                carbon_find_result_type(&ft, &find);
                ASSERT_TRUE(field_type_is_column_or_subtype(ft));
                col_it *find_result = carbon_find_result_column(&find);
                ASSERT_TRUE(find_result != NULL);
                carbon_find_update_column_type(&find, LIST_UNSORTED_SET);
                carbon_revise_find_end(&find);
                carbon_revise_end(&revise);

                carbon_find_begin(&find, "x.0.y", &doc5);
                ASSERT_FALSE(carbon_find_column_is_multiset(&find));
                ASSERT_FALSE(carbon_find_column_is_sorted(&find));
                carbon_find_end(&find);
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