#include <gtest/gtest.h>
#include <stdio.h>

#include <carbon.h>

TEST(FixMapTest, CreationAndDrop)
{
    hashtable map;
    err     err;
    bool             status;

    status = hashtable_create(&map, &err, sizeof(carbon_u32), sizeof(carbon_u64), 100);
    ASSERT_TRUE(status);
    status = hashtable_drop(&map);
    ASSERT_TRUE(status);
}

TEST(FixMapTest, MapAndGetWithoutRehash)
{
    hashtable map;
    err     err;
    bool             status;

    hashtable_create(&map, &err, sizeof(carbon_u32), sizeof(carbon_u64), 100);

    for (carbon_u32 key = 0; key < 10; key++) {
        carbon_u64 value = key << 2;
        status = hashtable_insert_or_update(&map, &key, &value, 1);
        ASSERT_TRUE(status);
    }

    for (carbon_u32 key = 0; key < 10; key++) {
        const void *value = hashtable_get_value(&map, &key);
        ASSERT_TRUE(value != NULL);
        ASSERT_TRUE(*(carbon_u64 *) value == key << 2);
    }

    hashtable_drop(&map);

}

TEST(FixMapTest, MapAndGetWitRehash)
{
    hashtable map;
    err     err;
    bool             status;

    hashtable_create(&map, &err, sizeof(carbon_u32), sizeof(carbon_u64), 10);

    for (carbon_u32 key = 0; key < 10000; key++) {
        carbon_u64 value = key << 2;
        status = hashtable_insert_or_update(&map, &key, &value, 1);
        ASSERT_TRUE(status);
    }

    for (carbon_u32 key = 0; key < 10000; key++) {
        const void *value = hashtable_get_value(&map, &key);
        ASSERT_TRUE(value != NULL);
        ASSERT_TRUE(*(carbon_u64 *) value == key << 2);
    }

    hashtable_drop(&map);
}

TEST(FixMapTest, DisplaceTest)
{
    hashtable map;
    err     err;
    bool             status;

    hashtable_create(&map, &err, sizeof(carbon_u32), sizeof(carbon_u64), 10700);

    for (carbon_u32 key = 0; key < 10000; key++) {
        carbon_u64 value = key << 2;
        status = hashtable_insert_or_update(&map, &key, &value, 1);
        ASSERT_TRUE(status);
    }

    for (carbon_u32 key = 0; key < 10000; key++) {
        const void *value = hashtable_get_value(&map, &key);
        ASSERT_TRUE(value != NULL);
        ASSERT_TRUE(*(carbon_u64 *) value == key << 2);
    }

    float dis;
    hashtable_avg_displace(&dis, &map);
    printf("Avg Displace: %f\n", dis);


    hashtable_drop(&map);
}



TEST(FixMapTest, MapAndGetNotContainedWithoutRehash)
{
    hashtable map;
    err     err;
    bool             status;

    hashtable_create(&map, &err, sizeof(carbon_u32), sizeof(carbon_u64), 100);

    for (carbon_u32 key = 0; key < 10; key++) {
        carbon_u64 value = key << 2;
        status = hashtable_insert_or_update(&map, &key, &value, 1);
        ASSERT_TRUE(status);
    }

    for (carbon_u32 key = 0; key < 10; key++) {
        carbon_u32 unknown_key = 10 + key;
        const void *value = hashtable_get_value(&map, &unknown_key);
        ASSERT_TRUE(value == NULL);
    }

    hashtable_drop(&map);
}

TEST(FixMapTest, MapAndGetNotContainedWitRehash)
{
    hashtable map;
    err     err;
    bool             status;

    hashtable_create(&map, &err, sizeof(carbon_u32), sizeof(carbon_u64), 10);

    for (carbon_u32 key = 0; key < 10000; key++) {
        carbon_u64 value = key << 2;
        status = hashtable_insert_or_update(&map, &key, &value, 1);
        ASSERT_TRUE(status);
    }

    for (carbon_u32 key = 0; key < 10000; key++) {
        carbon_u32 unknown_key = 10000 + key;
        const void *value = hashtable_get_value(&map, &unknown_key);
        ASSERT_TRUE(value == NULL);
    }

    hashtable_drop(&map);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}