#include <gtest/gtest.h>

#include <jakson/jakson.h>

TEST(TestCarbonItem, CreateId) {
        rec doc;
        arr_it array;
        item *item;

        carbon_from_json(&doc, "[true, false, null, \"Hello World\", 42, -42, 23.53, [\"a\", 1, 2], {\"x\": \"y\"}]", KEY_NOKEY, NULL);
        carbon_read_begin(&array, &doc);
        while ((item = arr_it_next(&array))) {
                u64 index = item_get_index(item);
                switch (index) {
                case 0:
                        EXPECT_TRUE(item_is_boolean(item));
                        EXPECT_TRUE(item_is_true(item));
                break;
                case 1:
                        EXPECT_TRUE(item_is_boolean(item));
                        EXPECT_TRUE(item_is_false(item));
                break;
                case 2:
                        EXPECT_TRUE(item_is_null(item));
                break;
                case 3:
                        EXPECT_TRUE(item_is_string(item));
                        {
                                string_field value = item_get_string(item, NULL_STRING);
                                EXPECT_TRUE(value.len = strlen("Hello World"));
                                EXPECT_TRUE(strncmp(value.str, "Hello World", value.len) == 0);
                        }
                break;
                case 4:
                        EXPECT_TRUE(item_is_number(item));
                        EXPECT_TRUE(item_is_unsigned(item));
                        {
                                i64 value = item_get_number_unsigned(item, CARBON_NULL_UNSIGNED);
                                EXPECT_TRUE(value == 42);
                        }
                break;
                case 5:
                        EXPECT_TRUE(item_is_number(item));
                        EXPECT_TRUE(item_is_signed(item));
                        {
                                u64 value = item_get_number_signed(item, CARBON_NULL_SIGNED);
                                EXPECT_TRUE(value == (u64) -42);
                        }
                break;
                case 6:
                        EXPECT_TRUE(item_is_number(item));
                        EXPECT_TRUE(item_is_float(item));
                        {
                                float value = item_get_number_float(item, CARBON_NULL_FLOAT);
                                EXPECT_TRUE(value > 23.52 && value < 23.54);
                        }
                break;
                case 7:
                        EXPECT_TRUE(item_is_array(item));
                        {
                                arr_it *value = item_get_array(item);
                                EXPECT_TRUE(value != NULL);
                        }
                break;
                case 8:
                        EXPECT_TRUE(item_is_object(item));
                break;
                default:
                        FAIL();
                }
        }
        carbon_read_end(&array);
        carbon_drop(&doc);
}

int main(int argc, char **argv) {
        ::testing::InitGoogleTest(&argc, argv);
        return RUN_ALL_TESTS();
}