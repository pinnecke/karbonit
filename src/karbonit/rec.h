/*
 * rec - Carbon file record implementation
 *
 * Copyright 2019 Marcus Pinnecke
 */

#ifndef HAD_REC_H
#define HAD_REC_H

#include <karbonit/stdinc.h>
#include <karbonit/forwdecl.h>
#include <karbonit/mem/memfile.h>
#include <karbonit/carbon/markers.h>
#include <karbonit/carbon/abstract.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Carbon files are built for task parallel environments where multiple readers and (potentially) a single writer
 * operate on a single file at a certain point in time. A per-file history is managed by a sequence of revisions
 * that result from revising an existing record. Each revision has a commit hash that identifies that particular version
 * of the record. Revising is an atomic modification operation on a record, which may span multiple manipulations
 * (inserts, updates, deletes) at once in one bulk. By the concept of revisions (see 'revise_begin()' and
 * 'carbon_revision_close()') readers are never blocked by a writer because a writer modifies a copy of the record
 * eventually swapping the old one with the revised record. However, for single-threaded environments, setting up a
 * new revision may be to much overhead. Therefore, a revision can be patched. Carbon file patching is, in a nut shell,
 * a revise but without data snapshotting and commit hash management. Patching allows to modify a document without
 * creating a new revision but by altered the current revision (i.e., the commit hash remains the same). See patch.h
 * for more. */
typedef struct rec {
        memblock *block;
        memfile file;
        offset_t data_off;
} rec;

/* record revision context */
typedef struct rev {
        rec *original;
        rec *revised;
} rev;

typedef struct rec_new {
        rec original;
        rev context;
        arr_it *array;
        insert *in;
        /** options shrink or compact (or both) documents, see
         * CARBON_KEEP, CARBON_SHRINK, CARBON_COMPACT, and CARBON_OPTIMIZE  */
        int mode;
} rec_new;

#define CARBON_NIL_STR "undef"

typedef enum key {
        /** no key, no revision number */
        KEY_NOKEY = MNOKEY,
        /** auto-generated 64bit unsigned integer key */
        KEY_AUTOKEY = MAUTOKEY,
        /** user-defined 64bit unsigned integer key */
        KEY_UKEY = MUKEY,
        /** user-defined 64bit signed integer key */
        KEY_IKEY = MIKEY,
        /** user-defined n-char str_buf key */
        KEY_SKEY = MSKEY
} key_e;

#define KEEP              0x00 /** do not shrink, do not compact, use UNSORTED_MULTISET (equiv. JSON array) */
#define SHRINK            0x01 /** perform shrinking, i.e., remove tail-buffer from carbon file */
#define COMPACT           0x02 /** perform compacting, i.e., remove reserved memory from containers */
#define UNSORTED_MULTISET 0x04 /** annotate the record outer-most array as unsorted multi set */
#define SORTED_MULTISET   0x08 /** annotate the record outer-most array as sorted multi set */
#define UNSORTED_SET      0x10 /** annotate the record outer-most array as unsorted set */
#define SORTED_SET        0x20 /** annotate the record outer-most array as sorted set */

#define OPTIMIZE          (SHRINK | COMPACT | UNSORTED_MULTISET)

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
 *  <li>...<code>UNSORTED_MULTISET</code> that allows duplicates and has no sorting</li>
 *  <li>...<code>SORTED_MULTISET</code> that allows duplicates and has a particular sorting</li>
 *  <li>...<code>UNSORTED_SET</code> that disallows duplicates and has no sorting</li>
 *  <li>...<code>SORTED_SET</code> that disallows duplicates and has a particular sorting</li>
 *  </ul>
 *  If no annotation is given, the outer-most record array is annotated as <code>UNSORTED_MULTISET</code>, which
 *  matches the semantics of a JSON array. Please note that this library does not provide any features to make
 *  deduplication and sorting work. Instead, the annotation stores the semantics of that particular container, which
 *  functionality must be effectively implemented at caller site.
 */
insert *rec_create_begin(rec_new *context, rec *doc, key_e type, int options);
void rec_create_end(rec_new *context);
void rec_create_empty(rec *doc, list_type_e derivation, key_e type);
void rec_create_empty_ex(rec *doc, list_type_e derivation, key_e type, u64 doc_cap, u64 array_cap);

bool rec_from_json(rec *doc, const char *json, key_e type, const void *key);
bool rec_from_raw_data(rec *doc, const void *data, u64 len);

bool rec_drop(rec *doc);

const void *rec_raw_data(u64 *len, rec *doc);

void rec_clone(rec *clone, rec *doc);
bool rec_commit_hash(u64 *hash, rec *doc);

/** Checks if the records most-outer array is annotated as a multi set abstract type. Returns true if the record
 * is a multi set, and false if the record is a set. In case of any ERROR, a failure is returned. */
bool rec_is_multiset(rec *doc);

/** Checks if the records most-outer array is annotated as a sorted abstract type. Returns true if this is the case,
 * otherwise false. In case of any ERROR, a failure is returned. */
bool rec_is_sorted(rec *doc);

/** Changes the abstract type of the most-outer record array to the given abstract type */
void rec_update_list_type(rec *revised, rec *doc, list_type_e derivation);

const char *rec_to_json(str_buf *dst, rec *doc);

/* Opens a read-only iterator for navigating though the records contents.
 *
 * For readers, use 'rec_read', and 'revise_begin()' for writers.
 *
 * In case a single record must be updated while the upfront costs for the entire revision process is not sustainable,
 * a record might be patched using a patch iterator, see
 *
 *
 * An opened iterator must be closed by calling 'rec_read_end'. Not closing an iterator leads to undefined
 * behavior. */
void rec_read(arr_it *it, rec *doc);

bool rec_is_array(const rec *doc);

bool rec_hexdump_print(FILE *file, rec *doc);

bool rec_key_type(key_e *out, rec *doc);
const void *rec_key_raw_value(u64 *len, key_e *type, rec *doc);
bool rec_key_signed_value(i64 *key, rec *doc);
bool rec_key_unsigned_value(u64 *key, rec *doc);
const char *key_string_value(u64 *len, rec *doc);
bool rec_has_key(key_e type);
bool rec_key_is_unsigned(key_e type);
bool rec_key_is_signed(key_e type);
bool rec_key_is_string(key_e type);

#ifdef __cplusplus
}
#endif

#endif
