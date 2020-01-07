#include <gtest/gtest.h>
#include <printf.h>

#include <jakson/jakson.h>

TEST(ArrayIteratorTests, ConstArrayLengthFunction)
{
        rec document;
        arr_it it;
        u64 arr_len;
        offset_t pos_old, pos_new;

        rec_from_json(&document, "[1,2,3,4,5,6]", KEY_NOKEY, NULL);
        rec_read(&it, &document);

        pos_old = it.file.pos;
        arr_it_length(&arr_len, &it);
        pos_new = it.file.pos;
        ASSERT_EQ(pos_old, pos_new);

        rec_drop(&document);
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}