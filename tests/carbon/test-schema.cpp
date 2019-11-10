#include <gtest/gtest.h>

#include <fcntl.h>

#include <jakson/jakson.h>

// array tests currently have to be nested, see issue https://github.com/jaksonlabs/jakson/issues/3
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

    json_in7 = "[[]]";
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
    carbon fileToVal7;
    const char *json_in;
    const char *json_in2;
    const char *json_in3;
    const char *json_in4;
    const char *json_in5;
    const char *json_in6;
    const char *json_in7;
    const char *json_in8;
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

    json_in6 = "[[]]";
    carbon_from_json(&fileToVal5, json_in6, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal5)));

    json_in7 = "null";
    carbon_from_json(&fileToVal6, json_in7, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal6)));

    json_in8 = "\"foo\"";
    carbon_from_json(&fileToVal7, json_in8, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal7)));
}

TEST(SchemaTest, ValidateMaximum) {

    carbon schemaFile;
    carbon fileToVal;
    carbon fileToVal2;
    carbon fileToVal3;
    carbon fileToVal4;
    carbon fileToVal5;
    carbon fileToVal6;
    carbon fileToVal7;
    const char *json_in;
    const char *json_in2;
    const char *json_in3;
    const char *json_in4;
    const char *json_in5;
    const char *json_in6;
    const char *json_in7;
    const char *json_in8;
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

    json_in6 = "[[]]";
    carbon_from_json(&fileToVal5, json_in6, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal5)));

    json_in7 = "null";
    carbon_from_json(&fileToVal6, json_in7, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal6)));

    json_in8 = "\"foo\"";
    carbon_from_json(&fileToVal7, json_in8, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal7)));
}

TEST(SchemaTest, ValidateExclusiveMinimum) {

    carbon schemaFile;
    carbon fileToVal;
    carbon fileToVal2;
    carbon fileToVal3;
    carbon fileToVal4;
    carbon fileToVal5;
    carbon fileToVal6;
    carbon fileToVal7;
    const char *json_in;
    const char *json_in2;
    const char *json_in3;
    const char *json_in4;
    const char *json_in5;
    const char *json_in6;
    const char *json_in7;
    const char *json_in8;
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

    json_in6 = "[[]]";
    carbon_from_json(&fileToVal5, json_in6, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal5)));

    json_in7 = "null";
    carbon_from_json(&fileToVal6, json_in7, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal6)));

    json_in8 = "\"foo\"";
    carbon_from_json(&fileToVal7, json_in8, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal7)));
}

TEST(SchemaTest, ValidateExclusiveMaximum) {

    carbon schemaFile;
    carbon fileToVal;
    carbon fileToVal2;
    carbon fileToVal3;
    carbon fileToVal4;
    carbon fileToVal5;
    carbon fileToVal6;
    carbon fileToVal7;
    const char *json_in;
    const char *json_in2;
    const char *json_in3;
    const char *json_in4;
    const char *json_in5;
    const char *json_in6;
    const char *json_in7;
    const char *json_in8;
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

    json_in6 = "[[]]";
    carbon_from_json(&fileToVal5, json_in6, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal5)));

    json_in7 = "null";
    carbon_from_json(&fileToVal6, json_in7, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal6)));

    json_in8 = "\"foo\"";
    carbon_from_json(&fileToVal7, json_in8, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal7)));
}


TEST(SchemaTest, ValidateMultipleOf) {

    carbon schemaFile;
    carbon fileToVal;
    carbon fileToVal2;
    carbon fileToVal3;
    carbon fileToVal4;
    carbon fileToVal5;
    carbon fileToVal6;
    carbon fileToVal7;
    carbon fileToVal8;
    const char *json_in;
    const char *json_in2;
    const char *json_in3;
    const char *json_in4;
    const char *json_in5;
    const char *json_in6;
    const char *json_in7;
    const char *json_in8;
    const char *json_in9;
    err err;

    json_in = "{\"multipleOf\": 6.2}";
    carbon_from_json(&schemaFile, json_in, CARBON_KEY_NOKEY, NULL, &err);

    json_in2 = "12.4";
    carbon_from_json(&fileToVal, json_in2, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal)));

    json_in3 = "12";
    carbon_from_json(&fileToVal2, json_in3, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal2)));
    
    json_in4 = "6";
    carbon_from_json(&fileToVal3, json_in4, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal3)));
    
    json_in5 = "{}";
    carbon_from_json(&fileToVal4, json_in5, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal4)));

    json_in6 = "[[]]";
    carbon_from_json(&fileToVal5, json_in6, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal5)));

    json_in7 = "null";
    carbon_from_json(&fileToVal6, json_in7, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal6)));

    json_in8 = "3.1";
    carbon_from_json(&fileToVal7, json_in8, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal7)));

    json_in9 = "\"foo\"";
    carbon_from_json(&fileToVal8, json_in9, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal8)));
}


TEST(SchemaTest, ValidateMinLength) {

    carbon schemaFile;
    carbon fileToVal;
    carbon fileToVal2;
    carbon fileToVal3;
    carbon fileToVal4;
    carbon fileToVal5;
    carbon fileToVal6;
    carbon fileToVal7;
    const char *json_in;
    const char *json_in2;
    const char *json_in3;
    const char *json_in4;
    const char *json_in5;
    const char *json_in6;
    const char *json_in7;
    const char *json_in8;
    err err;

    json_in = "{\"minLength\": 6}";
    carbon_from_json(&schemaFile, json_in, CARBON_KEY_NOKEY, NULL, &err);

    json_in2 = "\"foo\"";
    carbon_from_json(&fileToVal, json_in2, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal)));

    json_in3 = "\"foobar\"";
    carbon_from_json(&fileToVal2, json_in3, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal2)));
    
    json_in4 = "\"foobarz\"";
    carbon_from_json(&fileToVal3, json_in4, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal3)));
    
    json_in5 = "{}";
    carbon_from_json(&fileToVal4, json_in5, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal4)));

    json_in6 = "[[]]";
    carbon_from_json(&fileToVal5, json_in6, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal5)));

    json_in7 = "null";
    carbon_from_json(&fileToVal6, json_in7, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal6)));

    json_in8 = "3.1";
    carbon_from_json(&fileToVal7, json_in8, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal7)));
}


TEST(SchemaTest, ValidateMaxLength) {

    carbon schemaFile;
    carbon fileToVal;
    carbon fileToVal2;
    carbon fileToVal3;
    carbon fileToVal4;
    carbon fileToVal5;
    carbon fileToVal6;
    carbon fileToVal7;
    const char *json_in;
    const char *json_in2;
    const char *json_in3;
    const char *json_in4;
    const char *json_in5;
    const char *json_in6;
    const char *json_in7;
    const char *json_in8;
    err err;

    json_in = "{\"maxLength\": 6}";
    carbon_from_json(&schemaFile, json_in, CARBON_KEY_NOKEY, NULL, &err);

    json_in2 = "\"foo\"";
    carbon_from_json(&fileToVal, json_in2, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal)));

    json_in3 = "\"foobar\"";
    carbon_from_json(&fileToVal2, json_in3, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal2)));
    
    json_in4 = "\"foobarz\"";
    carbon_from_json(&fileToVal3, json_in4, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal3)));
    
    json_in5 = "{}";
    carbon_from_json(&fileToVal4, json_in5, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal4)));

    json_in6 = "[[]]";
    carbon_from_json(&fileToVal5, json_in6, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal5)));

    json_in7 = "null";
    carbon_from_json(&fileToVal6, json_in7, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal6)));

    json_in8 = "3.1";
    carbon_from_json(&fileToVal7, json_in8, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal7)));
}


TEST(SchemaTest, ValidatePattern) {

    carbon schemaFile;
    carbon fileToVal;
    carbon fileToVal2;
    carbon fileToVal3;
    carbon fileToVal4;
    carbon fileToVal5;
    carbon fileToVal6;
    carbon fileToVal7;
    carbon fileToVal8;
    carbon fileToVal9;
    const char *json_in;
    const char *json_in2;
    const char *json_in3;
    const char *json_in4;
    const char *json_in5;
    const char *json_in6;
    const char *json_in7;
    const char *json_in8;
    const char *json_in9;
    const char *json_in10;
    err err;

    json_in = "{\"pattern\": \"[abc]+\"}";
    carbon_from_json(&schemaFile, json_in, CARBON_KEY_NOKEY, NULL, &err);

    json_in2 = "\"a\"";
    carbon_from_json(&fileToVal, json_in2, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal)));

    json_in3 = "\"abcd\"";
    carbon_from_json(&fileToVal2, json_in3, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal2)));
    
    json_in4 = "\"cde\"";
    carbon_from_json(&fileToVal3, json_in4, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal3)));

    json_in5 = "{}";
    carbon_from_json(&fileToVal4, json_in5, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal4)));

    json_in6 = "[[]]";
    carbon_from_json(&fileToVal5, json_in6, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal5)));

    json_in7 = "null";
    carbon_from_json(&fileToVal6, json_in7, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal6)));

    json_in8 = "3.1";
    carbon_from_json(&fileToVal7, json_in8, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal7)));

    json_in9 = "\"\"";
    carbon_from_json(&fileToVal8, json_in9, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal8)));

    json_in10 = "\"def\"";
    carbon_from_json(&fileToVal9, json_in10, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal9)));
}


TEST(SchemaTest, ValidateFormatIPv4) {

    carbon schemaFile;
    carbon fileToVal;
    carbon fileToVal3;
    carbon fileToVal4;
    carbon fileToVal5;
    carbon fileToVal6;
    carbon fileToVal7;
    carbon fileToVal8;
    const char *json_in;
    const char *json_in2;
    const char *json_in4;
    const char *json_in5;
    const char *json_in6;
    const char *json_in7;
    const char *json_in8;
    const char *json_in9;
    err err;

    json_in = "{\"format\": \"ipv4\"}";
    carbon_from_json(&schemaFile, json_in, CARBON_KEY_NOKEY, NULL, &err);

    json_in2 = "\"192.168.0.1\"";
    carbon_from_json(&fileToVal, json_in2, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal)));

    json_in4 = "\"cde\"";
    carbon_from_json(&fileToVal3, json_in4, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal3)));

    json_in5 = "{}";
    carbon_from_json(&fileToVal4, json_in5, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal4)));

    json_in6 = "[[]]";
    carbon_from_json(&fileToVal5, json_in6, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal5)));

    json_in7 = "null";
    carbon_from_json(&fileToVal6, json_in7, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal6)));

    json_in8 = "3.1";
    carbon_from_json(&fileToVal7, json_in8, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal7)));

    json_in9 = "\"\"";
    carbon_from_json(&fileToVal8, json_in9, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal8)));

}


TEST(SchemaTest, ValidateFormatDate) {

    carbon schemaFile;
    carbon fileToVal;
    carbon fileToVal3;
    carbon fileToVal4;
    carbon fileToVal5;
    carbon fileToVal6;
    carbon fileToVal7;
    carbon fileToVal8;
    const char *json_in;
    const char *json_in2;
    const char *json_in4;
    const char *json_in5;
    const char *json_in6;
    const char *json_in7;
    const char *json_in8;
    const char *json_in9;
    err err;

    json_in = "{\"format\": \"date\"}";
    carbon_from_json(&schemaFile, json_in, CARBON_KEY_NOKEY, NULL, &err);

    json_in2 = "\"2050-12-31\"";
    carbon_from_json(&fileToVal, json_in2, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal)));

    json_in4 = "\"cde\"";
    carbon_from_json(&fileToVal3, json_in4, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal3)));

    json_in5 = "{}";
    carbon_from_json(&fileToVal4, json_in5, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal4)));

    json_in6 = "[[]]";
    carbon_from_json(&fileToVal5, json_in6, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal5)));

    json_in7 = "null";
    carbon_from_json(&fileToVal6, json_in7, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal6)));

    json_in8 = "3.1";
    carbon_from_json(&fileToVal7, json_in8, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal7)));

    json_in9 = "\"\"";
    carbon_from_json(&fileToVal8, json_in9, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal8)));

}


TEST(SchemaTest, ValidateFormatDateTime) {

    carbon schemaFile;
    carbon fileToVal;
    carbon fileToVal3;
    carbon fileToVal4;
    carbon fileToVal5;
    carbon fileToVal6;
    carbon fileToVal7;
    carbon fileToVal8;
    const char *json_in;
    const char *json_in2;
    const char *json_in4;
    const char *json_in5;
    const char *json_in6;
    const char *json_in7;
    const char *json_in8;
    const char *json_in9;
    err err;

    json_in = "{\"format\": \"date-time\"}";
    carbon_from_json(&schemaFile, json_in, CARBON_KEY_NOKEY, NULL, &err);

    json_in2 = "\"2050-12-31 12:30\"";
    carbon_from_json(&fileToVal, json_in2, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal)));

    json_in4 = "\"cde\"";
    carbon_from_json(&fileToVal3, json_in4, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal3)));

    json_in5 = "{}";
    carbon_from_json(&fileToVal4, json_in5, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal4)));

    json_in6 = "[[]]";
    carbon_from_json(&fileToVal5, json_in6, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal5)));

    json_in7 = "null";
    carbon_from_json(&fileToVal6, json_in7, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal6)));

    json_in8 = "3.1";
    carbon_from_json(&fileToVal7, json_in8, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal7)));

    json_in9 = "\"2050-12-31\"";
    carbon_from_json(&fileToVal8, json_in9, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal8)));

}


TEST(SchemaTest, ValidateFormatURI) {

    carbon schemaFile;
    carbon fileToVal;
    carbon fileToVal3;
    carbon fileToVal4;
    carbon fileToVal5;
    carbon fileToVal6;
    carbon fileToVal7;
    carbon fileToVal8;
    const char *json_in;
    const char *json_in2;
    const char *json_in4;
    const char *json_in5;
    const char *json_in6;
    const char *json_in7;
    const char *json_in8;
    const char *json_in9;
    err err;

    json_in = "{\"format\": \"uri\"}";
    carbon_from_json(&schemaFile, json_in, CARBON_KEY_NOKEY, NULL, &err);

    json_in2 = "\"mailto:John.Doe@example.com\"";
    carbon_from_json(&fileToVal, json_in2, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal)));

    json_in4 = "\"cde\"";
    carbon_from_json(&fileToVal3, json_in4, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal3)));

    json_in5 = "{}";
    carbon_from_json(&fileToVal4, json_in5, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal4)));

    json_in6 = "[[]]";
    carbon_from_json(&fileToVal5, json_in6, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal5)));

    json_in7 = "null";
    carbon_from_json(&fileToVal6, json_in7, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal6)));

    json_in8 = "3.1";
    carbon_from_json(&fileToVal7, json_in8, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal7)));

    json_in9 = "\"\"";
    carbon_from_json(&fileToVal8, json_in9, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal8)));

}


TEST(SchemaTest, ValidateFormatEmail) {

    carbon schemaFile;
    carbon fileToVal;
    carbon fileToVal3;
    carbon fileToVal4;
    carbon fileToVal5;
    carbon fileToVal6;
    carbon fileToVal7;
    carbon fileToVal8;
    const char *json_in;
    const char *json_in2;
    const char *json_in4;
    const char *json_in5;
    const char *json_in6;
    const char *json_in7;
    const char *json_in8;
    const char *json_in9;
    err err;

    json_in = "{\"format\": \"email\"}";
    carbon_from_json(&schemaFile, json_in, CARBON_KEY_NOKEY, NULL, &err);

    json_in2 = "\"foo@local.host\"";
    carbon_from_json(&fileToVal, json_in2, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal)));

    json_in4 = "\"cde@local\"";
    carbon_from_json(&fileToVal3, json_in4, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal3)));

    json_in5 = "{}";
    carbon_from_json(&fileToVal4, json_in5, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal4)));

    json_in6 = "[[]]";
    carbon_from_json(&fileToVal5, json_in6, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal5)));

    json_in7 = "null";
    carbon_from_json(&fileToVal6, json_in7, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal6)));

    json_in8 = "3.1";
    carbon_from_json(&fileToVal7, json_in8, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal7)));

    json_in9 = "\"\"";
    carbon_from_json(&fileToVal8, json_in9, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal8)));

}


TEST(SchemaTest, ValidateFormatHostname) {

    carbon schemaFile;
    carbon fileToVal;
    carbon fileToVal3;
    carbon fileToVal4;
    carbon fileToVal5;
    carbon fileToVal6;
    carbon fileToVal7;
    carbon fileToVal8;
    const char *json_in;
    const char *json_in2;
    const char *json_in4;
    const char *json_in5;
    const char *json_in6;
    const char *json_in7;
    const char *json_in8;
    const char *json_in9;
    err err;

    json_in = "{\"format\": \"hostname\"}";
    carbon_from_json(&schemaFile, json_in, CARBON_KEY_NOKEY, NULL, &err);

    json_in2 = "\"local.host\"";
    carbon_from_json(&fileToVal, json_in2, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal)));

    json_in4 = "\"cde@local\"";
    carbon_from_json(&fileToVal3, json_in4, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal3)));

    json_in5 = "{}";
    carbon_from_json(&fileToVal4, json_in5, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal4)));

    json_in6 = "[[]]";
    carbon_from_json(&fileToVal5, json_in6, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal5)));

    json_in7 = "null";
    carbon_from_json(&fileToVal6, json_in7, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal6)));

    json_in8 = "3.1";
    carbon_from_json(&fileToVal7, json_in8, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal7)));

    json_in9 = "\"\"";
    carbon_from_json(&fileToVal8, json_in9, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal8)));

}


TEST(SchemaTest, ValidateFormatIPv6) {

    carbon schemaFile;
    carbon fileToVal;
    carbon fileToVal3;
    carbon fileToVal4;
    carbon fileToVal5;
    carbon fileToVal6;
    carbon fileToVal7;
    carbon fileToVal8;
    const char *json_in;
    const char *json_in2;
    const char *json_in4;
    const char *json_in5;
    const char *json_in6;
    const char *json_in7;
    const char *json_in8;
    const char *json_in9;
    err err;

    json_in = "{\"format\": \"ipv6\"}";
    carbon_from_json(&schemaFile, json_in, CARBON_KEY_NOKEY, NULL, &err);

    json_in2 = "\"2001:0db8:0000:0000:0000:8a2e:0370:7334\"";
    carbon_from_json(&fileToVal, json_in2, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal)));

    json_in4 = "\"2001:db8::8a2e:370:7334\"";
    carbon_from_json(&fileToVal3, json_in4, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal3)));

    json_in5 = "{}";
    carbon_from_json(&fileToVal4, json_in5, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal4)));

    json_in6 = "[[]]";
    carbon_from_json(&fileToVal5, json_in6, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal5)));

    json_in7 = "null";
    carbon_from_json(&fileToVal6, json_in7, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal6)));

    json_in8 = "3.1";
    carbon_from_json(&fileToVal7, json_in8, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal7)));

    json_in9 = "\"192.168.0.1\"";
    carbon_from_json(&fileToVal8, json_in9, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal8)));

}


TEST(SchemaTest, ValidateFormatRegex) {

    carbon schemaFile;
    carbon fileToVal;
    carbon fileToVal3;
    carbon fileToVal4;
    carbon fileToVal5;
    carbon fileToVal6;
    carbon fileToVal7;
    carbon fileToVal8;
    const char *json_in;
    const char *json_in2;
    const char *json_in4;
    const char *json_in5;
    const char *json_in6;
    const char *json_in7;
    const char *json_in8;
    const char *json_in9;
    err err;

    json_in = "{\"format\": \"regex\"}";
    carbon_from_json(&schemaFile, json_in, CARBON_KEY_NOKEY, NULL, &err);

    json_in2 = "\"[abc]+\"";
    carbon_from_json(&fileToVal, json_in2, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal)));

    json_in4 = "\"[\"";
    carbon_from_json(&fileToVal3, json_in4, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal3)));

    json_in5 = "{}";
    carbon_from_json(&fileToVal4, json_in5, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal4)));

    json_in6 = "[[]]";
    carbon_from_json(&fileToVal5, json_in6, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal5)));

    json_in7 = "null";
    carbon_from_json(&fileToVal6, json_in7, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal6)));

    json_in8 = "3.1";
    carbon_from_json(&fileToVal7, json_in8, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal7)));

    json_in9 = "\"^(([a-zA-Z0-9]|[a-zA-Z0-9][a-zA-Z0-9\\-]*[a-zA-Z0-9])\\.)*([A-Za-z0-9]|[A-Za-z0-9][A-Za-z0-9\\-]*[A-Za-z0-9])$\"";
    carbon_from_json(&fileToVal8, json_in9, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal8)));

}


TEST(SchemaTest, ValidateMinItems) {

    carbon schemaFile;
    carbon fileToVal;
    carbon fileToVal3;
    carbon fileToVal4;
    carbon fileToVal5;
    carbon fileToVal6;
    carbon fileToVal7;
    carbon fileToVal8;
    const char *json_in;
    const char *json_in2;
    const char *json_in4;
    const char *json_in5;
    const char *json_in6;
    const char *json_in7;
    const char *json_in8;
    const char *json_in9;
    err err;

    json_in = "{\"minItems\": 5}";
    carbon_from_json(&schemaFile, json_in, CARBON_KEY_NOKEY, NULL, &err);

    json_in2 = "[[1, 2, 3, 4]]";
    carbon_from_json(&fileToVal, json_in2, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal)));

    json_in4 = "[[1, 2, 3, 4, 5, 6]]";
    carbon_from_json(&fileToVal3, json_in4, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal3)));

    json_in5 = "{}";
    carbon_from_json(&fileToVal4, json_in5, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal4)));

    json_in6 = "[[1, 2, 3, 4, 5]]";
    carbon_from_json(&fileToVal5, json_in6, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal5)));

    json_in7 = "null";
    carbon_from_json(&fileToVal6, json_in7, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal6)));

    json_in8 = "3.1";
    carbon_from_json(&fileToVal7, json_in8, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal7)));

    json_in9 = "\"foo\"";
    carbon_from_json(&fileToVal8, json_in9, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal8)));

}


TEST(SchemaTest, ValidateMaxItems) {

    carbon schemaFile;
    carbon fileToVal;
    carbon fileToVal3;
    carbon fileToVal4;
    carbon fileToVal5;
    carbon fileToVal6;
    carbon fileToVal7;
    carbon fileToVal8;
    const char *json_in;
    const char *json_in2;
    const char *json_in4;
    const char *json_in5;
    const char *json_in6;
    const char *json_in7;
    const char *json_in8;
    const char *json_in9;
    err err;

    json_in = "{\"maxItems\": 5}";
    carbon_from_json(&schemaFile, json_in, CARBON_KEY_NOKEY, NULL, &err);

    json_in2 = "[[1, 2, 3, 4, 5]]";
    carbon_from_json(&fileToVal, json_in2, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal)));

    json_in4 = "[[1, 2, 3, 4, 5, 6]]";
    carbon_from_json(&fileToVal3, json_in4, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal3)));

    json_in5 = "{}";
    carbon_from_json(&fileToVal4, json_in5, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal4)));

    json_in6 = "[[]]";
    carbon_from_json(&fileToVal5, json_in6, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal5)));

    json_in7 = "null";
    carbon_from_json(&fileToVal6, json_in7, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal6)));

    json_in8 = "3.1";
    carbon_from_json(&fileToVal7, json_in8, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal7)));

    json_in9 = "\"foo\"";
    carbon_from_json(&fileToVal8, json_in9, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal8)));

}


TEST(SchemaTest, ValidateItemsObject) {

    carbon schemaFile;
    carbon fileToVal;
    carbon fileToVal3;
    carbon fileToVal4;
    carbon fileToVal5;
    carbon fileToVal6;
    carbon fileToVal7;
    carbon fileToVal8;
    const char *json_in;
    const char *json_in2;
    const char *json_in4;
    const char *json_in5;
    const char *json_in6;
    const char *json_in7;
    const char *json_in8;
    const char *json_in9;
    err err;

    json_in = "{\"items\": { \"type\": [\"string\"]} }";
    carbon_from_json(&schemaFile, json_in, CARBON_KEY_NOKEY, NULL, &err);

    json_in2 = "[[\"foo\", \"bar\"]]";
    carbon_from_json(&fileToVal, json_in2, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal)));

    json_in4 = "[[\"foo\", 6]]";
    carbon_from_json(&fileToVal3, json_in4, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal3)));

    json_in5 = "{}";
    carbon_from_json(&fileToVal4, json_in5, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal4)));

    json_in6 = "[[]]";
    carbon_from_json(&fileToVal5, json_in6, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal5)));

    json_in7 = "null";
    carbon_from_json(&fileToVal6, json_in7, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal6)));

    json_in8 = "3.1";
    carbon_from_json(&fileToVal7, json_in8, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal7)));

    json_in9 = "\"foo\"";
    carbon_from_json(&fileToVal8, json_in9, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal8)));

}


TEST(SchemaTest, ValidateItemsArray) {

    carbon schemaFile;
    carbon fileToVal;
    carbon fileToVal3;
    carbon fileToVal4;
    carbon fileToVal5;
    carbon fileToVal6;
    carbon fileToVal7;
    carbon fileToVal8;
    carbon fileToVal9;
    carbon fileToVal10;
    carbon fileToVal11;
    const char *json_in;
    const char *json_in2;
    const char *json_in4;
    const char *json_in5;
    const char *json_in6;
    const char *json_in7;
    const char *json_in8;
    const char *json_in9;
    const char *json_in10;
    const char *json_in11;
    const char *json_in12;
    err err;

    json_in = "{ \"items\": [{\"type\": [\"string\"]}, {\"type\": [\"array\"]}] }";
    carbon_from_json(&schemaFile, json_in, CARBON_KEY_NOKEY, NULL, &err);

    json_in2 = "[[\"foo\", \"bar\"]]";
    carbon_from_json(&fileToVal, json_in2, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal)));

    json_in4 = "[[\"foo\", 6]]";
    carbon_from_json(&fileToVal3, json_in4, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal3)));

    json_in5 = "{}";
    carbon_from_json(&fileToVal4, json_in5, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal4)));

    json_in6 = "[[]]";
    carbon_from_json(&fileToVal5, json_in6, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal5)));

    json_in7 = "null";
    carbon_from_json(&fileToVal6, json_in7, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal6)));

    json_in8 = "3.1";
    carbon_from_json(&fileToVal7, json_in8, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal7)));

    json_in9 = "\"foo\"";
    carbon_from_json(&fileToVal8, json_in9, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal8)));

    json_in10 = "[[\"foo\", [1, 2]]]";
    carbon_from_json(&fileToVal9, json_in10, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal9)));

    json_in11 = "[[\"foo\", [1, \"bar\"], 1]]";
    carbon_from_json(&fileToVal10, json_in11, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal10)));

    json_in12 = "[[\"foo\", [1, \"bar\"]]]";
    carbon_from_json(&fileToVal11, json_in12, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal11)));
}


TEST(SchemaTest, ValidateAdditionalItemsBool) {

    carbon schemaFile;
    carbon fileToVal;
    carbon fileToVal3;
    const char *json_in;
    const char *json_in2;
    const char *json_in4;
    err err;

    json_in = "{ \"items\": [{\"type\": [\"string\"]}, {\"type\": [\"column\"]}], \"additionalItems\": true }";
    carbon_from_json(&schemaFile, json_in, CARBON_KEY_NOKEY, NULL, &err);

    json_in2 = "[[\"foo\", \"bar\"]]";
    carbon_from_json(&fileToVal, json_in2, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal)));

    json_in4 = "[[\"foo\", [1,2], \"bar\"]]";
    carbon_from_json(&fileToVal3, json_in4, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal3)));

}

TEST(SchemaTest, ValidateAdditionalItemsObject) {

    carbon schemaFile;
    carbon fileToVal;
    carbon fileToVal3;
    carbon fileToVal4;
    carbon fileToVal5;
    const char *json_in;
    const char *json_in2;
    const char *json_in4;
    const char *json_in5;
    const char *json_in6;
    err err;

    json_in = "{ \"items\": [{\"type\": \"string\"}, {\"type\": \"array\"}], \"additionalItems\": {\"type\": \"string\"} }";
    carbon_from_json(&schemaFile, json_in, CARBON_KEY_NOKEY, NULL, &err);

    json_in2 = "[[\"foo\", \"bar\"]]";
    carbon_from_json(&fileToVal, json_in2, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal)));

    json_in4 = "[[\"foo\", [1,2], \"bar\"]]";
    carbon_from_json(&fileToVal3, json_in4, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal3)));

    json_in5 = "[[\"foo\", [1,2], \"bar\", \"baz\"]]";
    carbon_from_json(&fileToVal4, json_in5, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal4)));

    json_in6 = "[[\"foo\", [1,2], 1]]";
    carbon_from_json(&fileToVal5, json_in6, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal5)));

}


TEST(SchemaTest, ValidateContains) {

    carbon schemaFile;
    carbon fileToVal;
    carbon fileToVal3;
    carbon fileToVal4;
    carbon fileToVal5;
    carbon fileToVal6;
    carbon fileToVal7;
    carbon fileToVal8;
    carbon fileToVal9;
    carbon fileToVal10;
    const char *json_in;
    const char *json_in2;
    const char *json_in4;
    const char *json_in5;
    const char *json_in6;
    const char *json_in7;
    const char *json_in8;
    const char *json_in9;
    const char *json_in10;
    const char *json_in11;
    err err;

    json_in = "{ \"contains\": {\"type\": [\"number\"]} }";
    carbon_from_json(&schemaFile, json_in, CARBON_KEY_NOKEY, NULL, &err);

    json_in2 = "[[\"foo\", \"bar\"]]";
    carbon_from_json(&fileToVal, json_in2, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal)));

    json_in4 = "[[\"foo\", 6]]";
    carbon_from_json(&fileToVal3, json_in4, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal3)));

    json_in5 = "{}";
    carbon_from_json(&fileToVal4, json_in5, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal4)));

    json_in6 = "[[]]";
    carbon_from_json(&fileToVal5, json_in6, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal5)));

    json_in7 = "null";
    carbon_from_json(&fileToVal6, json_in7, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal6)));

    json_in8 = "3.1";
    carbon_from_json(&fileToVal7, json_in8, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal7)));

    json_in9 = "\"foo\"";
    carbon_from_json(&fileToVal8, json_in9, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal8)));

    json_in10 = "[[\"foo\", [1, 2]]]";
    carbon_from_json(&fileToVal9, json_in10, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal9)));

    json_in11 = "[[\"foo\", [1, 2], 1]]";
    carbon_from_json(&fileToVal10, json_in11, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal10)));
}


TEST(SchemaTest, ValidateMinProperties) {

    carbon schemaFile;
    carbon fileToVal;
    carbon fileToVal3;
    carbon fileToVal4;
    carbon fileToVal5;
    carbon fileToVal6;
    carbon fileToVal7;
    carbon fileToVal8;
    carbon fileToVal9;
    const char *json_in;
    const char *json_in2;
    const char *json_in4;
    const char *json_in5;
    const char *json_in6;
    const char *json_in7;
    const char *json_in8;
    const char *json_in9;
    const char *json_in10;
    err err;

    json_in = "{ \"minProperties\": 2 }";
    carbon_from_json(&schemaFile, json_in, CARBON_KEY_NOKEY, NULL, &err);

    json_in2 = "{\"foo\":1, \"bar\":2, \"oof\": 1}";
    carbon_from_json(&fileToVal, json_in2, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal)));

    json_in4 = "{\"foo\":1}";
    carbon_from_json(&fileToVal3, json_in4, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal3)));

    json_in5 = "{}";
    carbon_from_json(&fileToVal4, json_in5, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal4)));

    json_in6 = "[[]]";
    carbon_from_json(&fileToVal5, json_in6, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal5)));

    json_in7 = "null";
    carbon_from_json(&fileToVal6, json_in7, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal6)));

    json_in8 = "3.1";
    carbon_from_json(&fileToVal7, json_in8, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal7)));

    json_in9 = "\"foo\"";
    carbon_from_json(&fileToVal8, json_in9, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal8)));

    json_in10 = "{\"foo\":1, \"bar\":2}";
    carbon_from_json(&fileToVal9, json_in10, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal9)));

}


TEST(SchemaTest, ValidateMaxProperties) {

    carbon schemaFile;
    carbon fileToVal;
    carbon fileToVal3;
    carbon fileToVal4;
    carbon fileToVal5;
    carbon fileToVal6;
    carbon fileToVal7;
    carbon fileToVal8;
    carbon fileToVal9;
    const char *json_in;
    const char *json_in2;
    const char *json_in4;
    const char *json_in5;
    const char *json_in6;
    const char *json_in7;
    const char *json_in8;
    const char *json_in9;
    const char *json_in10;
    err err;

    json_in = "{ \"maxProperties\": 2 }";
    carbon_from_json(&schemaFile, json_in, CARBON_KEY_NOKEY, NULL, &err);

    json_in2 = "{\"foo\":1, \"bar\":2, \"oof\": 1}";
    carbon_from_json(&fileToVal, json_in2, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal)));

    json_in4 = "{\"foo\":1}";
    carbon_from_json(&fileToVal3, json_in4, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal3)));

    json_in5 = "{}";
    carbon_from_json(&fileToVal4, json_in5, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal4)));

    json_in6 = "[[]]";
    carbon_from_json(&fileToVal5, json_in6, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal5)));

    json_in7 = "null";
    carbon_from_json(&fileToVal6, json_in7, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal6)));

    json_in8 = "3.1";
    carbon_from_json(&fileToVal7, json_in8, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal7)));

    json_in9 = "\"foo\"";
    carbon_from_json(&fileToVal8, json_in9, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal8)));

    json_in10 = "{\"foo\":1, \"bar\":2}";
    carbon_from_json(&fileToVal9, json_in10, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal9)));

}


TEST(SchemaTest, ValidateRequired) {

    carbon schemaFile;
    carbon fileToVal;
    carbon fileToVal3;
    carbon fileToVal4;
    carbon fileToVal5;
    carbon fileToVal6;
    carbon fileToVal7;
    carbon fileToVal8;
    carbon fileToVal9;
    const char *json_in;
    const char *json_in2;
    const char *json_in4;
    const char *json_in5;
    const char *json_in6;
    const char *json_in7;
    const char *json_in8;
    const char *json_in9;
    const char *json_in10;
    err err;

    json_in = "{ \"required\": [\"foo\", \"bar\"] }";
    carbon_from_json(&schemaFile, json_in, CARBON_KEY_NOKEY, NULL, &err);

    json_in2 = "{\"foo\":1, \"bar\":2, \"oof\": 1}";
    carbon_from_json(&fileToVal, json_in2, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal)));

    json_in4 = "{\"foo\":1}";
    carbon_from_json(&fileToVal3, json_in4, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal3)));

    json_in5 = "{}";
    carbon_from_json(&fileToVal4, json_in5, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal4)));

    json_in6 = "[[]]";
    carbon_from_json(&fileToVal5, json_in6, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal5)));

    json_in7 = "null";
    carbon_from_json(&fileToVal6, json_in7, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal6)));

    json_in8 = "3.1";
    carbon_from_json(&fileToVal7, json_in8, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal7)));

    json_in9 = "\"foo\"";
    carbon_from_json(&fileToVal8, json_in9, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal8)));

    json_in10 = "{\"foo\":1, \"bar\":2}";
    carbon_from_json(&fileToVal9, json_in10, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal9)));

}


TEST(SchemaTest, ValidateProperties) {

    carbon schemaFile;
    carbon fileToVal;
    carbon fileToVal3;
    carbon fileToVal4;
    carbon fileToVal5;
    carbon fileToVal6;
    carbon fileToVal7;
    carbon fileToVal8;
    carbon fileToVal9;
    const char *json_in;
    const char *json_in2;
    const char *json_in4;
    const char *json_in5;
    const char *json_in6;
    const char *json_in7;
    const char *json_in8;
    const char *json_in9;
    const char *json_in10;
    err err;

    json_in = "{ \"properties\": {\"foo\": {\"type\": [\"string\"]}, \"bar\": {\"type\": [\"number\"], \"minimum\": 2 } }, \"additionalProperties\": true }";
    carbon_from_json(&schemaFile, json_in, CARBON_KEY_NOKEY, NULL, &err);

    json_in2 = "{\"foo\":\"oof\", \"bar\":2, \"oof\": 1}";
    carbon_from_json(&fileToVal, json_in2, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal)));

    json_in4 = "{\"bar\":3}";
    carbon_from_json(&fileToVal3, json_in4, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal3)));

    json_in5 = "{}";
    carbon_from_json(&fileToVal4, json_in5, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal4)));

    json_in6 = "[[]]";
    carbon_from_json(&fileToVal5, json_in6, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal5)));

    json_in7 = "null";
    carbon_from_json(&fileToVal6, json_in7, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal6)));

    json_in8 = "3.1";
    carbon_from_json(&fileToVal7, json_in8, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal7)));

    json_in9 = "\"foo\"";
    carbon_from_json(&fileToVal8, json_in9, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal8)));

    json_in10 = "{\"foo\":\"oof\", \"bar\":1}";
    carbon_from_json(&fileToVal9, json_in10, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal9)));

}


TEST(SchemaTest, ValidatePatternProperties) {

    carbon schemaFile;
    carbon fileToVal;
    carbon fileToVal3;
    carbon fileToVal4;
    carbon fileToVal5;
    carbon fileToVal6;
    carbon fileToVal7;
    carbon fileToVal8;
    carbon fileToVal9;
    const char *json_in;
    const char *json_in2;
    const char *json_in4;
    const char *json_in5;
    const char *json_in6;
    const char *json_in7;
    const char *json_in8;
    const char *json_in9;
    const char *json_in10;
    err err;

    json_in = "{ \"patternProperties\": {\"^fo.*$\": {\"type\": [\"string\"]}, \"^ba.*$\": {\"type\": [\"number\"], \"minimum\": 2 } } }";
    carbon_from_json(&schemaFile, json_in, CARBON_KEY_NOKEY, NULL, &err);

    json_in2 = "{\"foo\":\"oof\", \"bar\":2, \"oof\": 1}";
    carbon_from_json(&fileToVal, json_in2, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal)));

    json_in4 = "{\"bar\":3}";
    carbon_from_json(&fileToVal3, json_in4, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal3)));

    json_in5 = "{}";
    carbon_from_json(&fileToVal4, json_in5, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal4)));

    json_in6 = "[[]]";
    carbon_from_json(&fileToVal5, json_in6, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal5)));

    json_in7 = "null";
    carbon_from_json(&fileToVal6, json_in7, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal6)));

    json_in8 = "3.1";
    carbon_from_json(&fileToVal7, json_in8, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal7)));

    json_in9 = "\"foo\"";
    carbon_from_json(&fileToVal8, json_in9, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal8)));

    json_in10 = "{\"foo\":\"oof\", \"bar\":1}";
    carbon_from_json(&fileToVal9, json_in10, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal9)));

}


TEST(SchemaTest, ValidateAdditionalPropertiesBool) {

    carbon schemaFile;
    carbon fileToVal;
    carbon fileToVal3;
    carbon fileToVal4;
    carbon fileToVal5;
    carbon fileToVal6;
    carbon fileToVal7;
    carbon fileToVal8;
    carbon fileToVal9;
    const char *json_in;
    const char *json_in2;
    const char *json_in4;
    const char *json_in5;
    const char *json_in6;
    const char *json_in7;
    const char *json_in8;
    const char *json_in9;
    const char *json_in10;
    err err;

    json_in = "{ \"patternProperties\": {\"^fo.*$\": {\"type\": [\"string\"]} }, \"properties\": { \"bar\": {\"type\": [\"number\"], \"minimum\": 2 } }, \"additionalProperties\": false }";
    carbon_from_json(&schemaFile, json_in, CARBON_KEY_NOKEY, NULL, &err);

    json_in2 = "{\"foo\":\"oof\", \"bar\":2, \"oof\": 1}";
    carbon_from_json(&fileToVal, json_in2, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal)));

    json_in4 = "{\"a\":3}";
    carbon_from_json(&fileToVal3, json_in4, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal3)));

    json_in5 = "{}";
    carbon_from_json(&fileToVal4, json_in5, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal4)));

    json_in6 = "[[]]";
    carbon_from_json(&fileToVal5, json_in6, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal5)));

    json_in7 = "null";
    carbon_from_json(&fileToVal6, json_in7, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal6)));

    json_in8 = "3.1";
    carbon_from_json(&fileToVal7, json_in8, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal7)));

    json_in9 = "\"foo\"";
    carbon_from_json(&fileToVal8, json_in9, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal8)));

    json_in10 = "{\"foo\":\"oof\", \"bar\":2}";
    carbon_from_json(&fileToVal9, json_in10, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal9)));

}

TEST(SchemaTest, ValidateAdditionalPropertiesObject) {

    carbon schemaFile;
    carbon fileToVal;
    carbon fileToVal3;
    carbon fileToVal4;
    carbon fileToVal5;
    carbon fileToVal6;
    carbon fileToVal7;
    carbon fileToVal8;
    carbon fileToVal9;
    const char *json_in;
    const char *json_in2;
    const char *json_in4;
    const char *json_in5;
    const char *json_in6;
    const char *json_in7;
    const char *json_in8;
    const char *json_in9;
    const char *json_in10;
    err err;

    json_in = "{ \"patternProperties\": {\"^fo.*$\": {\"type\": [\"string\"]} }, \"properties\": { \"bar\": {\"type\": [\"number\"], \"minimum\": 2 } }, \"additionalProperties\": {\"type\": [\"string\"] } }";
    carbon_from_json(&schemaFile, json_in, CARBON_KEY_NOKEY, NULL, &err);

    json_in2 = "{\"foo\":\"oof\", \"bar\":2, \"oof\": \"rab\"}";
    carbon_from_json(&fileToVal, json_in2, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal)));

    json_in4 = "{\"a\":3}";
    carbon_from_json(&fileToVal3, json_in4, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal3)));

    json_in5 = "{}";
    carbon_from_json(&fileToVal4, json_in5, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal4)));

    json_in6 = "[[]]";
    carbon_from_json(&fileToVal5, json_in6, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal5)));

    json_in7 = "null";
    carbon_from_json(&fileToVal6, json_in7, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal6)));

    json_in8 = "3.1";
    carbon_from_json(&fileToVal7, json_in8, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal7)));

    json_in9 = "\"foo\"";
    carbon_from_json(&fileToVal8, json_in9, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal8)));

    json_in10 = "{\"a\":\"oof\"}";
    carbon_from_json(&fileToVal9, json_in10, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal9)));

}


TEST(SchemaTest, ValidateDependencies) {

    carbon schemaFile;
    carbon fileToVal;
    carbon fileToVal3;
    carbon fileToVal4;
    carbon fileToVal5;
    carbon fileToVal6;
    carbon fileToVal7;
    carbon fileToVal8;
    carbon fileToVal9;
    carbon fileToVal10;
    const char *json_in;
    const char *json_in2;
    const char *json_in4;
    const char *json_in5;
    const char *json_in6;
    const char *json_in7;
    const char *json_in8;
    const char *json_in9;
    const char *json_in10;
    const char *json_in11;
    err err;

    json_in = "{ \"dependencies\": {\"foo\": [\"bar\", \"baz\"] } }";
    carbon_from_json(&schemaFile, json_in, CARBON_KEY_NOKEY, NULL, &err);

    json_in2 = "{\"foo\":\"oof\", \"bar\":2, \"baz\": \"rab\"}";
    carbon_from_json(&fileToVal, json_in2, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal)));

    json_in4 = "{\"a\":3}";
    carbon_from_json(&fileToVal3, json_in4, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal3)));

    json_in5 = "{}";
    carbon_from_json(&fileToVal4, json_in5, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal4)));

    json_in6 = "[[]]";
    carbon_from_json(&fileToVal5, json_in6, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal5)));

    json_in7 = "null";
    carbon_from_json(&fileToVal6, json_in7, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal6)));

    json_in8 = "3.1";
    carbon_from_json(&fileToVal7, json_in8, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal7)));

    json_in9 = "\"foo\"";
    carbon_from_json(&fileToVal8, json_in9, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal8)));

    json_in10 = "{\"bar\":\"oof\"}";
    carbon_from_json(&fileToVal9, json_in10, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal9)));

    json_in11 = "{\"foo\":\"oof\"}";
    carbon_from_json(&fileToVal10, json_in11, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal10)));

}


TEST(SchemaTest, ValidatePropertyNames) {

    carbon schemaFile;
    carbon fileToVal;
    carbon fileToVal3;
    carbon fileToVal4;
    carbon fileToVal5;
    carbon fileToVal6;
    carbon fileToVal7;
    carbon fileToVal8;
    const char *json_in;
    const char *json_in2;
    const char *json_in4;
    const char *json_in5;
    const char *json_in6;
    const char *json_in7;
    const char *json_in8;
    const char *json_in9;
    err err;

    json_in = "{ \"propertyNames\": {\"format\": \"date\"} }";
    carbon_from_json(&schemaFile, json_in, CARBON_KEY_NOKEY, NULL, &err);

    json_in2 = "{\"2019-12-29\": true}";
    carbon_from_json(&fileToVal, json_in2, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal)));

    json_in4 = "{\"a\":3}";
    carbon_from_json(&fileToVal3, json_in4, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal3)));

    json_in5 = "{}";
    carbon_from_json(&fileToVal4, json_in5, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal4)));

    json_in6 = "[[]]";
    carbon_from_json(&fileToVal5, json_in6, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal5)));

    json_in7 = "null";
    carbon_from_json(&fileToVal6, json_in7, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal6)));

    json_in8 = "3.1";
    carbon_from_json(&fileToVal7, json_in8, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal7)));

    json_in9 = "\"foo\"";
    carbon_from_json(&fileToVal8, json_in9, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal8)));

}


TEST(SchemaTest, ValidatePatternRequired) {

    carbon schemaFile;
    carbon fileToVal;
    carbon fileToVal3;
    carbon fileToVal4;
    carbon fileToVal5;
    carbon fileToVal6;
    carbon fileToVal7;
    carbon fileToVal8;
    const char *json_in;
    const char *json_in2;
    const char *json_in4;
    const char *json_in5;
    const char *json_in6;
    const char *json_in7;
    const char *json_in8;
    const char *json_in9;
    err err;

    json_in = "{ \"patternRequired\": [\"f.*o\"] }";
    carbon_from_json(&schemaFile, json_in, CARBON_KEY_NOKEY, NULL, &err);

    json_in2 = "{\"2019-12-29\": true}";
    carbon_from_json(&fileToVal, json_in2, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal)));

    json_in4 = "{\"foo\":3, \"bar\":1}";
    carbon_from_json(&fileToVal3, json_in4, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal3)));

    json_in5 = "{}";
    carbon_from_json(&fileToVal4, json_in5, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal4)));

    json_in6 = "[[]]";
    carbon_from_json(&fileToVal5, json_in6, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal5)));

    json_in7 = "null";
    carbon_from_json(&fileToVal6, json_in7, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal6)));

    json_in8 = "3.1";
    carbon_from_json(&fileToVal7, json_in8, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal7)));

    json_in9 = "\"foo\"";
    carbon_from_json(&fileToVal8, json_in9, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal8)));

}


TEST(SchemaTest, ValidateEnum) {

    carbon schemaFile;
    carbon fileToVal;
    carbon fileToVal3;
    carbon fileToVal4;
    carbon fileToVal5;
    carbon fileToVal6;
    carbon fileToVal7;
    carbon fileToVal8;
    carbon fileToVal9;
    carbon fileToVal10;
    carbon fileToVal11;
    carbon fileToVal12;
    const char *json_in;
    const char *json_in2;
    const char *json_in4;
    const char *json_in5;
    const char *json_in6;
    const char *json_in7;
    const char *json_in8;
    const char *json_in9;
    const char *json_in10;
    const char *json_in11;
    const char *json_in12;
    const char *json_in13;
    err err;

    json_in = "{ \"enum\": [ 2, \"foo\", {\"foo\": \"bar\" }, [1, 2, 3] ] }";
    carbon_from_json(&schemaFile, json_in, CARBON_KEY_NOKEY, NULL, &err);

    json_in2 = "2";
    carbon_from_json(&fileToVal, json_in2, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal)));

    json_in4 = "\"foo\"";
    carbon_from_json(&fileToVal3, json_in4, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal3)));

    json_in5 = "{}";
    carbon_from_json(&fileToVal4, json_in5, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal4)));

    json_in6 = "[[]]";
    carbon_from_json(&fileToVal5, json_in6, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal5)));

    json_in7 = "null";
    carbon_from_json(&fileToVal6, json_in7, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal6)));

    json_in8 = "1";
    carbon_from_json(&fileToVal7, json_in8, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal7)));

    json_in9 = "\"{\"foo\": \"bar\"}";
    carbon_from_json(&fileToVal8, json_in9, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal8)));

    json_in10 = "[[1, 2, 3]]";
    carbon_from_json(&fileToVal9, json_in10, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal9)));

    json_in11 = "[[1, 2, 3, 4]]";
    carbon_from_json(&fileToVal10, json_in11, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal10)));

    json_in12 = "\"{\"foo\": \"baz\"}";
    carbon_from_json(&fileToVal11, json_in12, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal11)));

    json_in13 = "\"foz\"";
    carbon_from_json(&fileToVal12, json_in13, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal12)));
}


TEST(SchemaTest, ValidateConst) {

    carbon schemaFile;
    carbon fileToVal;
    carbon fileToVal3;
    carbon fileToVal4;
    carbon fileToVal5;
    carbon fileToVal6;
    carbon fileToVal7;
    carbon fileToVal8;
    carbon fileToVal9;
    carbon fileToVal10;
    carbon fileToVal11;
    carbon fileToVal12;
    const char *json_in;
    const char *json_in2;
    const char *json_in4;
    const char *json_in5;
    const char *json_in6;
    const char *json_in7;
    const char *json_in8;
    const char *json_in9;
    const char *json_in10;
    const char *json_in11;
    const char *json_in12;
    const char *json_in13;
    err err;

    json_in = "{ \"enum\": [ 2, \"foo\", {\"foo\": \"bar\" }, [1, 2, 3] ] }";
    carbon_from_json(&schemaFile, json_in, CARBON_KEY_NOKEY, NULL, &err);

    json_in2 = "2";
    carbon_from_json(&fileToVal, json_in2, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal)));

    json_in4 = "\"foo\"";
    carbon_from_json(&fileToVal3, json_in4, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal3)));

    json_in5 = "{}";
    carbon_from_json(&fileToVal4, json_in5, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal4)));

    json_in6 = "[[]]";
    carbon_from_json(&fileToVal5, json_in6, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal5)));

    json_in7 = "null";
    carbon_from_json(&fileToVal6, json_in7, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal6)));

    json_in8 = "1";
    carbon_from_json(&fileToVal7, json_in8, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal7)));

    json_in9 = "\"{\"foo\": \"bar\"}";
    carbon_from_json(&fileToVal8, json_in9, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal8)));

    json_in10 = "[[1, 2, 3]]";
    carbon_from_json(&fileToVal9, json_in10, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal9)));

    json_in11 = "[[1, 2, 3, 4]]";
    carbon_from_json(&fileToVal10, json_in11, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal10)));

    json_in12 = "\"{\"foo\": \"baz\"}";
    carbon_from_json(&fileToVal11, json_in12, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal11)));

    json_in13 = "\"foz\"";
    carbon_from_json(&fileToVal12, json_in13, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal12)));
}


TEST(SchemaTest, ValidateNot) {

    carbon schemaFile;
    carbon fileToVal;
    carbon fileToVal3;
    carbon fileToVal4;
    carbon fileToVal5;
    carbon fileToVal6;
    carbon fileToVal7;
    carbon fileToVal8;
    carbon fileToVal9;
    const char *json_in;
    const char *json_in2;
    const char *json_in4;
    const char *json_in5;
    const char *json_in6;
    const char *json_in7;
    const char *json_in8;
    const char *json_in9;
    const char *json_in10;
    err err;

    json_in = "{ \"not\": { \"minimum\": 3 } }";
    carbon_from_json(&schemaFile, json_in, CARBON_KEY_NOKEY, NULL, &err);

    json_in2 = "{\"foo\": true}";
    carbon_from_json(&fileToVal, json_in2, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal)));

    json_in4 = "2";
    carbon_from_json(&fileToVal3, json_in4, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal3)));

    json_in5 = "{}";
    carbon_from_json(&fileToVal4, json_in5, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal4)));

    json_in6 = "[[]]";
    carbon_from_json(&fileToVal5, json_in6, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal5)));

    json_in7 = "null";
    carbon_from_json(&fileToVal6, json_in7, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal6)));

    json_in8 = "4";
    carbon_from_json(&fileToVal7, json_in8, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal7)));

    json_in9 = "\"foo\"";
    carbon_from_json(&fileToVal8, json_in9, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal8)));

    json_in10 = "[[\"foo\", \"bar\", \"oof\", \"rab\"]]";
    carbon_from_json(&fileToVal9, json_in10, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal9)));

}


TEST(SchemaTest, ValidateOneOf) {

    carbon schemaFile;
    carbon fileToVal;
    carbon fileToVal3;
    carbon fileToVal4;
    carbon fileToVal5;
    carbon fileToVal6;
    carbon fileToVal7;
    carbon fileToVal8;
    const char *json_in;
    const char *json_in2;
    const char *json_in4;
    const char *json_in5;
    const char *json_in6;
    const char *json_in7;
    const char *json_in8;
    const char *json_in9;
    err err;

    json_in = "{ \"oneOf\": [{ \"minimum\": 3 }, {\"maximum\": 3}] }";
    carbon_from_json(&schemaFile, json_in, CARBON_KEY_NOKEY, NULL, &err);

    json_in2 = "\"foo\"";
    carbon_from_json(&fileToVal, json_in2, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal)));

    json_in4 = "2";
    carbon_from_json(&fileToVal3, json_in4, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal3)));

    json_in5 = "{}";
    carbon_from_json(&fileToVal4, json_in5, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal4)));

    json_in6 = "[[]]";
    carbon_from_json(&fileToVal5, json_in6, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal5)));

    json_in7 = "null";
    carbon_from_json(&fileToVal6, json_in7, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal6)));

    json_in8 = "3";
    carbon_from_json(&fileToVal7, json_in8, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal7)));

    json_in9 = "4";
    carbon_from_json(&fileToVal8, json_in9, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal8)));

}


TEST(SchemaTest, ValidateAnyOf) {

    carbon schemaFile;
    carbon fileToVal;
    carbon fileToVal3;
    carbon fileToVal4;
    carbon fileToVal5;
    carbon fileToVal6;
    carbon fileToVal7;
    carbon fileToVal8;
    const char *json_in;
    const char *json_in2;
    const char *json_in4;
    const char *json_in5;
    const char *json_in6;
    const char *json_in7;
    const char *json_in8;
    const char *json_in9;
    err err;

    json_in = "{ \"anyOf\": [{ \"minimum\": 5}, {\"multipleOf\": 2}] }";
    carbon_from_json(&schemaFile, json_in, CARBON_KEY_NOKEY, NULL, &err);

    json_in2 = "\"foo\"";
    carbon_from_json(&fileToVal, json_in2, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal)));

    json_in4 = "1";
    carbon_from_json(&fileToVal3, json_in4, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal3)));

    json_in5 = "{}";
    carbon_from_json(&fileToVal4, json_in5, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal4)));

    json_in6 = "6";
    carbon_from_json(&fileToVal5, json_in6, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal5)));

    json_in7 = "null";
    carbon_from_json(&fileToVal6, json_in7, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal6)));

    json_in8 = "4";
    carbon_from_json(&fileToVal7, json_in8, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal7)));

    json_in9 = "[[\"foo\"]]";
    carbon_from_json(&fileToVal8, json_in9, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal8)));

}


TEST(SchemaTest, ValidateAllOf) {

    carbon schemaFile;
    carbon fileToVal;
    carbon fileToVal3;
    carbon fileToVal4;
    carbon fileToVal5;
    carbon fileToVal6;
    carbon fileToVal7;
    carbon fileToVal8;
    const char *json_in;
    const char *json_in2;
    const char *json_in4;
    const char *json_in5;
    const char *json_in6;
    const char *json_in7;
    const char *json_in8;
    const char *json_in9;
    err err;

    json_in = "{ \"allOf\": [{ \"type\": [\"number\"] }, {\"minimum\": 3}] }";
    carbon_from_json(&schemaFile, json_in, CARBON_KEY_NOKEY, NULL, &err);

    json_in2 = "\"foo\"";
    carbon_from_json(&fileToVal, json_in2, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal)));

    json_in4 = "2";
    carbon_from_json(&fileToVal3, json_in4, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal3)));

    json_in5 = "{}";
    carbon_from_json(&fileToVal4, json_in5, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal4)));

    json_in6 = "[[]]";
    carbon_from_json(&fileToVal5, json_in6, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal5)));

    json_in7 = "null";
    carbon_from_json(&fileToVal6, json_in7, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal6)));

    json_in8 = "4.5";
    carbon_from_json(&fileToVal7, json_in8, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal7)));

    json_in9 = "[[\"test\"]]";
    carbon_from_json(&fileToVal8, json_in9, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal8)));

}


TEST(SchemaTest, ValidateIfThenElse) {

    carbon schemaFile;
    carbon fileToVal;
    carbon fileToVal3;
    carbon fileToVal4;
    carbon fileToVal5;
    carbon fileToVal6;
    carbon fileToVal7;
    carbon fileToVal8;
    const char *json_in;
    const char *json_in2;
    const char *json_in4;
    const char *json_in5;
    const char *json_in6;
    const char *json_in7;
    const char *json_in8;
    const char *json_in9;
    err err;

    json_in = "{ \"if\": { \"type\": [\"number\"] }, \"then\": {\"minimum\": 5}, \"else\": {\"type\": [\"string\"]} }";
    carbon_from_json(&schemaFile, json_in, CARBON_KEY_NOKEY, NULL, &err);

    json_in2 = "\"foo\"";
    carbon_from_json(&fileToVal, json_in2, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal)));

    json_in4 = "2";
    carbon_from_json(&fileToVal3, json_in4, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal3)));

    json_in5 = "{}";
    carbon_from_json(&fileToVal4, json_in5, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal4)));

    json_in6 = "[[]]";
    carbon_from_json(&fileToVal5, json_in6, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal5)));

    json_in7 = "null";
    carbon_from_json(&fileToVal6, json_in7, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal6)));

    json_in8 = "5";
    carbon_from_json(&fileToVal7, json_in8, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_TRUE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal7)));

    json_in9 = "[[\"test\"]]";
    carbon_from_json(&fileToVal8, json_in9, CARBON_KEY_NOKEY, NULL, &err);
    ASSERT_FALSE(FN_IS_OK(schema_validate(&schemaFile, &fileToVal8)));

}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
