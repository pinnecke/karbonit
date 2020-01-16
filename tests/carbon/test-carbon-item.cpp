#include <gtest/gtest.h>

#include <karbonit/karbonit.h>

TEST(TestCarbonItem, CreateId) {
        rec doc;
        arr_it array;
        item *item;

        rec_from_json(&doc, "[true, false, null, \"Hello World\", 42, -42, 23.53, [\"a\", 1, 2], {\"x\": \"y\"}]", KEY_NOKEY, NULL, OPTIMIZE);
        rec_read(&array, &doc);
        while ((item = arr_it_next(&array))) {
                u64 index = ITEM_GET_INDEX(item);
                switch (index) {
                case 0:
                        EXPECT_TRUE(ITEM_IS_BOOLEAN(item));
                        EXPECT_TRUE(ITEM_IS_TRUE(item));
                break;
                case 1:
                        EXPECT_TRUE(ITEM_IS_BOOLEAN(item));
                        EXPECT_TRUE(ITEM_IS_FALSE(item));
                break;
                case 2:
                        EXPECT_TRUE(ITEM_IS_NULL(item));
                break;
                case 3:
                        EXPECT_TRUE(ITEM_IS_STRING(item));
                        {
                                string_field value = ITEM_GET_STRING(item, NULL_STRING);
                                EXPECT_TRUE(value.len = strlen("Hello World"));
                                EXPECT_TRUE(strncmp(value.str, "Hello World", value.len) == 0);
                        }
                break;
                case 4:
                        EXPECT_TRUE(ITEM_IS_NUMBER(item));
                        EXPECT_TRUE(ITEM_IS_UNSIGNED(item));
                        {
                                i64 value = ITEM_GET_NUMBER_UNSIGNED(item, CARBON_NULL_UNSIGNED);
                                EXPECT_TRUE(value == 42);
                        }
                break;
                case 5:
                        EXPECT_TRUE(ITEM_IS_NUMBER(item));
                        EXPECT_TRUE(ITEM_IS_SIGNED(item));
                        {
                                u64 value = ITEM_GET_NUMBER_SIGNED(item, CARBON_NULL_SIGNED);
                                EXPECT_TRUE(value == (u64) -42);
                        }
                break;
                case 6:
                        EXPECT_TRUE(ITEM_IS_NUMBER(item));
                        EXPECT_TRUE(ITEM_IS_FLOAT(item));
                        {
                                float value = ITEM_GET_NUMBER_FLOAT(item, CARBON_NULL_FLOAT);
                                EXPECT_TRUE(value > 23.52 && value < 23.54);
                        }
                break;
                case 7:
                        EXPECT_TRUE(ITEM_IS_ARRAY(item));
                        {
                                arr_it value;
                                ITEM_GET_ARRAY(&value, item)
                        }
                break;
                case 8:
                        EXPECT_TRUE(ITEM_IS_OBJECT(item));
                break;
                default:
                        FAIL();
                }
        }
        rec_drop(&doc);
}

int main(int argc, char **argv) {
        ::testing::InitGoogleTest(&argc, argv);
        return RUN_ALL_TESTS();
}