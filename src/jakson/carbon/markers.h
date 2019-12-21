/**
 * markers - encoding for marker-based format
 * see carbonspec.org/format-specs/format-overview/marker-format.html
 *
 * Copyright 2019 Marcus Pinnecke
 */

#ifndef HAD_MARKERS_H
#define HAD_MARKERS_H

#include <jakson/stdinc.h>

#ifdef __cplusplus
extern "C" {
#endif

/** data type marker */
#define MNULL                        'n'
#define MTRUE                        't'
#define MFALSE                       'f'
#define MSTRING                      's'
#define MU8                          'c'
#define MU16                         'd'
#define MU32                         'i'
#define MU64                         'l'
#define MI8                          'C'
#define MI16                         'D'
#define MI32                         'I'
#define MI64                         'L'
#define MFLOAT                       'r'
#define MBINARY                      'b'
#define MCUSTOM_BINARY               'x'

/** container marker */
#define MOBJECT_BEGIN                '{'
#define MOBJECT_END                  '}'
#define MARRAY_BEGIN                 '['
#define MARRAY_END                   ']'
#define MCOLUMN_U8                   '1'
#define MCOLUMN_U16                  '2'
#define MCOLUMN_U32                  '3'
#define MCOLUMN_U64                  '4'
#define MCOLUMN_I8                   '5'
#define MCOLUMN_I16                  '6'
#define MCOLUMN_I32                  '7'
#define MCOLUMN_I64                  '8'
#define MCOLUMN_FLOAT                'R'
#define MCOLUMN_BOOLEAN              'B'

/** record identifier marker */
#define MNOKEY                       '?'
#define MAUTOKEY                     '*'
#define MUKEY                        '+'
#define MIKEY                        '-'
#define MSKEY                        '!'

/** abstract types for object containers */
#define MUNSORTED_MULTIMAP           MOBJECT_BEGIN
#define MSORTED_MULTIMAP             '~'
#define MUNSORTED_MAP                ':'
#define MSORTED_MAP                  '#'

/** abstract types for array containers */
#define MUNSORTED_MULTISET_ARR       MARRAY_BEGIN
#define MSORTED_MULTISET_ARR         '<'
#define MUNSORTED_SET_ARR            '/'
#define MSORTED_SET_ARR              '='

/** abstract types for column-u8 containers */
#define MUNSORTED_MULTISET_U8        MCOLUMN_U8
#define MSORTED_MULTISET_U8          0x01 /** SOH */
#define MUNSORTED_SET_U8             0x02 /** STX */
#define MSORTED_SET_U8               0x03 /** ETX */

/** abstract types for column-u16 containers */
#define MUNSORTED_MULTISET_U16       MCOLUMN_U16
#define MSORTED_MULTISET_U16         0x05 /** ENQ */
#define MUNSORTED_SET_U16            0x06 /** ACK */
#define MSORTED_SET_U16              0x07 /** BEL */

/** abstract types for column-u32 containers */
#define MUNSORTED_MULTISET_U32       MCOLUMN_U32
#define MSORTED_MULTISET_U32         0x09 /** TAB */
#define MUNSORTED_SET_U32            0x0A /** LF */
#define MSORTED_SET_U32              0x0B /** VT */

/** abstract types for column-u64 containers */
#define MUNSORTED_MULTISET_U64       MCOLUMN_U64
#define MSORTED_MULTISET_U64         0x0D /** CR */
#define MUNSORTED_SET_U64            0x0E /** SO */
#define MSORTED_SET_U64              0x0F /** SI */

/** abstract types for column-i8 containers */
#define MUNSORTED_MULTISET_I8        MCOLUMN_I8
#define MSORTED_MULTISET_I8          0x11 /** DC1 */
#define MUNSORTED_SET_I8             0x12 /** DC2 */
#define MSORTED_SET_I8               0x13 /** DC3 */

/** abstract types for column-i16 containers */
#define MUNSORTED_MULTISET_I16       MCOLUMN_I16
#define MSORTED_MULTISET_I16         0x15 /** NAK */
#define MUNSORTED_SET_I16            0x16 /** SYN */
#define MSORTED_SET_I16              0x17 /** ETB */

/** abstract types for column-i32 containers */
#define MUNSORTED_MULTISET_I32       MCOLUMN_I32
#define MSORTED_MULTISET_I32         0x19 /** EM */
#define MUNSORTED_SET_I32            0x1A /** SUB */
#define MSORTED_SET_I32              0x1B /** ESC */

/** abstract types for column-i64 containers */
#define MUNSORTED_MULTISET_I64       MCOLUMN_I64
#define MSORTED_MULTISET_I64         0x1D /** GS */
#define MUNSORTED_SET_I64            0x1E /** RS */
#define MSORTED_SET_I64              0x1F /** US */

/** abstract types for column-float containers */
#define MUNSORTED_MULTISET_FLOAT    MCOLUMN_FLOAT
#define MSORTED_MULTISET_FLOAT      '"'
#define MUNSORTED_SET_FLOAT         '$'
#define MSORTED_SET_FLOAT           '.'

/** abstract types for column-boolean containers */
#define MUNSORTED_MULTISET_BOOLEAN  MCOLUMN_BOOLEAN
#define MSORTED_MULTISET_BOOLEAN    '_'
#define MUNSORTED_SET_BOOLEAN       '\''
#define MSORTED_SET_BOOLEAN         0x7F /** DEL */

#define MERR -1

void markers_print(str_buf *dst, rec *src);

#ifdef __cplusplus
}
#endif

#endif
