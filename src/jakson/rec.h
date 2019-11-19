/*
 * Copyright 2019 Marcus Pinnecke
 */

#ifndef HAD_REC_H
#define HAD_REC_H

#include <jakson/stdinc.h>
#include <jakson/error.h>
#include <jakson/mem/block.h>
#include <jakson/mem/file.h>
#include <jakson/stdx/unique_id.h>
#include <jakson/std/string.h>
#include <jakson/std/spinlock.h>
#include <jakson/std/vector.h>
#include <jakson/stdinc.h>
#include <jakson/stdx/alloc.h>
#include <jakson/std/bitmap.h>
#include <jakson/std/bloom.h>
#include <jakson/archive.h>
#include <jakson/archive/it.h>
#include <jakson/archive/visitor.h>
#include <jakson/archive/converter.h>
#include <jakson/archive/encoded_doc.h>
#include <jakson/stdinc.h>
#include <jakson/utils/convert.h>
#include <jakson/archive/column_doc.h>
#include <jakson/archive/doc.h>
#include <jakson/error.h>
#include <jakson/std/hash/table.h>
#include <jakson/std/hash.h>
#include <jakson/archive/huffman.h>
#include <jakson/json/parser.h>
#include <jakson/mem/block.h>
#include <jakson/mem/file.h>
#include <jakson/stdx/unique_id.h>
#include <jakson/utils/sort.h>
#include <jakson/std/async.h>
#include <jakson/stdx/slicelist.h>
#include <jakson/std/spinlock.h>
#include <jakson/std/string_dict.h>
#include <jakson/std/str_hash.h>
#include <jakson/archive/strid_it.h>
#include <jakson/archive/cache.h>
#include <jakson/utils/time.h>
#include <jakson/types.h>
#include <jakson/archive/query.h>
#include <jakson/std/vector.h>
#include <jakson/stdx/alloc/trace.h>
#include <jakson/archive/encode_async.h>
#include <jakson/archive/encode_sync.h>
#include <jakson/std/str_hash/mem.h>
#include <jakson/archive/pred/contains.h>
#include <jakson/archive/pred/equals.h>
#include <jakson/carbon/printers.h>
#include <jakson/std/uintvar/stream.h>
#include <jakson/carbon/markers.h>
#include <jakson/carbon/abstract.h>
#include <jakson/carbon/binary.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Carbon files are built for task parallel environments where multiple readers and (potentially) a single writer
 * operate on a single file at a certain point in time. A per-file history is managed by a sequence of revisions
 * that result from revising an existing record. Each revision has a commit hash that identifies that particular version
 * of the record. Revising is an atomic modification operation on a record, which may span multiple manipulations
 * (inserts, updates, deletes) at once in one bulk. By the concept of revisions (see 'carbon_revise_begin()' and
 * 'carbon_revision_close()') readers are never blocked by a writer because a writer modifies a copy of the record
 * eventually swapping the old one with the revised record. However, for single-threaded environments, setting up a
 * new revision may be to much overhead. Therefore, a revision can be patched. Carbon file patching is, in a nut shell,
 * a revise but without data snapshotting and commit hash management. Patching allows to modify a document without
 * creating a new revision but by altered the current revision (i.e., the commit hash remains the same). See patch.h
 * for more. */
typedef struct rec {
        area *area;
        memfile file;
} rec;

/* record revision context */
typedef struct rev {
        rec *original;
        rec *revised_doc;
} rev;

typedef struct rec_new {
        rec original;
        rev revision_context;
        carbon_array *content_it;
        carbon_insert *inserter;
        /** options shrink or compact (or both) documents, see
         * CARBON_KEEP, CARBON_SHRINK, CARBON_COMPACT, and CARBON_OPTIMIZE  */
        int mode;
} rec_new;

typedef enum carbon_printer_impl {
        JSON_EXTENDED, JSON_COMPACT
} carbon_printer_impl_e;

#define CARBON_NIL_STR "_nil"

typedef enum carbon_key_type {
        /** no key, no revision number */
        CARBON_KEY_NOKEY = CARBON_MNOKEY,
        /** auto-generated 64bit unsigned integer key */
        CARBON_KEY_AUTOKEY = CARBON_MAUTOKEY,
        /** user-defined 64bit unsigned integer key */
        CARBON_KEY_UKEY = CARBON_MUKEY,
        /** user-defined 64bit signed integer key */
        CARBON_KEY_IKEY = CARBON_MIKEY,
        /** user-defined n-char string_buffer key */
        CARBON_KEY_SKEY = CARBON_MSKEY
} carbon_key_e;

#define CARBON_KEEP              0x00 /** do not shrink, do not compact, use UNSORTED_MULTISET (equiv. JSON array) */
#define CARBON_SHRINK            0x01 /** perform shrinking, i.e., remove tail-buffer from carbon file */
#define CARBON_COMPACT           0x02 /** perform compacting, i.e., remove reserved memory from containers */
#define CARBON_UNSORTED_MULTISET 0x04 /** annotate the record outer-most array as unsorted multi set */
#define CARBON_SORTED_MULTISET   0x08 /** annotate the record outer-most array as sorted multi set */
#define CARBON_UNSORTED_SET      0x10 /** annotate the record outer-most array as unsorted set */
#define CARBON_SORTED_SET        0x20 /** annotate the record outer-most array as sorted set */

#define CARBON_OPTIMIZE          (CARBON_SHRINK | CARBON_COMPACT | CARBON_UNSORTED_MULTISET)

/**
 * Constructs a new context in which a new document can be created. The parameter <b>options</b> controls
 * how reserved spaces should be handled after document creation is done, and which abstract type for the outer-most
 * record array should be used.
 *
 * Set <code>options</code> to <code>CARBON_KEEP</code> for no optimization. With this option, all capacities
 * (i.e., additional ununsed but free space) in containers (objects, arrays, and columns) are kept and tailing free
 * space after the document is kept, too. Use this option to optimize for "insertion-heavy" documents since keeping all
 * capacities lowest the probability of reallocations and memory movements. Set <b>options</b> to
 * <code>CARBON_COMPACT</code> if capacities in containers should be removed after creation, and
 * <code>CARBON_COMPACT</code> to remove tailing free space. Use <code>CARBON_OPTIMIZE</code> to use both
 * <code>CARBON_SHRINK</code> and <code>CARBON_COMPACT</code>.
 *
 * As a rule of thumb for <b>options</b>. The resulting document...
 * <ul>
 *  <li>...will be updated heavily where updates may change the type-width of fields, will be target of many inserts
 *  containers, use <code>CARBON_KEEP</code>. The document will have a notable portion of reserved memory contained;
 *  insertions or updates will, however, not require immediately reallocation or memory movements.</li>
 *  <li>...will <i>not</i> be target of insertion of strings or blob fields in the near future, use
 *      <code>CARBON_SHRINK</code>. The document will not have padding reserved memory at the end, which means that
 *      a realloction will be required once the document grows (e.g., a container must be englarged). Typically,
 *      document growth is handled with container capacities (see <code>CARBON_COMPACT</code>). However, insertions
 *      of variable-length data (i.e., strings and blobs) may require container enlargement. In this case, having
 *      padding reserved memory at the end of the document lowers the risk of a reallocation.</li>
 *  <li>...will <i>not</i> not be target of insertion operations or update operations that changes a fields type-width
 *      in the near future. In simpler words, if a document is updated and each such update keeps the (byte) size
 *      of the field, use <code>CARBON_COMPACT</code>. This option will remove all capacities in containers.</li>
 *  <li>...is read-mostly, or updates will not change the type or type-width of fields, use <code>CARBON_OPTIMIZE</code>.
 *      The document will have the smallest memory footprint possible.</li>
 * </ul>
 *
 * To annotate the outer-most record array as an array container with particular properties, set <code>options</code>
 * to...
 * <ul>
 *  <li>...<code>CARBON_UNSORTED_MULTISET</code> that allows duplicates and has no sorting</li>
 *  <li>...<code>CARBON_SORTED_MULTISET</code> that allows duplicates and has a particular sorting</li>
 *  <li>...<code>CARBON_UNSORTED_SET</code> that disallows duplicates and has no sorting</li>
 *  <li>...<code>CARBON_SORTED_SET</code> that disallows duplicates and has a particular sorting</li>
 *  </ul>
 *  If no annotation is given, the outer-most record array is annotated as <code>CARBON_UNSORTED_MULTISET</code>, which
 *  matches the semantics of a JSON array. Please note that this library does not provide any features to make
 *  deduplication and sorting work. Instead, the annotation stores the semantics of that particular container, which
 *  functionality must be effectively implemented at caller site.
 */
carbon_insert * carbon_create_begin(rec_new *context, rec *doc, carbon_key_e type, int options);
void carbon_create_end(rec_new *context);
void carbon_create_empty(rec *doc, carbon_list_derivable_e derivation, carbon_key_e type);
void carbon_create_empty_ex(rec *doc, carbon_list_derivable_e derivation, carbon_key_e type, u64 doc_cap, u64 array_cap);

bool carbon_from_json(rec *doc, const char *json, carbon_key_e type, const void *key);
bool carbon_from_raw_data(rec *doc, const void *data, u64 len);

bool carbon_drop(rec *doc);

const void *carbon_raw_data(u64 *len, rec *doc);

bool carbon_key_type(carbon_key_e *out, rec *doc);
const void *carbon_key_raw_value(u64 *len, carbon_key_e *type, rec *doc);
bool carbon_key_signed_value(i64 *key, rec *doc);
bool carbon_key_unsigned_value(u64 *key, rec *doc);
const char *carbon_key_string_value(u64 *len, rec *doc);
bool carbon_has_key(carbon_key_e type);
bool carbon_key_is_unsigned(carbon_key_e type);
bool carbon_key_is_signed(carbon_key_e type);
bool carbon_key_is_string(carbon_key_e type);
bool carbon_clone(rec *clone, rec *doc);
bool carbon_commit_hash(u64 *hash, rec *doc);

/** Checks if the records most-outer array is annotated as a multi set abstract type. Returns true if the record
 * is a multi set, and false if the record is a set. In case of any error, a failure is returned. */
bool carbon_is_multiset(rec *doc);

/** Checks if the records most-outer array is annotated as a sorted abstract type. Returns true if this is the case,
 * otherwise false. In case of any error, a failure is returned. */
bool carbon_is_sorted(rec *doc);

/** Changes the abstract type of the most-outer record array to the given abstract type */
void carbon_update_list_type(rec *revised_doc, rec *doc, carbon_list_derivable_e derivation);

bool carbon_to_str(string_buffer *dst, carbon_printer_impl_e printer, rec *doc);
const char *carbon_to_json_extended(string_buffer *dst, rec *doc);
const char *carbon_to_json_compact(string_buffer *dst, rec *doc);
char *carbon_to_json_extended_dup(rec *doc);
char *carbon_to_json_compact_dup(rec *doc);

/* Opens a read-only iterator for navigating though the records contents.
 *
 * For readers, use 'carbon_read_begin', and 'carbon_revise_begin()' for writers.
 *
 * In case a single record must be updated while the upfront costs for the entire revision process is not sustainable,
 * a record might be patched using a patch iterator, see
 *
 *
 * An opened iterator must be closed by calling 'carbon_read_end'. Not closing an iterator leads to undefined
 * behavior. */
void carbon_read_begin(carbon_array *it, rec *doc);

/* Closes a read-only iterator, which was previously opened via 'carbon_read_begin' */
void carbon_read_end(carbon_array *it);

bool carbon_print(FILE *file, carbon_printer_impl_e printer, rec *doc);
bool carbon_hexdump_print(FILE *file, rec *doc);

#ifdef __cplusplus
}
#endif

#endif
