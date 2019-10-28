#include <gtest/gtest.h>

#include <jakson/jakson.h>

TEST(TestCarbonItem, CreateId) {
        carbon doc;
        struct err err;
        carbon_array array;
        carbon_item *item;

        carbon_from_json(&doc, "[true, false, null, \"Hello World\", 42, -42, 23.53, [\"a\", 1, 2], {\"x\": \"y\"}]", CARBON_KEY_NOKEY, NULL, &err);
        carbon_read_begin(&array, &doc);
        while ((item = carbon_array_next(&array))) {
                u64 index = carbon_item_get_index(item);
                switch (index) {
                case 0:
                        EXPECT_TRUE(carbon_item_is_boolean(item));
                        EXPECT_TRUE(carbon_item_is_true(item));
                break;
                case 1:
                        EXPECT_TRUE(carbon_item_is_boolean(item));
                        EXPECT_TRUE(carbon_item_is_false(item));
                break;
                case 2:
                        EXPECT_TRUE(carbon_item_is_null(item));
                break;
                case 3:
                        EXPECT_TRUE(carbon_item_is_string(item));
                        {
                                carbon_string_field value = carbon_item_get_string(item, CARBON_NULL_STRING);
                                EXPECT_TRUE(value.length = strlen("Hello World"));
                                EXPECT_TRUE(strncmp(value.string, "Hello World", value.length) == 0);
                        }
                break;
                case 4:
                        EXPECT_TRUE(carbon_item_is_number(item));
                        EXPECT_TRUE(carbon_item_is_unsigned(item));
                        {
                                i64 value = carbon_item_get_number_unsigned(item, CARBON_NULL_UNSIGNED);
                                EXPECT_TRUE(value == 42);
                        }
                break;
                case 5:
                        EXPECT_TRUE(carbon_item_is_number(item));
                        EXPECT_TRUE(carbon_item_is_signed(item));
                        {
                                u64 value = carbon_item_get_number_signed(item, CARBON_NULL_SIGNED);
                                EXPECT_TRUE(value == (u64) -42);
                        }
                break;
                case 6:
                        EXPECT_TRUE(carbon_item_is_number(item));
                        EXPECT_TRUE(carbon_item_is_float(item));
                        {
                                float value = carbon_item_get_number_float(item, CARBON_NULL_FLOAT);
                                EXPECT_TRUE(value > 23.52 && value < 23.54);
                        }
                break;
                case 7:
                        EXPECT_TRUE(carbon_item_is_array(item));
                        {
                                carbon_array *value = carbon_item_get_array(item);
                                EXPECT_TRUE(value != NULL);
                        }
                break;
                case 8:
                        EXPECT_TRUE(carbon_item_is_object(item));
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