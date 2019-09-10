#include <gtest/gtest.h>

#include <jakson/jakson.h>

TEST(SchemaTest, ValidateTypeString) {
    jak_carbon schemaCarbon;
    jak_carbon fileToVal1;
    jak_carbon fileToVal2;
    jak_carbon fileToVal3;
    jak_carbon fileToVal4;
    jak_carbon fileToVal5;
    jak_carbon fileToVal6;
    //jak_carbon fileToVal7;
    //jak_carbon fileToVal8;
    jak_error err;
    const char *json_in;
    jak_carbon_schema_input input;
    input.num_files = 0;

    jak_carbon_schema_input_file *files = (jak_carbon_schema_input_file*) malloc(10*sizeof(jak_carbon_schema_input_file*));
    input.files = files;

    json_in = "{\"type\": \"string\"}";
    jak_carbon_from_json(&schemaCarbon, json_in, JAK_CARBON_KEY_NOKEY, NULL, &err);

    json_in = "\"foo\"";
    jak_carbon_from_json(&fileToVal1, json_in, JAK_CARBON_KEY_NOKEY, NULL, &err);
    input.files[0].file_name = "testfile_1";
    input.files[0].data = &fileToVal1;
    input.num_files++;

    json_in = "{\"foo\": \"bar\"}";
    jak_carbon_from_json(&fileToVal2, json_in, JAK_CARBON_KEY_NOKEY, NULL, &err);
    input.files[1].file_name = "testfile_2";
    input.files[1].data = &fileToVal2;
    input.num_files++;

    json_in = "{\"foo\": 2}";
    jak_carbon_from_json(&fileToVal3, json_in, JAK_CARBON_KEY_NOKEY, NULL, &err);
    input.files[2].file_name = "testfile_3";
    input.files[2].data = &fileToVal3;
    input.num_files++;

    json_in = "{}";
    jak_carbon_from_json(&fileToVal4, json_in, JAK_CARBON_KEY_NOKEY, NULL, &err);
    input.files[3].file_name = "testfile_4";
    input.files[3].data = &fileToVal4;
    input.num_files++;

    json_in = "[]";
    jak_carbon_from_json(&fileToVal5, json_in, JAK_CARBON_KEY_NOKEY, NULL, &err);
    input.files[4].file_name = "testfile_5";
    input.files[4].data = &fileToVal5;
    input.num_files++;

    json_in = "4.5";
    jak_carbon_from_json(&fileToVal6, json_in, JAK_CARBON_KEY_NOKEY, NULL, &err);
    input.files[5].file_name = "testfile_6";
    input.files[5].data = &fileToVal6;
    input.num_files++;

    //json_in = "true";
    //jak_carbon_from_json(&fileToVal7, json_in, JAK_CARBON_KEY_NOKEY, NULL, &err);
    //input.files[6].file_name = "testfile_7";
    //input.files[6].data = &fileToVal7;
    //input.num_files++;

    //json_in = "null";
    //jak_carbon_from_json(&fileToVal8, json_in, JAK_CARBON_KEY_NOKEY, NULL, &err);
    //input.files[7].file_name = "testfile_8";
    //input.files[7].data = &fileToVal8;
    //input.num_files++;

    // currently segfaults
    // json_in = 2;
    // jak_carbon_from_json(&fileToVal, json_in, JAK_CARBON_KEY_NOKEY, NULL, &err);
    // input.files[3].file_name = "testfile_3";
    // input.files[3].data = &fileToVal;
    // input.num_files++;

    // json_in = "{\"foo\"}";
    // jak_carbon_from_json(&fileToVal, json_in, JAK_CARBON_KEY_NOKEY, NULL, &err);
    // input.files[3].file_name = "testfile_4";
    // input.files[3].data = &fileToVal;
    // input.num_files++;

    jak_carbon_schema_validate(&schemaCarbon, &input);

    ASSERT_TRUE(input.files[0].passed);
    ASSERT_FALSE(input.files[1].passed);
    ASSERT_FALSE(input.files[2].passed);
    ASSERT_FALSE(input.files[3].passed);
    ASSERT_FALSE(input.files[4].passed);
    ASSERT_FALSE(input.files[5].passed);
    //ASSERT_FALSE(input.files[6].passed);
    //ASSERT_FALSE(input.files[7].passed);

}
    

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
