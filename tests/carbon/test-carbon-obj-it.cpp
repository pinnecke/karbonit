#include <gtest/gtest.h>
#include <printf.h>

#include <jakson/jakson.h>

TEST(ObjectIteratorTests, ConstObjectLengthFunction)
{
        rec document;
        arr_it it;
        obj_it obj;
        u64 obj_len;
        offset_t pos_old, pos_new;
        item *item;

        rec_from_json(&document, "{\"a\":1,\"b\":2,\"c\":3,\"d\":4,\"e\":5,\"f\":6}", KEY_NOKEY, NULL);
        rec_read(&it, &document);
        ASSERT_TRUE(arr_it_has_next(&it));
        item = arr_it_next(&it);
        ASSERT_TRUE(ITEM_IS_OBJECT(item));
        ITEM_GET_OBJECT(&obj, item);

        pos_old = obj.file.pos;
        obj_len = obj_it_length(&obj);
        pos_new = obj.file.pos;
        ASSERT_EQ(pos_old, pos_new);
        ASSERT_TRUE(obj_len == 6);

        rec_drop(&document);
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}