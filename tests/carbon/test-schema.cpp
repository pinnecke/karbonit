#include <gtest/gtest.h>

#include <fcntl.h>

#include <jakson/jakson.h>

TEST(SchemaTest, ValidateType) {
    carbon schemaFile;
    carbon fileToVal;
    carbon fileToVal2;
    carbon fileToVal3;
    carbon fileToVal4;
    carbon fileToVal5;
    carbon fileToVal6;
    const char *json_in;
    const char *json_in2;
    const char *json_in3;
    const char *json_in4;
    const char *json_in5;
    const char *json_in6;
    const char *json_in7;
    err err;

    json_in = "{\"type\": [\"string\", \"object\"]}";
    carbon_from_json(&schemaFile, json_in, CARBON_KEY_NOKEY, NULL, &err);

    json_in2 = "\"foo\"";
    carbon_from_json(&fileToVal, json_in2, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal)));

    json_in3 = "10";
    carbon_from_json(&fileToVal2, json_in3, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal2)));
    
    json_in4 = "{\"foo\":\"bar\"}";
    carbon_from_json(&fileToVal3, json_in4, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal3)));
    
    json_in5 = "{\"foo\":[\"bar\"]}";
    carbon_from_json(&fileToVal4, json_in5, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal4)));

    json_in6 = "{}";
    carbon_from_json(&fileToVal5, json_in6, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal5)));

    json_in7 = "[]";
    carbon_from_json(&fileToVal6, json_in7, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal6)));

}


TEST(SchemaTest, ValidateMinimum) {

    carbon schemaFile;
    carbon fileToVal;
    carbon fileToVal2;
    carbon fileToVal3;
    carbon fileToVal4;
    carbon fileToVal5;
    carbon fileToVal6;
    const char *json_in;
    const char *json_in2;
    const char *json_in3;
    const char *json_in4;
    const char *json_in5;
    const char *json_in6;
    const char *json_in7;
    err err;

    json_in = "{\"minimum\": 5.3}";
    carbon_from_json(&schemaFile, json_in, CARBON_KEY_NOKEY, NULL, &err);

    json_in2 = "5";
    carbon_from_json(&fileToVal, json_in2, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal)));

    json_in3 = "5.3";
    carbon_from_json(&fileToVal2, json_in3, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal2)));
    
    json_in4 = "46000000000000000000";
    carbon_from_json(&fileToVal3, json_in4, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal3)));
    
    json_in5 = "{}";
    carbon_from_json(&fileToVal4, json_in5, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal4)));

    json_in6 = "[]";
    carbon_from_json(&fileToVal5, json_in6, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal5)));

    json_in7 = "null";
    carbon_from_json(&fileToVal6, json_in7, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal6)));

}

TEST(SchemaTest, ValidateMaximum) {

    carbon schemaFile;
    carbon fileToVal;
    carbon fileToVal2;
    carbon fileToVal3;
    carbon fileToVal4;
    carbon fileToVal5;
    carbon fileToVal6;
    const char *json_in;
    const char *json_in2;
    const char *json_in3;
    const char *json_in4;
    const char *json_in5;
    const char *json_in6;
    const char *json_in7;
    err err;

    json_in = "{\"maximum\": 5.3}";
    carbon_from_json(&schemaFile, json_in, CARBON_KEY_NOKEY, NULL, &err);

    json_in2 = "5";
    carbon_from_json(&fileToVal, json_in2, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal)));

    json_in3 = "5.3";
    carbon_from_json(&fileToVal2, json_in3, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal2)));
    
    json_in4 = "6";
    carbon_from_json(&fileToVal3, json_in4, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal3)));
    
    json_in5 = "{}";
    carbon_from_json(&fileToVal4, json_in5, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal4)));

    json_in6 = "[]";
    carbon_from_json(&fileToVal5, json_in6, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal5)));

    json_in7 = "null";
    carbon_from_json(&fileToVal6, json_in7, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal6)));

}

TEST(SchemaTest, ValidateExclusiveMinimum) {

    carbon schemaFile;
    carbon fileToVal;
    carbon fileToVal2;
    carbon fileToVal3;
    carbon fileToVal4;
    carbon fileToVal5;
    carbon fileToVal6;
    const char *json_in;
    const char *json_in2;
    const char *json_in3;
    const char *json_in4;
    const char *json_in5;
    const char *json_in6;
    const char *json_in7;
    err err;

    json_in = "{\"exclusiveMinimum\": 5.3}";
    carbon_from_json(&schemaFile, json_in, CARBON_KEY_NOKEY, NULL, &err);

    json_in2 = "5";
    carbon_from_json(&fileToVal, json_in2, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal)));

    json_in3 = "5.3";
    carbon_from_json(&fileToVal2, json_in3, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal2)));
    
    json_in4 = "5.4";
    carbon_from_json(&fileToVal3, json_in4, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal3)));
    
    json_in5 = "{}";
    carbon_from_json(&fileToVal4, json_in5, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal4)));

    json_in6 = "[]";
    carbon_from_json(&fileToVal5, json_in6, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal5)));

    json_in7 = "null";
    carbon_from_json(&fileToVal6, json_in7, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal6)));

}

TEST(SchemaTest, ValidateExclusiveMaximum) {

    carbon schemaFile;
    carbon fileToVal;
    carbon fileToVal2;
    carbon fileToVal3;
    carbon fileToVal4;
    carbon fileToVal5;
    carbon fileToVal6;
    const char *json_in;
    const char *json_in2;
    const char *json_in3;
    const char *json_in4;
    const char *json_in5;
    const char *json_in6;
    const char *json_in7;
    err err;

    json_in = "{\"exclusiveMaximum\": 5.3}";
    carbon_from_json(&schemaFile, json_in, CARBON_KEY_NOKEY, NULL, &err);

    json_in2 = "5.2";
    carbon_from_json(&fileToVal, json_in2, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal)));

    json_in3 = "5.3";
    carbon_from_json(&fileToVal2, json_in3, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal2)));
    
    json_in4 = "5.4";
    carbon_from_json(&fileToVal3, json_in4, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal3)));
    
    json_in5 = "{}";
    carbon_from_json(&fileToVal4, json_in5, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal4)));

    json_in6 = "[]";
    carbon_from_json(&fileToVal5, json_in6, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal5)));

    json_in7 = "null";
    carbon_from_json(&fileToVal6, json_in7, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal6)));

}


TEST(SchemaTest, Validate) {

    carbon schemaFile;
    carbon fileToVal;
    carbon fileToVal2;
    carbon fileToVal3;
    carbon fileToVal4;
    carbon fileToVal5;
    carbon fileToVal6;
    const char *json_in;
    const char *json_in2;
    const char *json_in3;
    const char *json_in4;
    const char *json_in5;
    const char *json_in6;
    const char *json_in7;
    err err;

    json_in = "{\"exclusiveMaximum\": 5.3}";
    carbon_from_json(&schemaFile, json_in, CARBON_KEY_NOKEY, NULL, &err);

    json_in2 = "5.2";
    carbon_from_json(&fileToVal, json_in2, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal)));

    json_in3 = "5.3";
    carbon_from_json(&fileToVal2, json_in3, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal2)));
    
    json_in4 = "5.4";
    carbon_from_json(&fileToVal3, json_in4, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal3)));
    
    json_in5 = "{}";
    carbon_from_json(&fileToVal4, json_in5, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal4)));

    json_in6 = "[]";
    carbon_from_json(&fileToVal5, json_in6, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal5)));

    json_in7 = "null";
    carbon_from_json(&fileToVal6, json_in7, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal6)));

}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
