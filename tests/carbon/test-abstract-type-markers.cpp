#include <gtest/gtest.h>
#include <karbonit/karbonit.h>

static void make_memfile(memfile *memfile) {
        memblock *memblock;
        MEMBLOCK_CREATE(&memblock, 1024);
        MEMFILE_OPEN(memfile, memblock, READ_WRITE);
}

static void drop_memfile(memfile *memfile) {
        MEMBLOCK_DROP(memfile->memblock);
}

TEST(TestAbstractTypeMarker, DetectBaseTypeByBase) {

        memfile memfile;
        make_memfile(&memfile);

        abstract_e type;

        /* object is base type */
        {
                abstract_write_base_type(&memfile, CONTAINER_OBJECT);
                MEMFILE_SEEK_FROM_HERE(&memfile, -1);
                u8 marker = MEMFILE_READ_BYTE(&memfile);
                type = abstract_type(marker);
                ASSERT_EQ(type, ABSTRACT_BASE);
        }

        /* array is base type */
        {
                abstract_write_base_type(&memfile, CONTAINER_ARRAY);
                MEMFILE_SEEK_FROM_HERE(&memfile, -1);
                u8 marker = MEMFILE_READ_BYTE(&memfile);
                type = abstract_type(marker);
                ASSERT_EQ(type, ABSTRACT_BASE);
        }

        /* column-... is base type */
        {
                abstract_write_base_type(&memfile, CONTAINER_COLUMN_U8);
                MEMFILE_SEEK_FROM_HERE(&memfile, -1);
                u8 marker = MEMFILE_READ_BYTE(&memfile);
                type = abstract_type(marker);
                ASSERT_EQ(type, ABSTRACT_BASE);

                abstract_write_base_type(&memfile, CONTAINER_COLUMN_U16);
                MEMFILE_SEEK_FROM_HERE(&memfile, -1);
                marker = MEMFILE_READ_BYTE(&memfile);
                type = abstract_type(marker);
                ASSERT_EQ(type, ABSTRACT_BASE);

                abstract_write_base_type(&memfile, CONTAINER_COLUMN_U32);
                MEMFILE_SEEK_FROM_HERE(&memfile, -1);
                marker = MEMFILE_READ_BYTE(&memfile);
                type = abstract_type(marker);
                ASSERT_EQ(type, ABSTRACT_BASE);

                abstract_write_base_type(&memfile, CONTAINER_COLUMN_U64);
                MEMFILE_SEEK_FROM_HERE(&memfile, -1);
                marker = MEMFILE_READ_BYTE(&memfile);
                type = abstract_type(marker);
                ASSERT_EQ(type, ABSTRACT_BASE);

                abstract_write_base_type(&memfile, CONTAINER_COLUMN_I8);
                MEMFILE_SEEK_FROM_HERE(&memfile, -1);
                marker = MEMFILE_READ_BYTE(&memfile);
                type = abstract_type(marker);
                ASSERT_EQ(type, ABSTRACT_BASE);

                abstract_write_base_type(&memfile, CONTAINER_COLUMN_I16);
                MEMFILE_SEEK_FROM_HERE(&memfile, -1);
                marker = MEMFILE_READ_BYTE(&memfile);
                type = abstract_type(marker);
                ASSERT_EQ(type, ABSTRACT_BASE);

                abstract_write_base_type(&memfile, CONTAINER_COLUMN_I32);
                MEMFILE_SEEK_FROM_HERE(&memfile, -1);
                marker = MEMFILE_READ_BYTE(&memfile);
                type = abstract_type(marker);
                ASSERT_EQ(type, ABSTRACT_BASE);

                abstract_write_base_type(&memfile, CONTAINER_COLUMN_I64);
                MEMFILE_SEEK_FROM_HERE(&memfile, -1);
                marker = MEMFILE_READ_BYTE(&memfile);
                type = abstract_type(marker);
                ASSERT_EQ(type, ABSTRACT_BASE);

                abstract_write_base_type(&memfile, CONTAINER_COLUMN_FLOAT);
                MEMFILE_SEEK_FROM_HERE(&memfile, -1);
                marker = MEMFILE_READ_BYTE(&memfile);
                type = abstract_type(marker);
                ASSERT_EQ(type, ABSTRACT_BASE);

                abstract_write_base_type(&memfile, CONTAINER_COLUMN_BOOLEAN);
                MEMFILE_SEEK_FROM_HERE(&memfile, -1);
                marker = MEMFILE_READ_BYTE(&memfile);
                type = abstract_type(marker);
                ASSERT_EQ(type, ABSTRACT_BASE);
        }
        drop_memfile(&memfile);
}

TEST(TestAbstractTypeMarker, DetectBaseTypeByDerivedType) {

        memfile memfile;
        make_memfile(&memfile);

        abstract_e type;

        /* UNSORTED_MULTIMAP is base type */
        {
                abstract_write_derived_type(&memfile, UNSORTED_MULTIMAP);
                MEMFILE_SEEK_FROM_HERE(&memfile, -1);
                u8 marker = MEMFILE_READ_BYTE(&memfile);
                type = abstract_type(marker);
                ASSERT_EQ(type, ABSTRACT_BASE);
        }

        /* UNSORTED_MULTISET_ARRAY is base type */
        {
                abstract_write_derived_type(&memfile, UNSORTED_MULTISET_ARRAY);
                MEMFILE_SEEK_FROM_HERE(&memfile, -1);
                u8 marker = MEMFILE_READ_BYTE(&memfile);
                type = abstract_type(marker);
                ASSERT_EQ(type, ABSTRACT_BASE);
        }

        /* UNSORTED_MULTISET_COL_U8 is base type */
        {
                abstract_write_derived_type(&memfile, UNSORTED_MULTISET_COL_U8);
                MEMFILE_SEEK_FROM_HERE(&memfile, -1);
                u8 marker = MEMFILE_READ_BYTE(&memfile);
                type = abstract_type(marker);
                ASSERT_EQ(type, ABSTRACT_BASE);
        }

        /* UNSORTED_MULTISET_COL_U16 is base type */
        {
                abstract_write_derived_type(&memfile, UNSORTED_MULTISET_COL_U16);
                MEMFILE_SEEK_FROM_HERE(&memfile, -1);
                u8 marker = MEMFILE_READ_BYTE(&memfile);
                type = abstract_type(marker);
                ASSERT_EQ(type, ABSTRACT_BASE);
        }

        /* UNSORTED_MULTISET_COL_U32 is base type */
        {
                abstract_write_derived_type(&memfile, UNSORTED_MULTISET_COL_U32);
                MEMFILE_SEEK_FROM_HERE(&memfile, -1);
                u8 marker = MEMFILE_READ_BYTE(&memfile);
                type = abstract_type(marker);
                ASSERT_EQ(type, ABSTRACT_BASE);
        }

        /* UNSORTED_MULTISET_COL_U64 is base type */
        {
                abstract_write_derived_type(&memfile, UNSORTED_MULTISET_COL_U64);
                MEMFILE_SEEK_FROM_HERE(&memfile, -1);
                u8 marker = MEMFILE_READ_BYTE(&memfile);
                type = abstract_type(marker);
                ASSERT_EQ(type, ABSTRACT_BASE);
        }

        /* UNSORTED_MULTISET_COL_I8 is base type */
        {
                abstract_write_derived_type(&memfile, UNSORTED_MULTISET_COL_I8);
                MEMFILE_SEEK_FROM_HERE(&memfile, -1);
                u8 marker = MEMFILE_READ_BYTE(&memfile);
                type = abstract_type(marker);
                ASSERT_EQ(type, ABSTRACT_BASE);
        }

        /* UNSORTED_MULTISET_COL_I16 is base type */
        {
                abstract_write_derived_type(&memfile, UNSORTED_MULTISET_COL_I16);
                MEMFILE_SEEK_FROM_HERE(&memfile, -1);
                u8 marker = MEMFILE_READ_BYTE(&memfile);
                type = abstract_type(marker);
                ASSERT_EQ(type, ABSTRACT_BASE);
        }

        /* UNSORTED_MULTISET_COL_I32 is base type */
        {
                abstract_write_derived_type(&memfile, UNSORTED_MULTISET_COL_I32);
                MEMFILE_SEEK_FROM_HERE(&memfile, -1);
                u8 marker = MEMFILE_READ_BYTE(&memfile);
                type = abstract_type(marker);
                ASSERT_EQ(type, ABSTRACT_BASE);
        }

        /* UNSORTED_MULTISET_COL_I32 is base type */
        {
                abstract_write_derived_type(&memfile, UNSORTED_MULTISET_COL_I32);
                MEMFILE_SEEK_FROM_HERE(&memfile, -1);
                u8 marker = MEMFILE_READ_BYTE(&memfile);
                type = abstract_type(marker);
                ASSERT_EQ(type, ABSTRACT_BASE);
        }

        /* UNSORTED_MULTISET_COL_I64 is base type */
        {
                abstract_write_derived_type(&memfile, UNSORTED_MULTISET_COL_I64);
                MEMFILE_SEEK_FROM_HERE(&memfile, -1);
                u8 marker = MEMFILE_READ_BYTE(&memfile);
                type = abstract_type(marker);
                ASSERT_EQ(type, ABSTRACT_BASE);
        }

        /* UNSORTED_MULTISET_COL_FLOAT is base type */
        {
                abstract_write_derived_type(&memfile, UNSORTED_MULTISET_COL_FLOAT);
                MEMFILE_SEEK_FROM_HERE(&memfile, -1);
                u8 marker = MEMFILE_READ_BYTE(&memfile);
                type = abstract_type(marker);
                ASSERT_EQ(type, ABSTRACT_BASE);
        }

        /* UNSORTED_MULTISET_COL_BOOLEAN is base type */
        {
                abstract_write_derived_type(&memfile, UNSORTED_MULTISET_COL_BOOLEAN);
                MEMFILE_SEEK_FROM_HERE(&memfile, -1);
                u8 marker = MEMFILE_READ_BYTE(&memfile);
                type = abstract_type(marker);
                ASSERT_EQ(type, ABSTRACT_BASE);
        }

        drop_memfile(&memfile);
}


static void test_derived_is_base(memfile *memfile, derived_e type, bool is_base)
{
        abstract_write_derived_type(memfile, type);
        MEMFILE_SEEK_FROM_HERE(memfile, -1);
        u8 marker = MEMFILE_READ_BYTE(memfile);
        bool result = abstract_is_base(marker);
        ASSERT_EQ(result, is_base);
}

TEST(TestAbstractTypeMarker, DetectNonBaseTypeByDerivedType) {

        memfile memfile;
        make_memfile(&memfile);

        test_derived_is_base(&memfile, UNSORTED_MULTIMAP, true);
        test_derived_is_base(&memfile, SORTED_MULTIMAP, false);
        test_derived_is_base(&memfile, UNSORTED_MAP, false);
        test_derived_is_base(&memfile, SORTED_MAP, false);

        test_derived_is_base(&memfile, UNSORTED_MULTISET_ARRAY, true);
        test_derived_is_base(&memfile, SORTED_MULTISET_ARRAY, false);
        test_derived_is_base(&memfile, UNSORTED_SET_ARRAY, false);
        test_derived_is_base(&memfile, SORTED_SET_ARRAY, false);

        test_derived_is_base(&memfile, UNSORTED_MULTISET_COL_U8, true);
        test_derived_is_base(&memfile, SORTED_MULTISET_COL_U8, false);
        test_derived_is_base(&memfile, UNSORTED_SET_COL_U8, false);
        test_derived_is_base(&memfile, SORTED_SET_COL_U8, false);

        test_derived_is_base(&memfile, UNSORTED_MULTISET_COL_U16, true);
        test_derived_is_base(&memfile, SORTED_MULTISET_COL_U16, false);
        test_derived_is_base(&memfile, UNSORTED_SET_COL_U16, false);
        test_derived_is_base(&memfile, SORTED_SET_COL_U16, false);

        test_derived_is_base(&memfile, UNSORTED_MULTISET_COL_U32, true);
        test_derived_is_base(&memfile, SORTED_MULTISET_COL_U32, false);
        test_derived_is_base(&memfile, UNSORTED_SET_COL_U32, false);
        test_derived_is_base(&memfile, SORTED_SET_COL_U32, false);

        test_derived_is_base(&memfile, UNSORTED_MULTISET_COL_U64, true);
        test_derived_is_base(&memfile, SORTED_MULTISET_COL_U64, false);
        test_derived_is_base(&memfile, UNSORTED_SET_COL_U64, false);
        test_derived_is_base(&memfile, SORTED_SET_COL_U64, false);

        test_derived_is_base(&memfile, UNSORTED_MULTISET_COL_I8, true);
        test_derived_is_base(&memfile, SORTED_MULTISET_COL_I8, false);
        test_derived_is_base(&memfile, UNSORTED_SET_COL_I8, false);
        test_derived_is_base(&memfile, SORTED_SET_COL_I8, false);

        test_derived_is_base(&memfile, UNSORTED_MULTISET_COL_I16, true);
        test_derived_is_base(&memfile, SORTED_MULTISET_COL_I16, false);
        test_derived_is_base(&memfile, UNSORTED_SET_COL_I16, false);
        test_derived_is_base(&memfile, SORTED_SET_COL_I16, false);

        test_derived_is_base(&memfile, UNSORTED_MULTISET_COL_I32, true);
        test_derived_is_base(&memfile, SORTED_MULTISET_COL_I32, false);
        test_derived_is_base(&memfile, UNSORTED_SET_COL_I32, false);
        test_derived_is_base(&memfile, SORTED_SET_COL_I32, false);

        test_derived_is_base(&memfile, UNSORTED_MULTISET_COL_I64, true);
        test_derived_is_base(&memfile, SORTED_MULTISET_COL_I64, false);
        test_derived_is_base(&memfile, UNSORTED_SET_COL_I64, false);
        test_derived_is_base(&memfile, SORTED_SET_COL_I64, false);

        test_derived_is_base(&memfile, UNSORTED_MULTISET_COL_FLOAT, true);
        test_derived_is_base(&memfile, SORTED_MULTISET_COL_FLOAT, false);
        test_derived_is_base(&memfile, UNSORTED_SET_COL_FLOAT, false);
        test_derived_is_base(&memfile, SORTED_SET_COL_FLOAT, false);

        test_derived_is_base(&memfile, UNSORTED_MULTISET_COL_BOOLEAN, true);
        test_derived_is_base(&memfile, SORTED_MULTISET_COL_BOOLEAN, false);
        test_derived_is_base(&memfile, UNSORTED_SET_COL_BOOLEAN, false);
        test_derived_is_base(&memfile, SORTED_SET_COL_BOOLEAN, false);

        drop_memfile(&memfile);
}

static void test_get_class_of_concrete_derived(memfile *file, derived_e concrete,
        abstract_type_class_e expected)
{
        abstract_type_class_e clazz;
        abstract_write_derived_type(file, concrete);
        MEMFILE_SEEK_FROM_HERE(file, -1);
        u8 marker = MEMFILE_READ_BYTE(file);
        clazz = abstract_get_class(marker);
        ASSERT_EQ(clazz, expected);
}

TEST(TestAbstractTypeMarker, GetClassOfConcreteDerivedType) {

        memfile memfile;
        make_memfile(&memfile);

        test_get_class_of_concrete_derived(&memfile, UNSORTED_MULTIMAP, TYPE_UNSORTED_MULTIMAP);
        test_get_class_of_concrete_derived(&memfile, SORTED_MULTIMAP, TYPE_SORTED_MULTIMAP);
        test_get_class_of_concrete_derived(&memfile, UNSORTED_MAP, TYPE_UNSORTED_MAP);
        test_get_class_of_concrete_derived(&memfile, SORTED_MAP, TYPE_SORTED_MAP);

        test_get_class_of_concrete_derived(&memfile, UNSORTED_MULTISET_ARRAY, TYPE_UNSORTED_MULTISET);
        test_get_class_of_concrete_derived(&memfile, SORTED_MULTISET_ARRAY, TYPE_SORTED_MULTISET);
        test_get_class_of_concrete_derived(&memfile, UNSORTED_SET_ARRAY, TYPE_UNSORTED_SET);
        test_get_class_of_concrete_derived(&memfile, SORTED_SET_ARRAY, TYPE_SORTED_SET);

        test_get_class_of_concrete_derived(&memfile, UNSORTED_MULTISET_COL_U8, TYPE_UNSORTED_MULTISET);
        test_get_class_of_concrete_derived(&memfile, SORTED_MULTISET_COL_U8, TYPE_SORTED_MULTISET);
        test_get_class_of_concrete_derived(&memfile, UNSORTED_SET_COL_U8, TYPE_UNSORTED_SET);
        test_get_class_of_concrete_derived(&memfile, SORTED_SET_COL_U8, TYPE_SORTED_SET);

        test_get_class_of_concrete_derived(&memfile, UNSORTED_MULTISET_COL_U16, TYPE_UNSORTED_MULTISET);
        test_get_class_of_concrete_derived(&memfile, SORTED_MULTISET_COL_U16, TYPE_SORTED_MULTISET);
        test_get_class_of_concrete_derived(&memfile, UNSORTED_SET_COL_U16, TYPE_UNSORTED_SET);
        test_get_class_of_concrete_derived(&memfile, SORTED_SET_COL_U16, TYPE_SORTED_SET);

        test_get_class_of_concrete_derived(&memfile, UNSORTED_MULTISET_COL_U32, TYPE_UNSORTED_MULTISET);
        test_get_class_of_concrete_derived(&memfile, SORTED_MULTISET_COL_U32, TYPE_SORTED_MULTISET);
        test_get_class_of_concrete_derived(&memfile, UNSORTED_SET_COL_U32, TYPE_UNSORTED_SET);
        test_get_class_of_concrete_derived(&memfile, SORTED_SET_COL_U32, TYPE_SORTED_SET);

        test_get_class_of_concrete_derived(&memfile, UNSORTED_MULTISET_COL_U64, TYPE_UNSORTED_MULTISET);
        test_get_class_of_concrete_derived(&memfile, SORTED_MULTISET_COL_U64, TYPE_SORTED_MULTISET);
        test_get_class_of_concrete_derived(&memfile, UNSORTED_SET_COL_U64, TYPE_UNSORTED_SET);
        test_get_class_of_concrete_derived(&memfile, SORTED_SET_COL_U64, TYPE_SORTED_SET);

        test_get_class_of_concrete_derived(&memfile, UNSORTED_MULTISET_COL_I8, TYPE_UNSORTED_MULTISET);
        test_get_class_of_concrete_derived(&memfile, SORTED_MULTISET_COL_I8, TYPE_SORTED_MULTISET);
        test_get_class_of_concrete_derived(&memfile, UNSORTED_SET_COL_I8, TYPE_UNSORTED_SET);
        test_get_class_of_concrete_derived(&memfile, SORTED_SET_COL_I8, TYPE_SORTED_SET);

        test_get_class_of_concrete_derived(&memfile, UNSORTED_MULTISET_COL_I16, TYPE_UNSORTED_MULTISET);
        test_get_class_of_concrete_derived(&memfile, SORTED_MULTISET_COL_I16, TYPE_SORTED_MULTISET);
        test_get_class_of_concrete_derived(&memfile, UNSORTED_SET_COL_I16, TYPE_UNSORTED_SET);
        test_get_class_of_concrete_derived(&memfile, SORTED_SET_COL_I16, TYPE_SORTED_SET);

        test_get_class_of_concrete_derived(&memfile, UNSORTED_MULTISET_COL_I32, TYPE_UNSORTED_MULTISET);
        test_get_class_of_concrete_derived(&memfile, SORTED_MULTISET_COL_I32, TYPE_SORTED_MULTISET);
        test_get_class_of_concrete_derived(&memfile, UNSORTED_SET_COL_I32, TYPE_UNSORTED_SET);
        test_get_class_of_concrete_derived(&memfile, SORTED_SET_COL_I32, TYPE_SORTED_SET);

        test_get_class_of_concrete_derived(&memfile, UNSORTED_MULTISET_COL_I64, TYPE_UNSORTED_MULTISET);
        test_get_class_of_concrete_derived(&memfile, SORTED_MULTISET_COL_I64, TYPE_SORTED_MULTISET);
        test_get_class_of_concrete_derived(&memfile, UNSORTED_SET_COL_I64, TYPE_UNSORTED_SET);
        test_get_class_of_concrete_derived(&memfile, SORTED_SET_COL_I64, TYPE_SORTED_SET);

        test_get_class_of_concrete_derived(&memfile, UNSORTED_MULTISET_COL_FLOAT, TYPE_UNSORTED_MULTISET);
        test_get_class_of_concrete_derived(&memfile, SORTED_MULTISET_COL_FLOAT, TYPE_SORTED_MULTISET);
        test_get_class_of_concrete_derived(&memfile, UNSORTED_SET_COL_FLOAT, TYPE_UNSORTED_SET);
        test_get_class_of_concrete_derived(&memfile, SORTED_SET_COL_FLOAT, TYPE_SORTED_SET);

        test_get_class_of_concrete_derived(&memfile, UNSORTED_MULTISET_COL_BOOLEAN, TYPE_UNSORTED_MULTISET);
        test_get_class_of_concrete_derived(&memfile, SORTED_MULTISET_COL_BOOLEAN, TYPE_SORTED_MULTISET);
        test_get_class_of_concrete_derived(&memfile, UNSORTED_SET_COL_BOOLEAN, TYPE_UNSORTED_SET);
        test_get_class_of_concrete_derived(&memfile, SORTED_SET_COL_BOOLEAN, TYPE_SORTED_SET);

        drop_memfile(&memfile);
}

static void test_get_container_for_derived_type(memfile *memfile, derived_e derived,
                                                sub_type_e expected)
{
        abstract_write_derived_type(memfile, derived);
        MEMFILE_SEEK_FROM_HERE(memfile, -1);
        u8 marker = MEMFILE_READ_BYTE(memfile);
        sub_type_e sub_type = abstract_get_container_subtype(marker);
        ASSERT_EQ(sub_type, expected);
}

TEST(TestAbstractTypeMarker, GetContainerForDerivedType)
{
        memfile memfile;
        make_memfile(&memfile);

        /* abstract types for object containers */
        test_get_container_for_derived_type(&memfile, UNSORTED_MULTIMAP, CONTAINER_OBJECT);
        test_get_container_for_derived_type(&memfile, SORTED_MULTIMAP, CONTAINER_OBJECT);
        test_get_container_for_derived_type(&memfile, UNSORTED_MAP, CONTAINER_OBJECT);
        test_get_container_for_derived_type(&memfile, SORTED_MAP, CONTAINER_OBJECT);

        /* abstract types for array containers */
        test_get_container_for_derived_type(&memfile, UNSORTED_MULTISET_ARRAY, CONTAINER_ARRAY);
        test_get_container_for_derived_type(&memfile, SORTED_MULTISET_ARRAY, CONTAINER_ARRAY);
        test_get_container_for_derived_type(&memfile, UNSORTED_SET_ARRAY, CONTAINER_ARRAY);
        test_get_container_for_derived_type(&memfile, SORTED_SET_ARRAY, CONTAINER_ARRAY);

        /* abstract types for column-u8 containers */
        test_get_container_for_derived_type(&memfile, UNSORTED_MULTISET_COL_U8, CONTAINER_COLUMN_U8);
        test_get_container_for_derived_type(&memfile, SORTED_MULTISET_COL_U8, CONTAINER_COLUMN_U8);
        test_get_container_for_derived_type(&memfile, UNSORTED_SET_COL_U8, CONTAINER_COLUMN_U8);
        test_get_container_for_derived_type(&memfile, SORTED_SET_COL_U8, CONTAINER_COLUMN_U8);

        /* abstract types for column-u16 containers */
        test_get_container_for_derived_type(&memfile, UNSORTED_MULTISET_COL_U16, CONTAINER_COLUMN_U16);
        test_get_container_for_derived_type(&memfile, SORTED_MULTISET_COL_U16, CONTAINER_COLUMN_U16);
        test_get_container_for_derived_type(&memfile, UNSORTED_SET_COL_U16, CONTAINER_COLUMN_U16);
        test_get_container_for_derived_type(&memfile, SORTED_SET_COL_U16, CONTAINER_COLUMN_U16);

        /* abstract types for column-u32 containers */
        test_get_container_for_derived_type(&memfile, UNSORTED_MULTISET_COL_U32, CONTAINER_COLUMN_U32);
        test_get_container_for_derived_type(&memfile, SORTED_MULTISET_COL_U32, CONTAINER_COLUMN_U32);
        test_get_container_for_derived_type(&memfile, UNSORTED_SET_COL_U32, CONTAINER_COLUMN_U32);
        test_get_container_for_derived_type(&memfile, SORTED_SET_COL_U32, CONTAINER_COLUMN_U32);

        /* abstract types for column-u64 containers */
        test_get_container_for_derived_type(&memfile, UNSORTED_MULTISET_COL_U64, CONTAINER_COLUMN_U64);
        test_get_container_for_derived_type(&memfile, SORTED_MULTISET_COL_U64, CONTAINER_COLUMN_U64);
        test_get_container_for_derived_type(&memfile, UNSORTED_SET_COL_U64, CONTAINER_COLUMN_U64);
        test_get_container_for_derived_type(&memfile, SORTED_SET_COL_U64, CONTAINER_COLUMN_U64);

        /* abstract types for column-i8 containers */
        test_get_container_for_derived_type(&memfile, UNSORTED_MULTISET_COL_I8, CONTAINER_COLUMN_I8);
        test_get_container_for_derived_type(&memfile, SORTED_MULTISET_COL_I8, CONTAINER_COLUMN_I8);
        test_get_container_for_derived_type(&memfile, UNSORTED_SET_COL_I8, CONTAINER_COLUMN_I8);
        test_get_container_for_derived_type(&memfile, SORTED_SET_COL_I8, CONTAINER_COLUMN_I8);

        /* abstract types for column-i16 containers */
        test_get_container_for_derived_type(&memfile, UNSORTED_MULTISET_COL_I16, CONTAINER_COLUMN_I16);
        test_get_container_for_derived_type(&memfile, SORTED_MULTISET_COL_I16, CONTAINER_COLUMN_I16);
        test_get_container_for_derived_type(&memfile, UNSORTED_SET_COL_I16, CONTAINER_COLUMN_I16);
        test_get_container_for_derived_type(&memfile, SORTED_SET_COL_I16, CONTAINER_COLUMN_I16);

        /* abstract types for column-i32 containers */
        test_get_container_for_derived_type(&memfile, UNSORTED_MULTISET_COL_I32, CONTAINER_COLUMN_I32);
        test_get_container_for_derived_type(&memfile, SORTED_MULTISET_COL_I32, CONTAINER_COLUMN_I32);
        test_get_container_for_derived_type(&memfile, UNSORTED_SET_COL_I32, CONTAINER_COLUMN_I32);
        test_get_container_for_derived_type(&memfile, SORTED_SET_COL_I32, CONTAINER_COLUMN_I32);

        /* abstract types for column-i64 containers */
        test_get_container_for_derived_type(&memfile, UNSORTED_MULTISET_COL_I64, CONTAINER_COLUMN_I64);
        test_get_container_for_derived_type(&memfile, SORTED_MULTISET_COL_I64, CONTAINER_COLUMN_I64);
        test_get_container_for_derived_type(&memfile, UNSORTED_SET_COL_I64, CONTAINER_COLUMN_I64);
        test_get_container_for_derived_type(&memfile, SORTED_SET_COL_I64, CONTAINER_COLUMN_I64);

        /* abstract types for column-float containers */
        test_get_container_for_derived_type(&memfile, UNSORTED_MULTISET_COL_FLOAT, CONTAINER_COLUMN_FLOAT);
        test_get_container_for_derived_type(&memfile, SORTED_MULTISET_COL_FLOAT, CONTAINER_COLUMN_FLOAT);
        test_get_container_for_derived_type(&memfile, UNSORTED_SET_COL_FLOAT, CONTAINER_COLUMN_FLOAT);
        test_get_container_for_derived_type(&memfile, SORTED_SET_COL_FLOAT, CONTAINER_COLUMN_FLOAT);

        /* abstract types for column-boolean containers */
        test_get_container_for_derived_type(&memfile, UNSORTED_MULTISET_COL_BOOLEAN, CONTAINER_COLUMN_BOOLEAN);
        test_get_container_for_derived_type(&memfile, SORTED_MULTISET_COL_BOOLEAN, CONTAINER_COLUMN_BOOLEAN);
        test_get_container_for_derived_type(&memfile, UNSORTED_SET_COL_BOOLEAN, CONTAINER_COLUMN_BOOLEAN);
        test_get_container_for_derived_type(&memfile, SORTED_SET_COL_BOOLEAN, CONTAINER_COLUMN_BOOLEAN);

        drop_memfile(&memfile);
}

static void test_get_derive_from_list(list_container_e is, list_type_e should,
                                      derived_e expected)
{
        derived_e concrete = abstract_derive_list_to(is, should);
        ASSERT_EQ(concrete, expected);
}

TEST(TestAbstractTypeMarker, GetDeriveFromList)
{
        test_get_derive_from_list(LIST_ARRAY, LIST_UNSORTED_MULTISET,
                                  UNSORTED_MULTISET_ARRAY);
        test_get_derive_from_list(LIST_ARRAY, LIST_SORTED_MULTISET,
                                  SORTED_MULTISET_ARRAY);
        test_get_derive_from_list(LIST_ARRAY, LIST_UNSORTED_SET,
                                  UNSORTED_SET_ARRAY);
        test_get_derive_from_list(LIST_ARRAY, LIST_SORTED_SET,
                                  SORTED_SET_ARRAY);

        test_get_derive_from_list(LIST_COLUMN_U8, LIST_UNSORTED_MULTISET,
                                  UNSORTED_MULTISET_COL_U8);
        test_get_derive_from_list(LIST_COLUMN_U8, LIST_SORTED_MULTISET,
                                  SORTED_MULTISET_COL_U8);
        test_get_derive_from_list(LIST_COLUMN_U8, LIST_UNSORTED_SET,
                                  UNSORTED_SET_COL_U8);
        test_get_derive_from_list(LIST_COLUMN_U8, LIST_SORTED_SET,
                                  SORTED_SET_COL_U8);

        test_get_derive_from_list(LIST_COLUMN_U16, LIST_UNSORTED_MULTISET,
                                  UNSORTED_MULTISET_COL_U16);
        test_get_derive_from_list(LIST_COLUMN_U16, LIST_SORTED_MULTISET,
                                  SORTED_MULTISET_COL_U16);
        test_get_derive_from_list(LIST_COLUMN_U16, LIST_UNSORTED_SET,
                                  UNSORTED_SET_COL_U16);
        test_get_derive_from_list(LIST_COLUMN_U16, LIST_SORTED_SET,
                                  SORTED_SET_COL_U16);

        test_get_derive_from_list(LIST_COLUMN_U32, LIST_UNSORTED_MULTISET,
                                  UNSORTED_MULTISET_COL_U32);
        test_get_derive_from_list(LIST_COLUMN_U32, LIST_SORTED_MULTISET,
                                  SORTED_MULTISET_COL_U32);
        test_get_derive_from_list(LIST_COLUMN_U32, LIST_UNSORTED_SET,
                                  UNSORTED_SET_COL_U32);
        test_get_derive_from_list(LIST_COLUMN_U32, LIST_SORTED_SET,
                                  SORTED_SET_COL_U32);

        test_get_derive_from_list(LIST_COLUMN_U64, LIST_UNSORTED_MULTISET,
                                  UNSORTED_MULTISET_COL_U64);
        test_get_derive_from_list(LIST_COLUMN_U64, LIST_SORTED_MULTISET,
                                  SORTED_MULTISET_COL_U64);
        test_get_derive_from_list(LIST_COLUMN_U64, LIST_UNSORTED_SET,
                                  UNSORTED_SET_COL_U64);
        test_get_derive_from_list(LIST_COLUMN_U64, LIST_SORTED_SET,
                                  SORTED_SET_COL_U64);

        test_get_derive_from_list(LIST_COLUMN_I8, LIST_UNSORTED_MULTISET,
                                  UNSORTED_MULTISET_COL_I8);
        test_get_derive_from_list(LIST_COLUMN_I8, LIST_SORTED_MULTISET,
                                  SORTED_MULTISET_COL_I8);
        test_get_derive_from_list(LIST_COLUMN_I8, LIST_UNSORTED_SET,
                                  UNSORTED_SET_COL_I8);
        test_get_derive_from_list(LIST_COLUMN_I8, LIST_SORTED_SET,
                                  SORTED_SET_COL_I8);

        test_get_derive_from_list(LIST_COLUMN_I16, LIST_UNSORTED_MULTISET,
                                  UNSORTED_MULTISET_COL_I16);
        test_get_derive_from_list(LIST_COLUMN_I16, LIST_SORTED_MULTISET,
                                  SORTED_MULTISET_COL_I16);
        test_get_derive_from_list(LIST_COLUMN_I16, LIST_UNSORTED_SET,
                                  UNSORTED_SET_COL_I16);
        test_get_derive_from_list(LIST_COLUMN_I16, LIST_SORTED_SET,
                                  SORTED_SET_COL_I16);

        test_get_derive_from_list(LIST_COLUMN_I32, LIST_UNSORTED_MULTISET,
                                  UNSORTED_MULTISET_COL_I32);
        test_get_derive_from_list(LIST_COLUMN_I32, LIST_SORTED_MULTISET,
                                  SORTED_MULTISET_COL_I32);
        test_get_derive_from_list(LIST_COLUMN_I32, LIST_UNSORTED_SET,
                                  UNSORTED_SET_COL_I32);
        test_get_derive_from_list(LIST_COLUMN_I32, LIST_SORTED_SET,
                                  SORTED_SET_COL_I32);

        test_get_derive_from_list(LIST_COLUMN_I64, LIST_UNSORTED_MULTISET,
                                  UNSORTED_MULTISET_COL_I64);
        test_get_derive_from_list(LIST_COLUMN_I64, LIST_SORTED_MULTISET,
                                  SORTED_MULTISET_COL_I64);
        test_get_derive_from_list(LIST_COLUMN_I64, LIST_UNSORTED_SET,
                                  UNSORTED_SET_COL_I64);
        test_get_derive_from_list(LIST_COLUMN_I64, LIST_SORTED_SET,
                                  SORTED_SET_COL_I64);

        test_get_derive_from_list(LIST_COLUMN_FLOAT, LIST_UNSORTED_MULTISET,
                                  UNSORTED_MULTISET_COL_FLOAT);
        test_get_derive_from_list(LIST_COLUMN_FLOAT, LIST_SORTED_MULTISET,
                                  SORTED_MULTISET_COL_FLOAT);
        test_get_derive_from_list(LIST_COLUMN_FLOAT, LIST_UNSORTED_SET,
                                  UNSORTED_SET_COL_FLOAT);
        test_get_derive_from_list(LIST_COLUMN_FLOAT, LIST_SORTED_SET,
                                  SORTED_SET_COL_FLOAT);

        test_get_derive_from_list(LIST_COLUMN_BOOLEAN, LIST_UNSORTED_MULTISET,
                                  UNSORTED_MULTISET_COL_BOOLEAN);
        test_get_derive_from_list(LIST_COLUMN_BOOLEAN, LIST_SORTED_MULTISET,
                                  SORTED_MULTISET_COL_BOOLEAN);
        test_get_derive_from_list(LIST_COLUMN_BOOLEAN, LIST_UNSORTED_SET,
                                  UNSORTED_SET_COL_BOOLEAN);
        test_get_derive_from_list(LIST_COLUMN_BOOLEAN, LIST_SORTED_SET,
                                  SORTED_SET_COL_BOOLEAN);
}

static void test_get_derive_from_list(map_type_e should, derived_e expected)
{
        derived_e concrete = abstract_derive_map_to(should);
        ASSERT_EQ(concrete, expected);
}

TEST(TestAbstractTypeMarker, GetDeriveFromMAP)
{
        test_get_derive_from_list(MAP_UNSORTED_MULTIMAP, UNSORTED_MULTIMAP);
        test_get_derive_from_list(MAP_SORTED_MULTIMAP, SORTED_MULTIMAP);
        test_get_derive_from_list(MAP_UNSORTED_MAP, UNSORTED_MAP);
        test_get_derive_from_list(MAP_SORTED_MAP, SORTED_MAP);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}