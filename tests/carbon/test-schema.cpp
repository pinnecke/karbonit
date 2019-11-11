#include <fcntl.h>
#include <gtest/gtest.h>
#include <jakson/jakson.h>

using test_cases = std::vector<const char*>;

static void schema_test_validation(const char* json_schema,
        const test_cases& expect_failure_data,
        const test_cases& expect_success_data)
{
    for (const char* expect_failure : expect_failure_data) {
        schema s;
        schema_generate_from_json(&s, json_schema, CARBON_KEY_NOKEY, NULL);

        bool parsed =
            FN_IS_OK(schema_validate_json(&s, expect_failure, CARBON_KEY_NOKEY, NULL));
        ASSERT_FALSE(parsed);

        // schema_drop(&s)
    }

    for (const char* expect_success : expect_success_data) {
        schema s;
        schema_generate_from_json(&s, json_schema, CARBON_KEY_NOKEY, NULL);
        bool parsed =
            FN_IS_OK(schema_validate_json(&s, expect_success, CARBON_KEY_NOKEY, NULL));
        ASSERT_TRUE(parsed);

        // schema_drop(&s);
    }
}

// array tests currently have to be nested, see issue
// https://github.com/jaksonlabs/jakson/issues/3
TEST(SchemaTest, ValidateType)
{
    const char* json_schema = "{\"type\": [\"string\", \"object\"]}";

    const test_cases expect_failure = {"10", "[[]]"};

    const test_cases expect_success = {"\"foo\"", "{\"foo\":\"bar\"}",
        "{\"foo\":[\"bar\"]}", "{}"};

    schema_test_validation(json_schema, expect_failure, expect_success);
}

TEST(SchemaTest, ValidateMinimum)
{
    const char* json_schema = "{\"minimum\": 5.3}";

    const test_cases expect_failure = {"5"};
    const test_cases expect_success = {
        "5.3", "46000000000000000000", "{}", "[[]]", "null", "\"foo\""};

    schema_test_validation(json_schema, expect_failure, expect_success);
}

TEST(SchemaTest, ValidateMaximum)
{
    const char* json_schema = "{\"maximum\": 5.3}";

    const test_cases expect_failure = {"6"};
    const test_cases expect_success = {"5",	  "5.3",  "{}",
        "[[]]", "null", "\"foo\""};

    schema_test_validation(json_schema, expect_failure, expect_success);
}

TEST(SchemaTest, ValidateExclusiveMinimum)
{
    const char* json_schema = "{\"exclusiveMinimum\": 5.3}";

    const test_cases expect_failure = {"5", "5.3"};
    const test_cases expect_success = {"5.4", "{}", "[[]]", "null", "\"foo\""};

    schema_test_validation(json_schema, expect_failure, expect_success);
}

TEST(SchemaTest, ValidateExclusiveMaximum)
{
    const char* json_schema = "{\"exclusiveMaximum\": 5.3}";

    const test_cases expect_success = {"5.2", "{}", "[[]]", "null", "\"foo\""};
    const test_cases expect_failure = {"5.3", "5.4"};

    schema_test_validation(json_schema, expect_failure, expect_success);
}

TEST(SchemaTest, ValidateMultipleOf)
{
    const char* json_schema = "{\"multipleOf\": 6.2}";

    const test_cases expect_success = {"12.4", "{}", "[[]]", "\"foo\""};
    const test_cases expect_failure = {"12", "6", "3.1"};

    schema_test_validation(json_schema, expect_failure, expect_success);
}

TEST(SchemaTest, ValidateMinLength)
{
    const char* json_schema = "{\"minLength\": 6}";

    const test_cases expect_failure = {"\"foo\""};
    const test_cases expect_success = {"\"foobar\"", "\"foobarz\"", "{}",
        "[[]]",		"null",		   "3.1"};

    schema_test_validation(json_schema, expect_failure, expect_success);
}

TEST(SchemaTest, ValidateMaxLength)
{
    const char* json_schema = "{\"maxLength\": 6}";

    const test_cases expect_failure = {"\"foobarz\""};
    const test_cases expect_success = {"\"foo\"", "\"foobar\"", "{}",
        "[[]]",	 "null",	   "3.1"};

    schema_test_validation(json_schema, expect_failure, expect_success);
}

TEST(SchemaTest, ValidatePattern)
{
    const char* json_schema = "{\"pattern\": \"[abc]+\"}";

    const test_cases expect_failure = {"\"\"", "\"def\""};
    const test_cases expect_success = {"\"a\"", "\"abcd\"", "\"cde\"", "{}",
        "[[]]",  "null",	   "3.1"};

    schema_test_validation(json_schema, expect_failure, expect_success);
}

TEST(SchemaTest, ValidateFormatIPv4)
{
    const char* json_schema = "{\"format\": \"ipv4\"}";

    const test_cases expect_failure = {"\"cde\"", "\"\""};
    const test_cases expect_success = {"\"192.168.0.1\"", "{}", "[[]]", "null",
        "3.1"};

    schema_test_validation(json_schema, expect_failure, expect_success);
}

TEST(SchemaTest, ValidateFormatDate)
{
    const char* json_schema = "{\"format\": \"date\"}";

    const test_cases expect_failure = {"\"cde\"", "\"\""};
    const test_cases expect_success = {"\"2050-12-31\"", "{}", "[[]]", "null",
        "3.1"};

    schema_test_validation(json_schema, expect_failure, expect_success);
}

TEST(SchemaTest, ValidateFormatDateTime)
{
    const char* json_schema = "{\"format\": \"date-time\"}";

    const test_cases expect_failure = {"\"cde\"", "\"2050-12-31\""};
    const test_cases expect_success = {"\"2050-12-31 12:30\"", "{}", "[[]]",
        "null", "3.1"};

    schema_test_validation(json_schema, expect_failure, expect_success);
}

TEST(SchemaTest, ValidateFormatURI)
{
    const char* json_schema = "{\"format\": \"uri\"}";

    const test_cases expect_failure = {"\"cde\"", "\"\""};

    const test_cases expect_success = {"\"mailto:John.Doe@example.com\"", "{}",
        "[[]]", "null", "3.1"};

    schema_test_validation(json_schema, expect_failure, expect_success);
}

TEST(SchemaTest, ValidateFormatEmail)
{
    const char* json_schema = "{\"format\": \"email\"}";

    const test_cases expect_failure = {"\"cde@local\"", "\"\""};

    const test_cases expect_success = {"\"foo@local.host\"", "{}", "[[]]",
        "null", "3.1"};

    schema_test_validation(json_schema, expect_failure, expect_success);
}

TEST(SchemaTest, ValidateFormatHostname)
{
    const char* json_schema = "{\"format\": \"hostname\"}";

    const test_cases expect_failure = {"\"cde@local\"", "\"\""};

    const test_cases expect_success = {"\"local.host\"", "{}", "[[]]", "null",
        "3.1"};

    schema_test_validation(json_schema, expect_failure, expect_success);
}

TEST(SchemaTest, ValidateFormatIPv6)
{
    const char* json_schema = "{\"format\": \"ipv6\"}";

    const test_cases expect_failure = {"\"192.168.0.1\""};

    const test_cases expect_success = {
        "\"2001:0db8:0000:0000:0000:8a2e:0370:7334\"",
        "\"2001:db8::8a2e:370:7334\"",
        "{}",
        "[[]]",
        "null",
        "3.1"};

    schema_test_validation(json_schema, expect_failure, expect_success);
}

TEST(SchemaTest, ValidateFormatRegex)
{
    const char* json_schema = "{\"format\": \"regex\"}";

    const test_cases expect_failure = {
        "\"[\"",
    };
    const test_cases expect_success = {
        "{}", "[[]]", "null", "3.1",
        "\"^(([a-zA-Z0-9]|[a-zA-Z0-9][a-zA-Z0-9\\-]*[a-zA-Z0-9])\\.)*([A-Za-z0-"
            "9]|[A-Za-z0-9][A-Za-z0-9\\-]*[A-Za-z0-9])$\""};

    schema_test_validation(json_schema, expect_failure, expect_success);
}

TEST(SchemaTest, ValidateMinItems)
{
    const char* json_schema = "{\"minItems\": 5}";

    const test_cases expect_failure = {"[[1, 2, 3, 4]]"};

    const test_cases expect_success = {"[[1, 2, 3, 4, 5, 6]]",
        "{}",
        "[[1, 2, 3, 4, 5]]",
        "null",
        "3.1",
        "\"foo\""};

    schema_test_validation(json_schema, expect_failure, expect_success);
}

TEST(SchemaTest, ValidateMaxItems)
{
    const char* json_schema = "{\"maxItems\": 5}";

    const test_cases expect_failure = {"[[1, 2, 3, 4, 5, 6]]"};
    const test_cases expect_success = {
        "[[1, 2, 3, 4, 5]]", "{}", "[[]]", "null", "3.1", "\"foo\""};

    schema_test_validation(json_schema, expect_failure, expect_success);
}

TEST(SchemaTest, ValidateItemsObject)
{
    const char* json_schema = "{\"items\": { \"type\": [\"string\"]} }";

    const test_cases expect_failure = {
        "[[\"foo\", 6]]",
    };

    const test_cases expect_success = {
        "[[\"foo\", \"bar\"]]", "{}", "[[]]", "null", "3.1", "\"foo\""};

    schema_test_validation(json_schema, expect_failure, expect_success);
}

TEST(SchemaTest, ValidateItemsArray)
{
    const char* json_schema =
        "{ \"items\": [{\"type\": [\"string\"]}, {\"type\": [\"array\"]}] }";

    const test_cases expect_failure = {
        "[[\"foo\", \"bar\"]]",
        "[[\"foo\", 6]]",
        "[[\"foo\", [1, 2]]]",
        "[[\"foo\", [1, \"bar\"], 1]]",
    };

    const test_cases expect_success = {
        "{}", "[[]]", "null", "3.1", "\"foo\"", "[[\"foo\", [1, \"bar\"]]]"};

    schema_test_validation(json_schema, expect_failure, expect_success);
}

TEST(SchemaTest, ValidateAdditionalItemsBool)
{
    const char* json_schema =
        "{ \"items\": [{\"type\": [\"string\"]}, {\"type\": [\"column\"]}], "
        "\"additionalItems\": true }";

    const test_cases expect_failure = {"[[\"foo\", \"bar\"]]"};

    const test_cases expect_success = {"[[\"foo\", [1,2], \"bar\"]]"};

    schema_test_validation(json_schema, expect_failure, expect_success);
}

TEST(SchemaTest, ValidateAdditionalItemsObject)
{
    const char* json_schema =
        "{ \"items\": [{\"type\": \"string\"}, {\"type\": \"array\"}], "
        "\"additionalItems\": {\"type\": \"string\"} }";

    const test_cases expect_failure = {"[[\"foo\", \"bar\"]]",
        "[[\"foo\", [1,2], 1]]"};

    const test_cases expect_success = {"[[\"foo\", [1,2], \"bar\"]]",
        "[[\"foo\", [1,2], \"bar\", \"baz\"]]"};

    schema_test_validation(json_schema, expect_failure, expect_success);
}

TEST(SchemaTest, ValidateContains)
{
    const char* json_schema = "{ \"contains\": {\"type\": [\"number\"]} }";

    const test_cases expect_failure = {"[[\"foo\", \"bar\"]]", "[[]]",
        "[[\"foo\", [1, 2]]]"};

    const test_cases expect_success = {
        "[[\"foo\", 6]]",		 "{}", "null", "3.1", "\"foo\"",
        "[[\"foo\", [1, 2], 1]]"};

    schema_test_validation(json_schema, expect_failure, expect_success);
}

TEST(SchemaTest, ValidateMinProperties)
{
    const char* json_schema = "{ \"minProperties\": 2 }";

    const test_cases expect_failure = {"{\"foo\":1}", "{}"};

    const test_cases expect_success = {"{\"foo\":1, \"bar\":2, \"oof\": 1}",
        "[[]]",
        "null",
        "3.1",
        "\"foo\"",
        "{\"foo\":1, \"bar\":2}"};

    schema_test_validation(json_schema, expect_failure, expect_success);
}

TEST(SchemaTest, ValidateMaxProperties)
{
    const char* json_schema = "{ \"maxProperties\": 2 }";

    const test_cases expect_failure = {"{\"foo\":1, \"bar\":2, \"oof\": 1}"};

    const test_cases expect_success = {
        "{\"foo\":1}",			 "{}", "[[]]", "null", "3.1", "\"foo\"",
        "{\"foo\":1, \"bar\":2}"};

    schema_test_validation(json_schema, expect_failure, expect_success);
}

TEST(SchemaTest, ValidateRequired)
{
    const char* json_schema = "{ \"required\": [\"foo\", \"bar\"] }";

    const test_cases expect_failure = {"{\"foo\":1}", "{}"};

    const test_cases expect_success = {"{\"foo\":1, \"bar\":2, \"oof\": 1}",
        "[[]]",
        "null",
        "3.1",
        "\"foo\"",
        "{\"foo\":1, \"bar\":2}"};

    schema_test_validation(json_schema, expect_failure, expect_success);
}

TEST(SchemaTest, ValidateProperties)
{
    const char* json_schema =
        "{ \"properties\": {\"foo\": {\"type\": [\"string\"]}, \"bar\": "
        "{\"type\": [\"number\"], \"minimum\": 2 } }, "
        "\"additionalProperties\": true }";

    const test_cases expect_failure = {"{\"foo\":\"oof\", \"bar\":1}"};

    const test_cases expect_success = {
        "{\"foo\":\"oof\", \"bar\":2, \"oof\": 1}",
        "{\"bar\":3}",
        "{}",
        "[[]]",
        "null",
        "3.1",
        "\"foo\"",
    };

    schema_test_validation(json_schema, expect_failure, expect_success);
}

TEST(SchemaTest, ValidatePatternProperties)
{
    const char* json_schema =
        "{ \"patternProperties\": {\"^fo.*$\": {\"type\": [\"string\"]}, "
        "\"^ba.*$\": {\"type\": [\"number\"], \"minimum\": 2 } } }";

    const test_cases expect_failure = {"{\"foo\":\"oof\", \"bar\":1}"};

    const test_cases expect_success = {
        "{\"foo\":\"oof\", \"bar\":2, \"oof\": 1}",
        "{\"bar\":3}",
        "{}",
        "[[]]",
        "null",
        "3.1",
        "\"foo\"",
    };

    schema_test_validation(json_schema, expect_failure, expect_success);
}

TEST(SchemaTest, ValidateAdditionalPropertiesBool)
{
    const char* json_schema =
        "{ \"patternProperties\": {\"^fo.*$\": {\"type\": [\"string\"]} }, "
        "\"properties\": { \"bar\": {\"type\": [\"number\"], \"minimum\": 2 } "
        "}, \"additionalProperties\": false }";

    const test_cases expect_failure = {
        "{\"foo\":\"oof\", \"bar\":2, \"oof\": 1}",
        "{\"a\":3}",
    };

    const test_cases expect_success = {
        "{}", "[[]]", "null", "3.1", "\"foo\"", "{\"foo\":\"oof\", \"bar\":2}"};

    schema_test_validation(json_schema, expect_failure, expect_success);
}

TEST(SchemaTest, ValidateAdditionalPropertiesObject)
{
    const char* json_schema =
        "{ \"patternProperties\": {\"^fo.*$\": {\"type\": [\"string\"]} }, "
        "\"properties\": { \"bar\": {\"type\": [\"number\"], \"minimum\": 2 } "
        "}, \"additionalProperties\": {\"type\": [\"string\"] } }";

    const test_cases expect_failure = {
        "{\"a\":3}",
    };

    const test_cases expect_success = {
        "{\"foo\":\"oof\", \"bar\":2, \"oof\": \"rab\"}",
        "{}",
        "[[]]",
        "null",
        "3.1",
        "\"foo\"",
        "{\"a\":\"oof\"}"};

    schema_test_validation(json_schema, expect_failure, expect_success);
}

TEST(SchemaTest, ValidateDependencies)
{
    const char* json_schema =
        "{ \"dependencies\": {\"foo\": [\"bar\", \"baz\"] } }";

    const test_cases expect_failure = {"{\"foo\":\"oof\"}"};

    const test_cases expect_success = {
        "{\"foo\":\"oof\", \"bar\":2, \"baz\": \"rab\"}",
        "{\"a\":3}",
        "{}",
        "[[]]",
        "null",
        "3.1",
        "\"foo\"",
        "{\"bar\":\"oof\"}"};

    schema_test_validation(json_schema, expect_failure, expect_success);
}

TEST(SchemaTest, ValidatePropertyNames)
{
    const char* json_schema = "{ \"propertyNames\": {\"format\": \"date\"} }";

    const test_cases expect_failure = {
        "{\"a\":3}",
    };

    const test_cases expect_success = {
        "{\"2019-12-29\": true}", "{}", "[[]]", "null", "3.1", "\"foo\""};

    schema_test_validation(json_schema, expect_failure, expect_success);
}

TEST(SchemaTest, ValidatePatternRequired)
{
    const char* json_schema = "{ \"patternRequired\": [\"f.*o\"] }";

    const test_cases expect_failure = {"{\"2019-12-29\": true}", "{}"};

    const test_cases expect_success = {"{\"foo\":3, \"bar\":1}", "[[]]", "null",
        "3.1", "\"foo\""};

    schema_test_validation(json_schema, expect_failure, expect_success);
}

TEST(SchemaTest, ValidateEnum)
{
    const char* json_schema =
        "{ \"enum\": [ 2, \"foo\", {\"foo\": \"bar\" }, [1, 2, 3] ] }";

    const test_cases expect_failure = {
        "{}",	  "[[]]", "null", "1", "[[1, 2, 3, 4]]", "\"{\"foo\": \"baz\"}",
        "\"foz\""};

    const test_cases expect_success = {
        "2",
        "\"foo\"",
        "\"{\"foo\": \"bar\"}",
        "[[1, 2, 3]]",
    };

    schema_test_validation(json_schema, expect_failure, expect_success);
}

TEST(SchemaTest, ValidateNot)
{
    const char* json_schema = "{ \"not\": { \"minimum\": 3 } }";

    const test_cases expect_failure = {
        "{\"foo\": true}",
        "{}",
        "[[]]",
        "null",
        "4",
        "\"foo\"",
        "[[\"foo\", \"bar\", \"oof\", \"rab\"]]"};

    const test_cases expect_success = {
        "2",
    };

    schema_test_validation(json_schema, expect_failure, expect_success);
}

TEST(SchemaTest, ValidateOneOf)
{
    const char* json_schema =
        "{ \"oneOf\": [{ \"minimum\": 3 }, {\"maximum\": 3}] }";

    const test_cases expect_failure = {"\"foo\"", "{}", "[[]]", "null", "3"};

    const test_cases expect_success = {"2", "4"};

    schema_test_validation(json_schema, expect_failure, expect_success);
}

TEST(SchemaTest, ValidateAnyOf)
{
    const char* json_schema =
        "{ \"anyOf\": [{ \"minimum\": 5}, {\"multipleOf\": 2}] }";

    const test_cases expect_failure = {"1"};

    const test_cases expect_success = {"\"foo\"", "{}", "6",
        "null",	 "4",  "[[\"foo\"]]"};

    schema_test_validation(json_schema, expect_failure, expect_success);
}

TEST(SchemaTest, ValidateAllOf)
{
    const char* json_schema =
        "{ \"allOf\": [{ \"type\": [\"number\"] }, {\"minimum\": 3}] }";

    const test_cases expect_failure = {"\"foo\"", "2",	  "{}",
        "[[]]",	  "null", "[[\"test\"]]"};

    const test_cases expect_success = {"4.5"};

    schema_test_validation(json_schema, expect_failure, expect_success);
}

TEST(SchemaTest, ValidateIfThenElse)
{
    const char* json_schema =
        "{ \"if\": { \"type\": [\"number\"] }, \"then\": {\"minimum\": 5}, "
        "\"else\": {\"type\": [\"string\"]} }";

    const test_cases expect_failure = {"2", "{}", "[[]]", "null",
        "[[\"test\"]]"};

    const test_cases expect_success = {"5"};

    schema_test_validation(json_schema, expect_failure, expect_success);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
