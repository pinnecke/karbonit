#include <gtest/gtest.h>

#include <fcntl.h>

#include <jakson/jakson.h>

TEST(SchemaTest, ValidateTypeStringTrue) {
    carbon schemaFile;
    carbon fileToVal;
    const char *json_in;
    const char *json_in2;
    err err;

    json_in = "{\"type\": \"string\"}";
    carbon_from_json(&schemaFile, json_in, CARBON_KEY_NOKEY, NULL, &err);
    json_in2 = "{\"foo\"}";
    carbon_from_json(&fileToVal, json_in, CARBON_KEY_NOKEY, NULL, &err);

    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal)));

}
    

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
