
#ifndef __cplusplus
# include <stdatomic.h>
#else
# include <atomic>
# define _Atomic(X) std::atomic< X >
#endif

#include <gtest/gtest.h>
#include <stdio.h>
#include <inttypes.h>

#include <karbonit/karbonit.h>

static void
iterate_properties(prop_iter *prop_iter);

static void
iterate_object_vals(archive_value_vector *value_iter)
{
    bool status;
    bool is_object;
    u32 VEC_LENGTH;
    archive_object object;
    prop_iter  prop_iter;

    status = archive_value_vec_is_of_objects(&is_object, value_iter);
    ASSERT_TRUE(status);
    ASSERT_TRUE(is_object);

    status = archive_value_vec_get_length(&VEC_LENGTH, value_iter);
    ASSERT_TRUE(status);

    for (u32 i = 0; i < VEC_LENGTH; i++)
    {
        status = archive_value_vec_get_object_at(&object, i, value_iter);
        ASSERT_TRUE(status);
        printf("\t\t{type: object, id: %" PRIu64 "}\n", object.object_id);


        status = archive_prop_iter_from_object(&prop_iter, ARCHIVE_ITER_MASK_ANY, &object);
        ASSERT_TRUE(status);

        iterate_properties(&prop_iter);
    }
}

static void
iterate_object(archive_value_vector *value_iter)
{
    ASSERT_TRUE (!value_iter->is_array);
    iterate_object_vals(value_iter);
}

static void
print_basic_fixed_types_basic(archive_value_vector *value_iter, u32 idx)
{
    u32 num_values;
    switch (value_iter->prop_type) {
    case ARCHIVE_FIELD_INT8: {
        const archive_field_i8_t *values = archive_value_vec_get_int8s(&num_values, value_iter);
        ASSERT_TRUE(values != NULL);
        ASSERT_TRUE(idx < num_values);
        printf("\t\t{ type: int8, value: %" PRIi8 " }\n", values[idx]);
    } break;
    case ARCHIVE_FIELD_INT16: {
        const archive_field_i16_t *values = archive_value_vec_get_int16s(&num_values, value_iter);
        ASSERT_TRUE(values != NULL);
        ASSERT_TRUE(idx < num_values);
        printf("\t\t{ type: int16, value: %" PRIi16 " }\n", values[idx]);
    } break;
    case ARCHIVE_FIELD_INT32: {
        const archive_field_i32_t *values = archive_value_vec_get_int32s(&num_values, value_iter);
        ASSERT_TRUE(values != NULL);
        ASSERT_TRUE(idx < num_values);
        printf("\t\t{ type: int32, value: %" PRIi32 " }\n", values[idx]);
    } break;
    case ARCHIVE_FIELD_INT64: {
        const archive_field_i64_t *values = archive_value_vec_get_int64s(&num_values, value_iter);
        ASSERT_TRUE(values != NULL);
        ASSERT_TRUE(idx < num_values);
        printf("\t\t{ type: int64, value: %" PRIi64 " }\n", values[idx]);
    } break;
    case ARCHIVE_FIELD_UINT8: {
        const archive_field_u8_t *values = archive_value_vec_get_uint8s(&num_values, value_iter);
        ASSERT_TRUE(values != NULL);
        ASSERT_TRUE(idx < num_values);
        printf("\t\t{ type: uint8, value: %" PRIu8 " }\n", values[idx]);
    } break;
    case ARCHIVE_FIELD_UINT16: {
        const archive_field_u16_t *values = archive_value_vec_get_uint16s(&num_values, value_iter);
        ASSERT_TRUE(values != NULL);
        ASSERT_TRUE(idx < num_values);
        printf("\t\t{ type: uint16, value: %" PRIu16 " }\n", values[idx]);
    } break;
    case ARCHIVE_FIELD_UINT32: {
        const archive_field_u32_t *values = archive_value_vec_get_uint32s(&num_values, value_iter);
        ASSERT_TRUE(values != NULL);
        ASSERT_TRUE(idx < num_values);
        printf("\t\t{ type: uint32, value: %" PRIu32 " }\n", values[idx]);
    } break;
    case ARCHIVE_FIELD_UINT64: {
        const archive_field_u64_t *values = archive_value_vec_get_uint64s(&num_values, value_iter);
        ASSERT_TRUE(values != NULL);
        ASSERT_TRUE(idx < num_values);
        printf("\t\t{ type: uint64, value: %" PRIu64 " }\n", values[idx]);
    } break;
    case ARCHIVE_FIELD_FLOAT: {
        const archive_field_number_t *values = archive_value_vec_get_numbers(&num_values, value_iter);
        ASSERT_TRUE(values != NULL);
        ASSERT_TRUE(idx < num_values);
        printf("\t\t{ type: number, value: %f }\n", values[idx]);
    } break;
    case ARCHIVE_FIELD_STRING: {
            const archive_field_sid_t *values = archive_value_vec_get_strings(&num_values, value_iter);
            ASSERT_TRUE(values != NULL);
            ASSERT_TRUE(idx < num_values);
            printf("\t\t{ type: str_buf, value: %" PRIu64 " }\n", values[idx]);
        } break;
    case ARCHIVE_FIELD_BOOLEAN: {
        const archive_field_boolean_t *values = archive_value_vec_get_booleans(&num_values, value_iter);
        ASSERT_TRUE(values != NULL);
        ASSERT_TRUE(idx < num_values);
        printf("\t\t{ type: boolean, value: %d }\n", values[idx]);
    } break;
    default:
        FAIL() << "Unsupported basic type";
    }
}

static void
print_basic_fixed_types_array(archive_value_vector *value_iter, u32 idx)
{
    u32 array_length;
    switch (value_iter->prop_type) {
    case ARCHIVE_FIELD_NULL: {
        const archive_field_u32_t *number_contained = archive_value_vec_get_null_arrays(&array_length, value_iter);
        ASSERT_TRUE(number_contained != NULL);
        ASSERT_TRUE(array_length != 0);
        printf("\t\t{ type: null_array, values: [");
        for (u32 i = 0; i < array_length; i++)
        {
            printf("null%s", i + 1 < array_length ? ", " : "");
        }
        printf("]\n");
    } break;
    case ARCHIVE_FIELD_INT8: {
        const archive_field_i8_t *values = archive_value_vec_get_int8_arrays_at(&array_length, idx, value_iter);
        ASSERT_TRUE(values != NULL);
        ASSERT_TRUE(array_length != 0);
        printf("\t\t{ type: int8_array, values: [");
        for (u32 i = 0; i < array_length; i++)
        {
            printf("%d%s", values[i], i + 1 < array_length ? ", " : "");
        }
        printf("]\n");
    } break;
    case ARCHIVE_FIELD_INT16: {
        const archive_field_i16_t *values = archive_value_vec_get_int16_arrays_at(&array_length, idx, value_iter);
        ASSERT_TRUE(values != NULL);
        ASSERT_TRUE(array_length != 0);
        printf("\t\t{ type: int16_array, values: [");
        for (u32 i = 0; i < array_length; i++)
        {
            printf("%" PRIi16 "%s", values[i], i + 1 < array_length ? ", " : "");
        }
        printf("]\n");
    } break;
    case ARCHIVE_FIELD_INT32: {
        const archive_field_i32_t *values = archive_value_vec_get_int32_arrays_at(&array_length, idx, value_iter);
        ASSERT_TRUE(values != NULL);
        ASSERT_TRUE(array_length != 0);
        printf("\t\t{ type: int32_array, values: [");
        for (u32 i = 0; i < array_length; i++)
        {
            printf("%" PRIi32 "%s", values[i], i + 1 < array_length ? ", " : "");
        }
        printf("]\n");
    } break;
    case ARCHIVE_FIELD_INT64: {
        const archive_field_i64_t *values = archive_value_vec_get_int64_arrays_at(&array_length, idx, value_iter);
        ASSERT_TRUE(values != NULL);
        ASSERT_TRUE(array_length != 0);
        printf("\t\t{ type: int64_array, values: [");
        for (u32 i = 0; i < array_length; i++)
        {
            printf("%" PRIi64 "%s", values[i], i + 1 < array_length ? ", " : "");
        }
        printf("]\n");
    } break;
    case ARCHIVE_FIELD_UINT8: {
        const archive_field_u8_t *values = archive_value_vec_get_uint8_arrays_at(&array_length, idx, value_iter);
        ASSERT_TRUE(values != NULL);
        ASSERT_TRUE(array_length != 0);
        printf("\t\t{ type: uint8_array, values: [");
        for (u32 i = 0; i < array_length; i++)
        {
            printf("%" PRIu8 "%s", values[i], i + 1 < array_length ? ", " : "");
        }
        printf("]\n");
    } break;
    case ARCHIVE_FIELD_UINT16: {
        const archive_field_u16_t *values = archive_value_vec_get_uint16_arrays_at(&array_length, idx, value_iter);
        ASSERT_TRUE(values != NULL);
        ASSERT_TRUE(array_length != 0);
        printf("\t\t{ type: uint16_array, values: [");
        for (u32 i = 0; i < array_length; i++)
        {
            printf("%" PRIu16 "%s", values[i], i + 1 < array_length ? ", " : "");
        }
        printf("]\n");
    } break;
    case ARCHIVE_FIELD_UINT32: {
        const archive_field_u32_t *values = archive_value_vec_get_uint32_arrays_at(&array_length, idx, value_iter);
        ASSERT_TRUE(values != NULL);
        ASSERT_TRUE(array_length != 0);
        printf("\t\t{ type: uint32_array, values: [");
        for (u32 i = 0; i < array_length; i++)
        {
            printf("%" PRIu32 "%s", values[i], i + 1 < array_length ? ", " : "");
        }
        printf("]\n");
    } break;
    case ARCHIVE_FIELD_UINT64: {
        const archive_field_u64_t *values = archive_value_vec_get_uint64_arrays_at(&array_length, idx, value_iter);
        ASSERT_TRUE(values != NULL);
        ASSERT_TRUE(array_length != 0);
        printf("\t\t{ type: uint64_array, values: [");
        for (u32 i = 0; i < array_length; i++)
        {
            printf("%" PRIu64 "%s", values[i], i + 1 < array_length ? ", " : "");
        }
        printf("]\n");
    } break;
    case ARCHIVE_FIELD_FLOAT: {
        const archive_field_number_t *values = archive_value_vec_get_number_arrays_at(&array_length, idx, value_iter);
        ASSERT_TRUE(values != NULL);
        ASSERT_TRUE(array_length != 0);
        printf("\t\t{ type: numbers_array, values: [");
        for (u32 i = 0; i < array_length; i++)
        {
            printf("%f%s", values[i], i + 1 < array_length ? ", " : "");
        }
        printf("]\n");
    } break;
    case ARCHIVE_FIELD_STRING: {
        const archive_field_sid_t *values = archive_value_vec_get_string_arrays_at(&array_length, idx, value_iter);
        ASSERT_TRUE(values != NULL);
        ASSERT_TRUE(array_length != 0);
        printf("\t\t{ type: string_array, values: [");
        for (u32 i = 0; i < array_length; i++)
        {
            printf("%" PRIu64 "%s", values[i], i + 1 < array_length ? ", " : "");
        }
        printf("]\n");
    } break;
    case ARCHIVE_FIELD_BOOLEAN: {
        const archive_field_boolean_t *values = archive_value_vec_get_boolean_arrays_at(&array_length, idx, value_iter);
        ASSERT_TRUE(values != NULL);
        ASSERT_TRUE(array_length != 0);
        printf("\t\t{ type: boolean_array, values: [");
        for (u32 i = 0; i < array_length; i++)
        {
            printf("%" PRIu8 "%s", values[i], i + 1 < array_length ? ", " : "");
        }
        printf("]\n");
    } break;
    default:
        FAIL() << "Unsupported basic type";
    }

}

static void
print_basic_fixed_types(archive_value_vector *value_iter, u32 idx)
{
    if (value_iter->is_array) {
        print_basic_fixed_types_array(value_iter, idx);
    } else {
        print_basic_fixed_types_basic(value_iter, idx);
    }
}




static void
iterate_properties(struct prop_iter *prop_iter)
{
    unique_id_t                oid;
    archive_value_vector     value_iter;
    enum archive_field_type               type;
    bool                              is_array;
    const archive_field_sid_t         *keys;
    u32                          num_pairs;
    prop_iter_mode_e   iter_type;
    independent_iter_state  collection_iter;
    u32                          num_column_groups;
    independent_iter_state group_iter;
    independent_iter_state       column_iter;
    independent_iter_state entry_iter;

    while (archive_prop_iter_next(&iter_type, &value_iter, &collection_iter, prop_iter))
    {
        if (iter_type == PROP_ITER_MODE_OBJECT)
        {
            keys = archive_value_vec_get_keys(&num_pairs, &value_iter);
            archive_value_vec_is_array_type(&is_array, &value_iter);
            archive_value_vec_get_basic_type(&type, &value_iter);
            archive_value_vec_get_object_id(&oid, &value_iter);
            for (u32 i = 0; i < num_pairs; i++) {
                printf("Key %" PRIu64 ", type: %d, is-array: %d\n", keys[i], type, is_array);

                switch (type) {
                case ARCHIVE_FIELD_OBJECT:
                    iterate_object(&value_iter);
                    break;
                case ARCHIVE_FIELD_NULL:
                    printf("\t\t{ type: null }\n");
                    break;
                case ARCHIVE_FIELD_INT8:
                case ARCHIVE_FIELD_INT16:
                case ARCHIVE_FIELD_INT32:
                case ARCHIVE_FIELD_INT64:
                case ARCHIVE_FIELD_UINT8:
                case ARCHIVE_FIELD_UINT16:
                case ARCHIVE_FIELD_UINT32:
                case ARCHIVE_FIELD_UINT64:
                case ARCHIVE_FIELD_FLOAT:
                case ARCHIVE_FIELD_STRING:
                case ARCHIVE_FIELD_BOOLEAN:
                    print_basic_fixed_types(&value_iter, i);
                    break;
                default:
                    FAIL() << "unknown basic type";
                }
            }
        } else {
            keys = archive_collection_iter_get_keys(&num_column_groups, &collection_iter);
            ASSERT_TRUE(keys != NULL);
            printf("\t\t{ column groups for keys:");
            for (u32 i = 0; i < num_column_groups; i++) {
                printf("%" PRIu64 " ", keys[i]);
            }
            printf("}\n");
            while (archive_collection_next_column_group(&group_iter, &collection_iter)) {

                u32 num_objs;
                const unique_id_t *ids = archive_column_group_get_object_ids(&num_objs, &group_iter);

                printf("\t\t{ column groups object ids:");
                for (u32 i = 0; i < num_objs; i++) {
                    printf("%" PRIu64 " ", ids[i]);
                }
                printf("}\n");

                while(archive_column_group_next_column(&column_iter, &group_iter)) {
                    archive_field_sid_t column_name;
                    enum archive_field_type column_entry_type;
                    u32 num_entries;
                    archive_column_get_name(&column_name, &column_entry_type, &column_iter);
                    const u32 *positions = archive_column_get_entry_positions(&num_entries, &column_iter);
                    printf("\t\t{ column-name: %" PRIu64 ", type: %d }\n", column_name, column_entry_type);
                    printf("\t\t{ entry positions:");
                    for (u32 i = 0; i < num_entries; i++) {
                        printf("%" PRIu32 " ", positions[i]);
                    }
                    printf("}\n");

                    while(archive_column_next_entry(&entry_iter, &column_iter)) {

                        DECLARE_AND_INIT(enum archive_field_type, entry_type)
                        DECLARE_AND_INIT(u32, entry_length)
                        archive_column_entry_get_type(&entry_type, &entry_iter);

                        switch (entry_type) {
                        case ARCHIVE_FIELD_STRING: {
                            const archive_field_sid_t *values = archive_column_entry_get_strings(&entry_length, &entry_iter);
                            printf("\t\t{ strings: [");
                            for (u32 i = 0; i < entry_length; i++) {
                                printf("%" PRIu64 "%s", values[i], i + 1 < entry_length ? ", " : "");
                            }
                            printf("]\n");
                        } break;
                        case ARCHIVE_FIELD_INT8: {
                            const archive_field_i8_t *values = archive_column_entry_get_int8s(&entry_length, &entry_iter);
                            printf("\t\t{ int8s: [");
                            for (u32 i = 0; i < entry_length; i++) {
                                printf("% " PRIi8 "%s", values[i], i + 1 < entry_length ? ", " : "");
                            }
                            printf("]\n");
                        } break;
                        case ARCHIVE_FIELD_INT16: {
                            const archive_field_i16_t *values = archive_column_entry_get_int16s(&entry_length, &entry_iter);
                            printf("\t\t{ int16s: [");
                            for (u32 i = 0; i < entry_length; i++) {
                                printf("% " PRIi16 "%s", values[i], i + 1 < entry_length ? ", " : "");
                            }
                            printf("]\n");
                        } break;
                        case ARCHIVE_FIELD_INT32: {
                            const archive_field_i32_t *values = archive_column_entry_get_int32s(&entry_length, &entry_iter);
                            printf("\t\t{ int32s: [");
                            for (u32 i = 0; i < entry_length; i++) {
                                printf("% " PRIi32 "%s", values[i], i + 1 < entry_length ? ", " : "");
                            }
                            printf("]\n");
                        } break;
                        case ARCHIVE_FIELD_INT64: {
                            const archive_field_i64_t *values = archive_column_entry_get_int64s(&entry_length, &entry_iter);
                            printf("\t\t{ int64s: [");
                            for (u32 i = 0; i < entry_length; i++) {
                                printf("% " PRIi64 "%s", values[i], i + 1 < entry_length ? ", " : "");
                            }
                            printf("]\n");
                        } break;
                        case ARCHIVE_FIELD_UINT8: {
                            const archive_field_u8_t *values = archive_column_entry_get_uint8s(&entry_length, &entry_iter);
                            printf("\t\t{ uint8s: [");
                            for (u32 i = 0; i < entry_length; i++) {
                                printf("%" PRIu8 "%s", values[i], i + 1 < entry_length ? ", " : "");
                            }
                            printf("]\n");
                        } break;
                        case ARCHIVE_FIELD_UINT16: {
                            const archive_field_u16_t *values = archive_column_entry_get_uint16s(&entry_length, &entry_iter);
                            printf("\t\t{ uint16s: [");
                            for (u32 i = 0; i < entry_length; i++) {
                                printf("%" PRIu16 "%s", values[i], i + 1 < entry_length ? ", " : "");
                            }
                            printf("]\n");
                        } break;
                        case ARCHIVE_FIELD_UINT32: {
                            const archive_field_u32_t *values = archive_column_entry_get_uint32s(&entry_length, &entry_iter);
                            printf("\t\t{ uint32s: [");
                            for (u32 i = 0; i < entry_length; i++) {
                                printf("%" PRIu32 "%s", values[i], i + 1 < entry_length ? ", " : "");
                            }
                            printf("]\n");
                        } break;
                        case ARCHIVE_FIELD_UINT64: {
                            const archive_field_u64_t *values = archive_column_entry_get_uint64s(&entry_length, &entry_iter);
                            printf("\t\t{ uint64s: [");
                            for (u64 i = 0; i < entry_length; i++) {
                                printf("%" PRIu64 "%s", values[i], i + 1 < entry_length ? ", " : "");
                            }
                            printf("]\n");
                        } break;
                        case ARCHIVE_FIELD_FLOAT: {
                            const archive_field_number_t *values = archive_column_entry_get_numbers(&entry_length, &entry_iter);
                            printf("\t\t{ numbers: [");
                            for (u64 i = 0; i < entry_length; i++) {
                                printf("%f%s", values[i], i + 1 < entry_length ? ", " : "");
                            }
                            printf("]\n");
                        } break;
                        case ARCHIVE_FIELD_BOOLEAN: {
                            const archive_field_boolean_t *values = archive_column_entry_get_booleans(&entry_length, &entry_iter);
                            printf("\t\t{ booleans: [");
                            for (u64 i = 0; i < entry_length; i++) {
                                printf("%d%s", values[i], i + 1 < entry_length ? ", " : "");
                            }
                            printf("]\n");
                        } break;
                        case ARCHIVE_FIELD_NULL: {
                            const archive_field_u32_t *values = archive_column_entry_get_nulls(&entry_length, &entry_iter);
                            printf("\t\t{ nulls: [");
                            for (u64 i = 0; i < entry_length; i++) {
                                printf("%d%s", values[i], i + 1 < entry_length ? ", " : "");
                            }
                            printf("]\n");
                        } break;
                        case ARCHIVE_FIELD_OBJECT: {
                            column_object_iter iter;
                            const archive_object *archive_object;
                            archive_column_entry_get_objects(&iter, &entry_iter);
                            printf("\t\t{ << objects >>: [");
                            while ((archive_object = archive_column_entry_object_iter_next_object(&iter)) != NULL) {
                                unique_id_t id;
                                archive_object_get_object_id(&id, archive_object);
                                printf("{ oid: %" PRIu64 " } \n", id);

                                struct prop_iter nested_obj_prop_iter;
                                archive_prop_iter_from_object(&nested_obj_prop_iter, ARCHIVE_ITER_MASK_ANY,
                                                                     archive_object);
                                iterate_properties(&nested_obj_prop_iter);
                            }
                            printf("]\n");
                        } break;
                        default:
                            FAIL() << "Unknown type";
                        }

                    }
                }

            }
        }
    }
}

TEST(ArchiveIterTest, CreateIterator)
{
    archive            archive;
    prop_iter  prop_iter;
    bool                        status;

    /* in order to access this file, the working directory of this test executable must be set to a sub directory
     * below the projects root directory (e.g., 'build/') */
    status = archive_open(&archive, "./assets/test-archive.carbon");
    ASSERT_TRUE(status);

    status = archive_prop_iter_from_archive(&prop_iter, ARCHIVE_ITER_MASK_ANY, &archive);
    ASSERT_TRUE(status);

    iterate_properties(&prop_iter);

    archive_close(&archive);
    ASSERT_TRUE(status);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}