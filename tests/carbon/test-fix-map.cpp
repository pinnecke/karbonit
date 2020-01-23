#include <gtest/gtest.h>
#include <stdio.h>

#include <karbonit/karbonit.h>

TEST(FixMapTest, CreationAndDrop)
{
    hashtable map;
    bool             status;

    status = hashtable_create(&map, sizeof(u32), sizeof(u64), 100);
    ASSERT_TRUE(status);
    status = hashtable_drop(&map);
    ASSERT_TRUE(status);
}

TEST(FixMapTest, MapAndGetWithoutRehash)
{
    hashtable map;
    bool             status;

    hashtable_create(&map, sizeof(u32), sizeof(u64), 100);

    for (u32 key = 0; key < 10; key++) {
        u64 value = key << 2;
        status = hashtable_insert_or_update(&map, &key, &value, 1);
        ASSERT_TRUE(status);
    }

    for (u32 key = 0; key < 10; key++) {
        const void *value = hashtable_get_value(&map, &key);
        ASSERT_TRUE(value != NULL);
        ASSERT_TRUE(*(u64 *) value == key << 2);
    }

    hashtable_drop(&map);

}

TEST(FixMapTest, MapAndGetWitRehash)
{
    hashtable map;
    bool             status;

    hashtable_create(&map, sizeof(u32), sizeof(u64), 10);

    for (u32 key = 0; key < 10000; key++) {
        u64 value = key << 2;
        status = hashtable_insert_or_update(&map, &key, &value, 1);
        ASSERT_TRUE(status);
    }

    for (u32 key = 0; key < 10000; key++) {
        const void *value = hashtable_get_value(&map, &key);
        ASSERT_TRUE(value != NULL);
        ASSERT_TRUE(*(u64 *) value == key << 2);
    }

    hashtable_drop(&map);
}

TEST(FixMapTest, DisplaceTest)
{
    hashtable map;
    bool             status;

    hashtable_create(&map, sizeof(u32), sizeof(u64), 10700);

    for (u32 key = 0; key < 10000; key++) {
        u64 value = key << 2;
        status = hashtable_insert_or_update(&map, &key, &value, 1);
        ASSERT_TRUE(status);
    }

    for (u32 key = 0; key < 10000; key++) {
        const void *value = hashtable_get_value(&map, &key);
        ASSERT_TRUE(value != NULL);
        ASSERT_TRUE(*(u64 *) value == key << 2);
    }

    float dis;
    hashtable_avg_displace(&dis, &map);
    printf("Avg Displace: %f\n", dis);


    hashtable_drop(&map);
}



TEST(FixMapTest, MapAndGetNotContainedWithoutRehash)
{
    hashtable map;
    bool             status;

    hashtable_create(&map, sizeof(u32), sizeof(u64), 100);

    for (u32 key = 0; key < 10; key++) {
        u64 value = key << 2;
        status = hashtable_insert_or_update(&map, &key, &value, 1);
        ASSERT_TRUE(status);
    }

    for (u32 key = 0; key < 10; key++) {
        u32 unknown_key = 10 + key;
        const void *value = hashtable_get_value(&map, &unknown_key);
        ASSERT_TRUE(value == NULL);
    }

    hashtable_drop(&map);
}

TEST(FixMapTest, MapAndGetNotContainedWitRehash)
{
    hashtable map;
    bool             status;

    hashtable_create(&map, sizeof(u32), sizeof(u64), 10);

    for (u32 key = 0; key < 10000; key++) {
        u64 value = key << 2;
        status = hashtable_insert_or_update(&map, &key, &value, 1);
        ASSERT_TRUE(status);
    }

    for (u32 key = 0; key < 10000; key++) {
        u32 unknown_key = 10000 + key;
        const void *value = hashtable_get_value(&map, &unknown_key);
        ASSERT_TRUE(value == NULL);
    }

    hashtable_drop(&map);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}