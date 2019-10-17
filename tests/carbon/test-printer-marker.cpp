#include <gtest/gtest.h>

#include <jakson/jakson.h>

TEST(CarbonFormatterTest, MarkerBasicTest) {
    unique_id_t id;
    bool result = unique_id_create(&id);
    EXPECT_TRUE(result);
    EXPECT_NE(id, 0U);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}