#include <gtest/gtest.h>
#include <jakson/jakson.h>

static void make_memfile(memfile *memfile) {
        memblock *memblock;
        memblock_create(&memblock, 1024);
        memfile_open(memfile, memblock, READ_WRITE);
}

static void drop_memfile(memfile *memfile) {
        memblock_drop(memfile->memblock);
}

TEST(TestAbstractTypeMarker, DetectBaseTypeByBase) {

        memfile memfile;
        make_memfile(&memfile);

        abstract_e type;

        /* object is base type */
        {
                abstract_write_base_type(&memfile, CARBON_CONTAINER_OBJECT);
                memfile_seek_from_here(&memfile, -1);
                ASSERT_TRUE(abstract_type(&type, &memfile));
                ASSERT_EQ(type, ABSTRACT_BASE);
        }

        /* array is base type */
        {
                abstract_write_base_type(&memfile, CARBON_CONTAINER_ARRAY);
                memfile_seek_from_here(&memfile, -1);
                ASSERT_TRUE(abstract_type(&type, &memfile));
                ASSERT_EQ(type, ABSTRACT_BASE);
        }

        /* column-... is base type */
        {
                abstract_write_base_type(&memfile, CARBON_CONTAINER_COLUMN_U8);
                memfile_seek_from_here(&memfile, -1);
                ASSERT_TRUE(abstract_type(&type, &memfile));
                ASSERT_EQ(type, ABSTRACT_BASE);

                abstract_write_base_type(&memfile, CARBON_CONTAINER_COLUMN_U16);
                memfile_seek_from_here(&memfile, -1);
                ASSERT_TRUE(abstract_type(&type, &memfile));
                ASSERT_EQ(type, ABSTRACT_BASE);

                abstract_write_base_type(&memfile, CARBON_CONTAINER_COLUMN_U32);
                memfile_seek_from_here(&memfile, -1);
                ASSERT_TRUE(abstract_type(&type, &memfile));
                ASSERT_EQ(type, ABSTRACT_BASE);

                abstract_write_base_type(&memfile, CARBON_CONTAINER_COLUMN_U64);
                memfile_seek_from_here(&memfile, -1);
                ASSERT_TRUE(abstract_type(&type, &memfile));
                ASSERT_EQ(type, ABSTRACT_BASE);

                abstract_write_base_type(&memfile, CARBON_CONTAINER_COLUMN_I8);
                memfile_seek_from_here(&memfile, -1);
                ASSERT_TRUE(abstract_type(&type, &memfile));
                ASSERT_EQ(type, ABSTRACT_BASE);

                abstract_write_base_type(&memfile, CARBON_CONTAINER_COLUMN_I16);
                memfile_seek_from_here(&memfile, -1);
                ASSERT_TRUE(abstract_type(&type, &memfile));
                ASSERT_EQ(type, ABSTRACT_BASE);

                abstract_write_base_type(&memfile, CARBON_CONTAINER_COLUMN_I32);
                memfile_seek_from_here(&memfile, -1);
                ASSERT_TRUE(abstract_type(&type, &memfile));
                ASSERT_EQ(type, ABSTRACT_BASE);

                abstract_write_base_type(&memfile, CARBON_CONTAINER_COLUMN_I64);
                memfile_seek_from_here(&memfile, -1);
                ASSERT_TRUE(abstract_type(&type, &memfile));
                ASSERT_EQ(type, ABSTRACT_BASE);

                abstract_write_base_type(&memfile, CARBON_CONTAINER_COLUMN_FLOAT);
                memfile_seek_from_here(&memfile, -1);
                ASSERT_TRUE(abstract_type(&type, &memfile));
                ASSERT_EQ(type, ABSTRACT_BASE);

                abstract_write_base_type(&memfile, CARBON_CONTAINER_COLUMN_BOOLEAN);
                memfile_seek_from_here(&memfile, -1);
                ASSERT_TRUE(abstract_type(&type, &memfile));
                ASSERT_EQ(type, ABSTRACT_BASE);
        }
        drop_memfile(&memfile);
}

TEST(TestAbstractTypeMarker, DetectBaseTypeByDerivedType) {

        memfile memfile;
        make_memfile(&memfile);

        abstract_e type;

        /* CARBON_UNSORTED_MULTIMAP is base type */
        {
                abstract_write_derived_type(&memfile, CARBON_UNSORTED_MULTIMAP);
                memfile_seek_from_here(&memfile, -1);
                ASSERT_TRUE(abstract_type(&type, &memfile));
                ASSERT_EQ(type, ABSTRACT_BASE);
        }

        /* CARBON_UNSORTED_MULTISET_ARRAY is base type */
        {
                abstract_write_derived_type(&memfile, CARBON_UNSORTED_MULTISET_ARRAY);
                memfile_seek_from_here(&memfile, -1);
                ASSERT_TRUE(abstract_type(&type, &memfile));
                ASSERT_EQ(type, ABSTRACT_BASE);
        }

        /* CARBON_UNSORTED_MULTISET_COL_U8 is base type */
        {
                abstract_write_derived_type(&memfile, CARBON_UNSORTED_MULTISET_COL_U8);
                memfile_seek_from_here(&memfile, -1);
                ASSERT_TRUE(abstract_type(&type, &memfile));
                ASSERT_EQ(type, ABSTRACT_BASE);
        }

        /* CARBON_UNSORTED_MULTISET_COL_U16 is base type */
        {
                abstract_write_derived_type(&memfile, CARBON_UNSORTED_MULTISET_COL_U16);
                memfile_seek_from_here(&memfile, -1);
                ASSERT_TRUE(abstract_type(&type, &memfile));
                ASSERT_EQ(type, ABSTRACT_BASE);
        }

        /* CARBON_UNSORTED_MULTISET_COL_U32 is base type */
        {
                abstract_write_derived_type(&memfile, CARBON_UNSORTED_MULTISET_COL_U32);
                memfile_seek_from_here(&memfile, -1);
                ASSERT_TRUE(abstract_type(&type, &memfile));
                ASSERT_EQ(type, ABSTRACT_BASE);
        }

        /* CARBON_UNSORTED_MULTISET_COL_U64 is base type */
        {
                abstract_write_derived_type(&memfile, CARBON_UNSORTED_MULTISET_COL_U64);
                memfile_seek_from_here(&memfile, -1);
                ASSERT_TRUE(abstract_type(&type, &memfile));
                ASSERT_EQ(type, ABSTRACT_BASE);
        }

        /* CARBON_UNSORTED_MULTISET_COL_I8 is base type */
        {
                abstract_write_derived_type(&memfile, CARBON_UNSORTED_MULTISET_COL_I8);
                memfile_seek_from_here(&memfile, -1);
                ASSERT_TRUE(abstract_type(&type, &memfile));
                ASSERT_EQ(type, ABSTRACT_BASE);
        }

        /* CARBON_UNSORTED_MULTISET_COL_I16 is base type */
        {
                abstract_write_derived_type(&memfile, CARBON_UNSORTED_MULTISET_COL_I16);
                memfile_seek_from_here(&memfile, -1);
                ASSERT_TRUE(abstract_type(&type, &memfile));
                ASSERT_EQ(type, ABSTRACT_BASE);
        }

        /* CARBON_UNSORTED_MULTISET_COL_I32 is base type */
        {
                abstract_write_derived_type(&memfile, CARBON_UNSORTED_MULTISET_COL_I32);
                memfile_seek_from_here(&memfile, -1);
                ASSERT_TRUE(abstract_type(&type, &memfile));
                ASSERT_EQ(type, ABSTRACT_BASE);
        }

        /* CARBON_UNSORTED_MULTISET_COL_I32 is base type */
        {
                abstract_write_derived_type(&memfile, CARBON_UNSORTED_MULTISET_COL_I32);
                memfile_seek_from_here(&memfile, -1);
                ASSERT_TRUE(abstract_type(&type, &memfile));
                ASSERT_EQ(type, ABSTRACT_BASE);
        }

        /* CARBON_UNSORTED_MULTISET_COL_I64 is base type */
        {
                abstract_write_derived_type(&memfile, CARBON_UNSORTED_MULTISET_COL_I64);
                memfile_seek_from_here(&memfile, -1);
                ASSERT_TRUE(abstract_type(&type, &memfile));
                ASSERT_EQ(type, ABSTRACT_BASE);
        }

        /* CARBON_UNSORTED_MULTISET_COL_FLOAT is base type */
        {
                abstract_write_derived_type(&memfile, CARBON_UNSORTED_MULTISET_COL_FLOAT);
                memfile_seek_from_here(&memfile, -1);
                ASSERT_TRUE(abstract_type(&type, &memfile));
                ASSERT_EQ(type, ABSTRACT_BASE);
        }

        /* CARBON_UNSORTED_MULTISET_COL_BOOLEAN is base type */
        {
                abstract_write_derived_type(&memfile, CARBON_UNSORTED_MULTISET_COL_BOOLEAN);
                memfile_seek_from_here(&memfile, -1);
                ASSERT_TRUE(abstract_type(&type, &memfile));
                ASSERT_EQ(type, ABSTRACT_BASE);
        }

        drop_memfile(&memfile);
}


static void test_derived_is_base(memfile *memfile, derived_e type, bool is_base)
{
        bool result;
        abstract_write_derived_type(memfile, type);
        memfile_seek_from_here(memfile, -1);
        bool status = abstract_is_base(&result, memfile);
        ASSERT_EQ(status, true);
        ASSERT_EQ(result, is_base);
}

TEST(TestAbstractTypeMarker, DetectNonBaseTypeByDerivedType) {

        memfile memfile;
        make_memfile(&memfile);

        test_derived_is_base(&memfile, CARBON_UNSORTED_MULTIMAP, true);
        test_derived_is_base(&memfile, CARBON_SORTED_MULTIMAP, false);
        test_derived_is_base(&memfile, CARBON_UNSORTED_MAP, false);
        test_derived_is_base(&memfile, CARBON_SORTED_MAP, false);

        test_derived_is_base(&memfile, CARBON_UNSORTED_MULTISET_ARRAY, true);
        test_derived_is_base(&memfile, CARBON_SORTED_MULTISET_ARRAY, false);
        test_derived_is_base(&memfile, CARBON_UNSORTED_SET_ARRAY, false);
        test_derived_is_base(&memfile, CARBON_SORTED_SET_ARRAY, false);

        test_derived_is_base(&memfile, CARBON_UNSORTED_MULTISET_COL_U8, true);
        test_derived_is_base(&memfile, CARBON_SORTED_MULTISET_COL_U8, false);
        test_derived_is_base(&memfile, CARBON_UNSORTED_SET_COL_U8, false);
        test_derived_is_base(&memfile, CARBON_SORTED_SET_COL_U8, false);

        test_derived_is_base(&memfile, CARBON_UNSORTED_MULTISET_COL_U16, true);
        test_derived_is_base(&memfile, CARBON_SORTED_MULTISET_COL_U16, false);
        test_derived_is_base(&memfile, CARBON_UNSORTED_SET_COL_U16, false);
        test_derived_is_base(&memfile, CARBON_SORTED_SET_COL_U16, false);

        test_derived_is_base(&memfile, CARBON_UNSORTED_MULTISET_COL_U32, true);
        test_derived_is_base(&memfile, CARBON_SORTED_MULTISET_COL_U32, false);
        test_derived_is_base(&memfile, CARBON_UNSORTED_SET_COL_U32, false);
        test_derived_is_base(&memfile, CARBON_SORTED_SET_COL_U32, false);

        test_derived_is_base(&memfile, CARBON_UNSORTED_MULTISET_COL_U64, true);
        test_derived_is_base(&memfile, CARBON_SORTED_MULTISET_COL_U64, false);
        test_derived_is_base(&memfile, CARBON_UNSORTED_SET_COL_U64, false);
        test_derived_is_base(&memfile, CARBON_SORTED_SET_COL_U64, false);

        test_derived_is_base(&memfile, CARBON_UNSORTED_MULTISET_COL_I8, true);
        test_derived_is_base(&memfile, CARBON_SORTED_MULTISET_COL_I8, false);
        test_derived_is_base(&memfile, CARBON_UNSORTED_SET_COL_I8, false);
        test_derived_is_base(&memfile, CARBON_SORTED_SET_COL_I8, false);

        test_derived_is_base(&memfile, CARBON_UNSORTED_MULTISET_COL_I16, true);
        test_derived_is_base(&memfile, CARBON_SORTED_MULTISET_COL_I16, false);
        test_derived_is_base(&memfile, CARBON_UNSORTED_SET_COL_I16, false);
        test_derived_is_base(&memfile, CARBON_SORTED_SET_COL_I16, false);

        test_derived_is_base(&memfile, CARBON_UNSORTED_MULTISET_COL_I32, true);
        test_derived_is_base(&memfile, CARBON_SORTED_MULTISET_COL_I32, false);
        test_derived_is_base(&memfile, CARBON_UNSORTED_SET_COL_I32, false);
        test_derived_is_base(&memfile, CARBON_SORTED_SET_COL_I32, false);

        test_derived_is_base(&memfile, CARBON_UNSORTED_MULTISET_COL_I64, true);
        test_derived_is_base(&memfile, CARBON_SORTED_MULTISET_COL_I64, false);
        test_derived_is_base(&memfile, CARBON_UNSORTED_SET_COL_I64, false);
        test_derived_is_base(&memfile, CARBON_SORTED_SET_COL_I64, false);

        test_derived_is_base(&memfile, CARBON_UNSORTED_MULTISET_COL_FLOAT, true);
        test_derived_is_base(&memfile, CARBON_SORTED_MULTISET_COL_FLOAT, false);
        test_derived_is_base(&memfile, CARBON_UNSORTED_SET_COL_FLOAT, false);
        test_derived_is_base(&memfile, CARBON_SORTED_SET_COL_FLOAT, false);

        test_derived_is_base(&memfile, CARBON_UNSORTED_MULTISET_COL_BOOLEAN, true);
        test_derived_is_base(&memfile, CARBON_SORTED_MULTISET_COL_BOOLEAN, false);
        test_derived_is_base(&memfile, CARBON_UNSORTED_SET_COL_BOOLEAN, false);
        test_derived_is_base(&memfile, CARBON_SORTED_SET_COL_BOOLEAN, false);

        drop_memfile(&memfile);
}

static void test_get_class_of_concrete_derived(memfile *file, derived_e concrete,
        abstract_type_class_e expected)
{
        abstract_type_class_e clazz;
        abstract_write_derived_type(file, concrete);
        memfile_seek_from_here(file, -1);
        ASSERT_TRUE(abstract_get_class(&clazz, file));
        ASSERT_EQ(clazz, expected);
}

TEST(TestAbstractTypeMarker, GetClassOfConcreteDerivedType) {

        memfile memfile;
        make_memfile(&memfile);

        test_get_class_of_concrete_derived(&memfile, CARBON_UNSORTED_MULTIMAP, TYPE_UNSORTED_MULTIMAP);
        test_get_class_of_concrete_derived(&memfile, CARBON_SORTED_MULTIMAP, TYPE_SORTED_MULTIMAP);
        test_get_class_of_concrete_derived(&memfile, CARBON_UNSORTED_MAP, TYPE_UNSORTED_MAP);
        test_get_class_of_concrete_derived(&memfile, CARBON_SORTED_MAP, TYPE_SORTED_MAP);

        test_get_class_of_concrete_derived(&memfile, CARBON_UNSORTED_MULTISET_ARRAY, TYPE_UNSORTED_MULTISET);
        test_get_class_of_concrete_derived(&memfile, CARBON_SORTED_MULTISET_ARRAY, TYPE_SORTED_MULTISET);
        test_get_class_of_concrete_derived(&memfile, CARBON_UNSORTED_SET_ARRAY, TYPE_UNSORTED_SET);
        test_get_class_of_concrete_derived(&memfile, CARBON_SORTED_SET_ARRAY, TYPE_SORTED_SET);

        test_get_class_of_concrete_derived(&memfile, CARBON_UNSORTED_MULTISET_COL_U8, TYPE_UNSORTED_MULTISET);
        test_get_class_of_concrete_derived(&memfile, CARBON_SORTED_MULTISET_COL_U8, TYPE_SORTED_MULTISET);
        test_get_class_of_concrete_derived(&memfile, CARBON_UNSORTED_SET_COL_U8, TYPE_UNSORTED_SET);
        test_get_class_of_concrete_derived(&memfile, CARBON_SORTED_SET_COL_U8, TYPE_SORTED_SET);

        test_get_class_of_concrete_derived(&memfile, CARBON_UNSORTED_MULTISET_COL_U16, TYPE_UNSORTED_MULTISET);
        test_get_class_of_concrete_derived(&memfile, CARBON_SORTED_MULTISET_COL_U16, TYPE_SORTED_MULTISET);
        test_get_class_of_concrete_derived(&memfile, CARBON_UNSORTED_SET_COL_U16, TYPE_UNSORTED_SET);
        test_get_class_of_concrete_derived(&memfile, CARBON_SORTED_SET_COL_U16, TYPE_SORTED_SET);

        test_get_class_of_concrete_derived(&memfile, CARBON_UNSORTED_MULTISET_COL_U32, TYPE_UNSORTED_MULTISET);
        test_get_class_of_concrete_derived(&memfile, CARBON_SORTED_MULTISET_COL_U32, TYPE_SORTED_MULTISET);
        test_get_class_of_concrete_derived(&memfile, CARBON_UNSORTED_SET_COL_U32, TYPE_UNSORTED_SET);
        test_get_class_of_concrete_derived(&memfile, CARBON_SORTED_SET_COL_U32, TYPE_SORTED_SET);

        test_get_class_of_concrete_derived(&memfile, CARBON_UNSORTED_MULTISET_COL_U64, TYPE_UNSORTED_MULTISET);
        test_get_class_of_concrete_derived(&memfile, CARBON_SORTED_MULTISET_COL_U64, TYPE_SORTED_MULTISET);
        test_get_class_of_concrete_derived(&memfile, CARBON_UNSORTED_SET_COL_U64, TYPE_UNSORTED_SET);
        test_get_class_of_concrete_derived(&memfile, CARBON_SORTED_SET_COL_U64, TYPE_SORTED_SET);

        test_get_class_of_concrete_derived(&memfile, CARBON_UNSORTED_MULTISET_COL_I8, TYPE_UNSORTED_MULTISET);
        test_get_class_of_concrete_derived(&memfile, CARBON_SORTED_MULTISET_COL_I8, TYPE_SORTED_MULTISET);
        test_get_class_of_concrete_derived(&memfile, CARBON_UNSORTED_SET_COL_I8, TYPE_UNSORTED_SET);
        test_get_class_of_concrete_derived(&memfile, CARBON_SORTED_SET_COL_I8, TYPE_SORTED_SET);

        test_get_class_of_concrete_derived(&memfile, CARBON_UNSORTED_MULTISET_COL_I16, TYPE_UNSORTED_MULTISET);
        test_get_class_of_concrete_derived(&memfile, CARBON_SORTED_MULTISET_COL_I16, TYPE_SORTED_MULTISET);
        test_get_class_of_concrete_derived(&memfile, CARBON_UNSORTED_SET_COL_I16, TYPE_UNSORTED_SET);
        test_get_class_of_concrete_derived(&memfile, CARBON_SORTED_SET_COL_I16, TYPE_SORTED_SET);

        test_get_class_of_concrete_derived(&memfile, CARBON_UNSORTED_MULTISET_COL_I32, TYPE_UNSORTED_MULTISET);
        test_get_class_of_concrete_derived(&memfile, CARBON_SORTED_MULTISET_COL_I32, TYPE_SORTED_MULTISET);
        test_get_class_of_concrete_derived(&memfile, CARBON_UNSORTED_SET_COL_I32, TYPE_UNSORTED_SET);
        test_get_class_of_concrete_derived(&memfile, CARBON_SORTED_SET_COL_I32, TYPE_SORTED_SET);

        test_get_class_of_concrete_derived(&memfile, CARBON_UNSORTED_MULTISET_COL_I64, TYPE_UNSORTED_MULTISET);
        test_get_class_of_concrete_derived(&memfile, CARBON_SORTED_MULTISET_COL_I64, TYPE_SORTED_MULTISET);
        test_get_class_of_concrete_derived(&memfile, CARBON_UNSORTED_SET_COL_I64, TYPE_UNSORTED_SET);
        test_get_class_of_concrete_derived(&memfile, CARBON_SORTED_SET_COL_I64, TYPE_SORTED_SET);

        test_get_class_of_concrete_derived(&memfile, CARBON_UNSORTED_MULTISET_COL_FLOAT, TYPE_UNSORTED_MULTISET);
        test_get_class_of_concrete_derived(&memfile, CARBON_SORTED_MULTISET_COL_FLOAT, TYPE_SORTED_MULTISET);
        test_get_class_of_concrete_derived(&memfile, CARBON_UNSORTED_SET_COL_FLOAT, TYPE_UNSORTED_SET);
        test_get_class_of_concrete_derived(&memfile, CARBON_SORTED_SET_COL_FLOAT, TYPE_SORTED_SET);

        test_get_class_of_concrete_derived(&memfile, CARBON_UNSORTED_MULTISET_COL_BOOLEAN, TYPE_UNSORTED_MULTISET);
        test_get_class_of_concrete_derived(&memfile, CARBON_SORTED_MULTISET_COL_BOOLEAN, TYPE_SORTED_MULTISET);
        test_get_class_of_concrete_derived(&memfile, CARBON_UNSORTED_SET_COL_BOOLEAN, TYPE_UNSORTED_SET);
        test_get_class_of_concrete_derived(&memfile, CARBON_SORTED_SET_COL_BOOLEAN, TYPE_SORTED_SET);

        drop_memfile(&memfile);
}

static void test_get_container_for_derived_type(memfile *memfile, derived_e derived,
                                                carbon_container_sub_type_e expected)
{
        carbon_container_sub_type_e sub_type;
        abstract_write_derived_type(memfile, derived);
        memfile_seek_from_here(memfile, -1);
        ASSERT_TRUE(abstract_get_container_subtype(&sub_type, memfile));
        ASSERT_EQ(sub_type, expected);
}

TEST(TestAbstractTypeMarker, GetContainerForDerivedType)
{
        memfile memfile;
        make_memfile(&memfile);

        /* abstract types for object containers */
        test_get_container_for_derived_type(&memfile, CARBON_UNSORTED_MULTIMAP, CARBON_CONTAINER_OBJECT);
        test_get_container_for_derived_type(&memfile, CARBON_SORTED_MULTIMAP, CARBON_CONTAINER_OBJECT);
        test_get_container_for_derived_type(&memfile, CARBON_UNSORTED_MAP, CARBON_CONTAINER_OBJECT);
        test_get_container_for_derived_type(&memfile, CARBON_SORTED_MAP, CARBON_CONTAINER_OBJECT);

        /* abstract types for array containers */
        test_get_container_for_derived_type(&memfile, CARBON_UNSORTED_MULTISET_ARRAY, CARBON_CONTAINER_ARRAY);
        test_get_container_for_derived_type(&memfile, CARBON_SORTED_MULTISET_ARRAY, CARBON_CONTAINER_ARRAY);
        test_get_container_for_derived_type(&memfile, CARBON_UNSORTED_SET_ARRAY, CARBON_CONTAINER_ARRAY);
        test_get_container_for_derived_type(&memfile, CARBON_SORTED_SET_ARRAY, CARBON_CONTAINER_ARRAY);

        /* abstract types for column-u8 containers */
        test_get_container_for_derived_type(&memfile, CARBON_UNSORTED_MULTISET_COL_U8, CARBON_CONTAINER_COLUMN_U8);
        test_get_container_for_derived_type(&memfile, CARBON_SORTED_MULTISET_COL_U8, CARBON_CONTAINER_COLUMN_U8);
        test_get_container_for_derived_type(&memfile, CARBON_UNSORTED_SET_COL_U8, CARBON_CONTAINER_COLUMN_U8);
        test_get_container_for_derived_type(&memfile, CARBON_SORTED_SET_COL_U8, CARBON_CONTAINER_COLUMN_U8);

        /* abstract types for column-u16 containers */
        test_get_container_for_derived_type(&memfile, CARBON_UNSORTED_MULTISET_COL_U16, CARBON_CONTAINER_COLUMN_U16);
        test_get_container_for_derived_type(&memfile, CARBON_SORTED_MULTISET_COL_U16, CARBON_CONTAINER_COLUMN_U16);
        test_get_container_for_derived_type(&memfile, CARBON_UNSORTED_SET_COL_U16, CARBON_CONTAINER_COLUMN_U16);
        test_get_container_for_derived_type(&memfile, CARBON_SORTED_SET_COL_U16, CARBON_CONTAINER_COLUMN_U16);

        /* abstract types for column-u32 containers */
        test_get_container_for_derived_type(&memfile, CARBON_UNSORTED_MULTISET_COL_U32, CARBON_CONTAINER_COLUMN_U32);
        test_get_container_for_derived_type(&memfile, CARBON_SORTED_MULTISET_COL_U32, CARBON_CONTAINER_COLUMN_U32);
        test_get_container_for_derived_type(&memfile, CARBON_UNSORTED_SET_COL_U32, CARBON_CONTAINER_COLUMN_U32);
        test_get_container_for_derived_type(&memfile, CARBON_SORTED_SET_COL_U32, CARBON_CONTAINER_COLUMN_U32);

        /* abstract types for column-u64 containers */
        test_get_container_for_derived_type(&memfile, CARBON_UNSORTED_MULTISET_COL_U64, CARBON_CONTAINER_COLUMN_U64);
        test_get_container_for_derived_type(&memfile, CARBON_SORTED_MULTISET_COL_U64, CARBON_CONTAINER_COLUMN_U64);
        test_get_container_for_derived_type(&memfile, CARBON_UNSORTED_SET_COL_U64, CARBON_CONTAINER_COLUMN_U64);
        test_get_container_for_derived_type(&memfile, CARBON_SORTED_SET_COL_U64, CARBON_CONTAINER_COLUMN_U64);

        /* abstract types for column-i8 containers */
        test_get_container_for_derived_type(&memfile, CARBON_UNSORTED_MULTISET_COL_I8, CARBON_CONTAINER_COLUMN_I8);
        test_get_container_for_derived_type(&memfile, CARBON_SORTED_MULTISET_COL_I8, CARBON_CONTAINER_COLUMN_I8);
        test_get_container_for_derived_type(&memfile, CARBON_UNSORTED_SET_COL_I8, CARBON_CONTAINER_COLUMN_I8);
        test_get_container_for_derived_type(&memfile, CARBON_SORTED_SET_COL_I8, CARBON_CONTAINER_COLUMN_I8);

        /* abstract types for column-i16 containers */
        test_get_container_for_derived_type(&memfile, CARBON_UNSORTED_MULTISET_COL_I16, CARBON_CONTAINER_COLUMN_I16);
        test_get_container_for_derived_type(&memfile, CARBON_SORTED_MULTISET_COL_I16, CARBON_CONTAINER_COLUMN_I16);
        test_get_container_for_derived_type(&memfile, CARBON_UNSORTED_SET_COL_I16, CARBON_CONTAINER_COLUMN_I16);
        test_get_container_for_derived_type(&memfile, CARBON_SORTED_SET_COL_I16, CARBON_CONTAINER_COLUMN_I16);

        /* abstract types for column-i32 containers */
        test_get_container_for_derived_type(&memfile, CARBON_UNSORTED_MULTISET_COL_I32, CARBON_CONTAINER_COLUMN_I32);
        test_get_container_for_derived_type(&memfile, CARBON_SORTED_MULTISET_COL_I32, CARBON_CONTAINER_COLUMN_I32);
        test_get_container_for_derived_type(&memfile, CARBON_UNSORTED_SET_COL_I32, CARBON_CONTAINER_COLUMN_I32);
        test_get_container_for_derived_type(&memfile, CARBON_SORTED_SET_COL_I32, CARBON_CONTAINER_COLUMN_I32);

        /* abstract types for column-i64 containers */
        test_get_container_for_derived_type(&memfile, CARBON_UNSORTED_MULTISET_COL_I64, CARBON_CONTAINER_COLUMN_I64);
        test_get_container_for_derived_type(&memfile, CARBON_SORTED_MULTISET_COL_I64, CARBON_CONTAINER_COLUMN_I64);
        test_get_container_for_derived_type(&memfile, CARBON_UNSORTED_SET_COL_I64, CARBON_CONTAINER_COLUMN_I64);
        test_get_container_for_derived_type(&memfile, CARBON_SORTED_SET_COL_I64, CARBON_CONTAINER_COLUMN_I64);

        /* abstract types for column-float containers */
        test_get_container_for_derived_type(&memfile, CARBON_UNSORTED_MULTISET_COL_FLOAT, CARBON_CONTAINER_COLUMN_FLOAT);
        test_get_container_for_derived_type(&memfile, CARBON_SORTED_MULTISET_COL_FLOAT, CARBON_CONTAINER_COLUMN_FLOAT);
        test_get_container_for_derived_type(&memfile, CARBON_UNSORTED_SET_COL_FLOAT, CARBON_CONTAINER_COLUMN_FLOAT);
        test_get_container_for_derived_type(&memfile, CARBON_SORTED_SET_COL_FLOAT, CARBON_CONTAINER_COLUMN_FLOAT);

        /* abstract types for column-boolean containers */
        test_get_container_for_derived_type(&memfile, CARBON_UNSORTED_MULTISET_COL_BOOLEAN, CARBON_CONTAINER_COLUMN_BOOLEAN);
        test_get_container_for_derived_type(&memfile, CARBON_SORTED_MULTISET_COL_BOOLEAN, CARBON_CONTAINER_COLUMN_BOOLEAN);
        test_get_container_for_derived_type(&memfile, CARBON_UNSORTED_SET_COL_BOOLEAN, CARBON_CONTAINER_COLUMN_BOOLEAN);
        test_get_container_for_derived_type(&memfile, CARBON_SORTED_SET_COL_BOOLEAN, CARBON_CONTAINER_COLUMN_BOOLEAN);

        drop_memfile(&memfile);
}

static void test_get_derive_from_list(carbon_list_container_e is, carbon_list_derivable_e should,
                                      derived_e expected)
{
        derived_e concrete;
        ASSERT_TRUE(abstract_derive_list_to(&concrete, is, should));
        ASSERT_EQ(concrete, expected);
}

TEST(TestAbstractTypeMarker, GetDeriveFromList)
{
        test_get_derive_from_list(CARBON_LIST_CONTAINER_ARRAY, CARBON_LIST_UNSORTED_MULTISET,
                                  CARBON_UNSORTED_MULTISET_ARRAY);
        test_get_derive_from_list(CARBON_LIST_CONTAINER_ARRAY, CARBON_LIST_SORTED_MULTISET,
                                  CARBON_SORTED_MULTISET_ARRAY);
        test_get_derive_from_list(CARBON_LIST_CONTAINER_ARRAY, CARBON_LIST_UNSORTED_SET,
                                  CARBON_UNSORTED_SET_ARRAY);
        test_get_derive_from_list(CARBON_LIST_CONTAINER_ARRAY, CARBON_LIST_SORTED_SET,
                                  CARBON_SORTED_SET_ARRAY);

        test_get_derive_from_list(CARBON_LIST_CONTAINER_COLUMN_U8, CARBON_LIST_UNSORTED_MULTISET,
                                  CARBON_UNSORTED_MULTISET_COL_U8);
        test_get_derive_from_list(CARBON_LIST_CONTAINER_COLUMN_U8, CARBON_LIST_SORTED_MULTISET,
                                  CARBON_SORTED_MULTISET_COL_U8);
        test_get_derive_from_list(CARBON_LIST_CONTAINER_COLUMN_U8, CARBON_LIST_UNSORTED_SET,
                                  CARBON_UNSORTED_SET_COL_U8);
        test_get_derive_from_list(CARBON_LIST_CONTAINER_COLUMN_U8, CARBON_LIST_SORTED_SET,
                                  CARBON_SORTED_SET_COL_U8);

        test_get_derive_from_list(CARBON_LIST_CONTAINER_COLUMN_U16, CARBON_LIST_UNSORTED_MULTISET,
                                  CARBON_UNSORTED_MULTISET_COL_U16);
        test_get_derive_from_list(CARBON_LIST_CONTAINER_COLUMN_U16, CARBON_LIST_SORTED_MULTISET,
                                  CARBON_SORTED_MULTISET_COL_U16);
        test_get_derive_from_list(CARBON_LIST_CONTAINER_COLUMN_U16, CARBON_LIST_UNSORTED_SET,
                                  CARBON_UNSORTED_SET_COL_U16);
        test_get_derive_from_list(CARBON_LIST_CONTAINER_COLUMN_U16, CARBON_LIST_SORTED_SET,
                                  CARBON_SORTED_SET_COL_U16);

        test_get_derive_from_list(CARBON_LIST_CONTAINER_COLUMN_U32, CARBON_LIST_UNSORTED_MULTISET,
                                  CARBON_UNSORTED_MULTISET_COL_U32);
        test_get_derive_from_list(CARBON_LIST_CONTAINER_COLUMN_U32, CARBON_LIST_SORTED_MULTISET,
                                  CARBON_SORTED_MULTISET_COL_U32);
        test_get_derive_from_list(CARBON_LIST_CONTAINER_COLUMN_U32, CARBON_LIST_UNSORTED_SET,
                                  CARBON_UNSORTED_SET_COL_U32);
        test_get_derive_from_list(CARBON_LIST_CONTAINER_COLUMN_U32, CARBON_LIST_SORTED_SET,
                                  CARBON_SORTED_SET_COL_U32);

        test_get_derive_from_list(CARBON_LIST_CONTAINER_COLUMN_U64, CARBON_LIST_UNSORTED_MULTISET,
                                  CARBON_UNSORTED_MULTISET_COL_U64);
        test_get_derive_from_list(CARBON_LIST_CONTAINER_COLUMN_U64, CARBON_LIST_SORTED_MULTISET,
                                  CARBON_SORTED_MULTISET_COL_U64);
        test_get_derive_from_list(CARBON_LIST_CONTAINER_COLUMN_U64, CARBON_LIST_UNSORTED_SET,
                                  CARBON_UNSORTED_SET_COL_U64);
        test_get_derive_from_list(CARBON_LIST_CONTAINER_COLUMN_U64, CARBON_LIST_SORTED_SET,
                                  CARBON_SORTED_SET_COL_U64);

        test_get_derive_from_list(CARBON_LIST_CONTAINER_COLUMN_I8, CARBON_LIST_UNSORTED_MULTISET,
                                  CARBON_UNSORTED_MULTISET_COL_I8);
        test_get_derive_from_list(CARBON_LIST_CONTAINER_COLUMN_I8, CARBON_LIST_SORTED_MULTISET,
                                  CARBON_SORTED_MULTISET_COL_I8);
        test_get_derive_from_list(CARBON_LIST_CONTAINER_COLUMN_I8, CARBON_LIST_UNSORTED_SET,
                                  CARBON_UNSORTED_SET_COL_I8);
        test_get_derive_from_list(CARBON_LIST_CONTAINER_COLUMN_I8, CARBON_LIST_SORTED_SET,
                                  CARBON_SORTED_SET_COL_I8);

        test_get_derive_from_list(CARBON_LIST_CONTAINER_COLUMN_I16, CARBON_LIST_UNSORTED_MULTISET,
                                  CARBON_UNSORTED_MULTISET_COL_I16);
        test_get_derive_from_list(CARBON_LIST_CONTAINER_COLUMN_I16, CARBON_LIST_SORTED_MULTISET,
                                  CARBON_SORTED_MULTISET_COL_I16);
        test_get_derive_from_list(CARBON_LIST_CONTAINER_COLUMN_I16, CARBON_LIST_UNSORTED_SET,
                                  CARBON_UNSORTED_SET_COL_I16);
        test_get_derive_from_list(CARBON_LIST_CONTAINER_COLUMN_I16, CARBON_LIST_SORTED_SET,
                                  CARBON_SORTED_SET_COL_I16);

        test_get_derive_from_list(CARBON_LIST_CONTAINER_COLUMN_I32, CARBON_LIST_UNSORTED_MULTISET,
                                  CARBON_UNSORTED_MULTISET_COL_I32);
        test_get_derive_from_list(CARBON_LIST_CONTAINER_COLUMN_I32, CARBON_LIST_SORTED_MULTISET,
                                  CARBON_SORTED_MULTISET_COL_I32);
        test_get_derive_from_list(CARBON_LIST_CONTAINER_COLUMN_I32, CARBON_LIST_UNSORTED_SET,
                                  CARBON_UNSORTED_SET_COL_I32);
        test_get_derive_from_list(CARBON_LIST_CONTAINER_COLUMN_I32, CARBON_LIST_SORTED_SET,
                                  CARBON_SORTED_SET_COL_I32);

        test_get_derive_from_list(CARBON_LIST_CONTAINER_COLUMN_I64, CARBON_LIST_UNSORTED_MULTISET,
                                  CARBON_UNSORTED_MULTISET_COL_I64);
        test_get_derive_from_list(CARBON_LIST_CONTAINER_COLUMN_I64, CARBON_LIST_SORTED_MULTISET,
                                  CARBON_SORTED_MULTISET_COL_I64);
        test_get_derive_from_list(CARBON_LIST_CONTAINER_COLUMN_I64, CARBON_LIST_UNSORTED_SET,
                                  CARBON_UNSORTED_SET_COL_I64);
        test_get_derive_from_list(CARBON_LIST_CONTAINER_COLUMN_I64, CARBON_LIST_SORTED_SET,
                                  CARBON_SORTED_SET_COL_I64);

        test_get_derive_from_list(CARBON_LIST_CONTAINER_COLUMN_FLOAT, CARBON_LIST_UNSORTED_MULTISET,
                                  CARBON_UNSORTED_MULTISET_COL_FLOAT);
        test_get_derive_from_list(CARBON_LIST_CONTAINER_COLUMN_FLOAT, CARBON_LIST_SORTED_MULTISET,
                                  CARBON_SORTED_MULTISET_COL_FLOAT);
        test_get_derive_from_list(CARBON_LIST_CONTAINER_COLUMN_FLOAT, CARBON_LIST_UNSORTED_SET,
                                  CARBON_UNSORTED_SET_COL_FLOAT);
        test_get_derive_from_list(CARBON_LIST_CONTAINER_COLUMN_FLOAT, CARBON_LIST_SORTED_SET,
                                  CARBON_SORTED_SET_COL_FLOAT);

        test_get_derive_from_list(CARBON_LIST_CONTAINER_COLUMN_BOOLEAN, CARBON_LIST_UNSORTED_MULTISET,
                                  CARBON_UNSORTED_MULTISET_COL_BOOLEAN);
        test_get_derive_from_list(CARBON_LIST_CONTAINER_COLUMN_BOOLEAN, CARBON_LIST_SORTED_MULTISET,
                                  CARBON_SORTED_MULTISET_COL_BOOLEAN);
        test_get_derive_from_list(CARBON_LIST_CONTAINER_COLUMN_BOOLEAN, CARBON_LIST_UNSORTED_SET,
                                  CARBON_UNSORTED_SET_COL_BOOLEAN);
        test_get_derive_from_list(CARBON_LIST_CONTAINER_COLUMN_BOOLEAN, CARBON_LIST_SORTED_SET,
                                  CARBON_SORTED_SET_COL_BOOLEAN);
}

static void test_get_derive_from_list(carbon_map_derivable_e should, derived_e expected)
{
        derived_e concrete;
        ASSERT_TRUE(abstract_derive_map_to(&concrete, should));
        ASSERT_EQ(concrete, expected);
}

TEST(TestAbstractTypeMarker, GetDeriveFromMAP)
{
        test_get_derive_from_list(CARBON_MAP_UNSORTED_MULTIMAP, CARBON_UNSORTED_MULTIMAP);
        test_get_derive_from_list(CARBON_MAP_SORTED_MULTIMAP, CARBON_SORTED_MULTIMAP);
        test_get_derive_from_list(CARBON_MAP_UNSORTED_MAP, CARBON_UNSORTED_MAP);
        test_get_derive_from_list(CARBON_MAP_SORTED_MAP, CARBON_SORTED_MAP);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}