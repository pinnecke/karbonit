## 0.7.00.0 [2020-01-16]
- Traverse framework
- improved json printer
- fix issue 2, memory debugging, performance and micro optimization
- closed sources
- first part for SET function on Carbon records

## 0.6.00.0 [2019-11-25]
- Add carbon revision patching as alternative to carbon revisions

## 0.5.00.03 [2019-09-27]
- Add source amalgamation as optional build option to increase runtime performance by generating a single translation 
  unit for Karbonits sources. Set `-DUSE_AMALGAMATION=ON` to use amalgamation, and see [INSTALL.md](INSTALL.md) for more
  information. Implementation details are given in [CMakeLists.txt](CMakeLists.txt).
- Karbonit Developer Documentation, see [book/](book/README.md) and examples in [examples/](examples).
- Add *abstract types* (abstract base types, and derived abstract types) to the specification and documentation
- Add *abstract types* to implementation (`carbon_abstract`)
- Add `fn_result` type that simplifies function err handling and function results. In a nutshell, `fn_result`
  is a 64bit value holding 48bit for a function return value (if any), and a 16bit err code encoding success,
  success with return result (32bit integer/unsigned integer, boolean, 48bit pointer), or a particular failure. In
  case of failure, a (thread-local) global err structure is filled and can be read from the caller.  

## 0.5.00.02 [2019-08-23]
- JSON parser for carbon files (see `rec_from_json` in `carbon.h`)
- Increase building times by sharing common sources in a static library
- Dot path evaluation for objects (e.g., `0.x.5."my key"`) for carbon files
- Add shortened path evaluation for record container to make the initial array index `0` optional, i.e., `0.x` is 
  equivalent to `x` if a record (array) container only contains one element, such as for an input `{"x":"y"}` . This 
  does not hold for single-element ("unit") arrays that are not the record container, i.e., `x.0.y` is not equivalent 
  to `x.y` for an input `{"x": [{"y": "z"}]}`. The reason for the latter is not shadow user-data semantics, while the 
  first leads to compatibility to MongoDB and CouchDB and hides internal structure of carbon files.   
- Results for `find` dot path expression can be printed with the default Json printer 
- Commit hash based revision numbers for revised records

## 0.3.00.01 [2019-08-09]
- Add thread pool implementation (`struct thread_pool`)

## 0.3.00.00 [2019-04-11]
- In `carbon-tool`, enable the user to set whether a single-threaded (`sync`) or multi-threaded (`async`) str_buf 
  dictionary should be used when conversion from JSON to CARBON archives is issued (via `convert` module). By
  default a multi-threaded implementation with 8 threads is used. To change that, `carbon-tool convert` has
  now to new parameters, `--dic-type` and `--dic-nthreads`. See `carbon-tool` usage instructions for details.
- Modified command line in module `convert` of `carbon-tool` to select compressor to be used for conversion 
  from JSON to CARBON archives. Until now, `--compressor=<compressor>` was used. Now, it is just 
  `--compressor <compressor>`.
- In Compressor Framework, add function to read implementation-specific extra data (e.g., in order to reconstruct
  a coding table). See `read_extra` of `carbon_compressor_t` in [carbon-compressor.h](include/carbon/compressor/compressor.h)  
- Fix a bug that permits to import empty JSON-objects (e.g., `{ "k": {} }`) in archives 
- Refactoring and cleanup

## 0.2.00.01 [2019-03-25]
- To avoid re-creation of the index used for *Indexed String Fetch* when a archive is loaded into memory, 
  this index can now be pre-computed an embedded into the archive file itself (which is now the default behavior). 
  For this, the index is built after archive creation and appended at the end of the archive (if not turned-off). 
  To turn it off in `carbon-tool`, use flag `--no-str_buf-id-index` for the module `convert`. If turned-off, the 
  index may be created during runtime depending on whether such an index should be used or not (see below).
  - Archive specification must be changed for this, see updated [SPECIFICATION.md](SPECIFICATION.md) and 
    [SPECIFICATION-COMPACT.md](SPECIFICATION_COMPACT.md).

## 0.2.00.00 [2019-02-27]
- Add archive visitor framework, see [archive visitor framework](include/carbon/archive/archive_visitor.h).
- Add in-memory key-value pair representation of unstructured JSON-like objects 
  (which still use [object identifiers](include/carbon/oid/oid.h) and encoded strings) in preparation to 
  print CARBON files in typical key-value pair structure, see [encoded documents](include/carbon/json/encoded_doc.h).
    - Used [archive visitor framework](include/carbon/archive/archive_visitor.h) to convert 
      [CARBON archives](include/carbon/archive/archive.h) to an
      [encoded document collection](include/carbon/json/encoded_doc.h)
- *I/O Improved String Fetch* To avoid I/O with the file system as much as possible - given a particular
  amount of memory reserved to hold (uncompressed) str_buf in main memory -, a str_buf fetch by default
  first accesses a LRU hash-table-based cache (*Cached String Fetch*). In case of a cache miss, the str_buf id is translated in-memory to
  its corresponding (potentially compressed) str_buf offset in the archive file, and fetched directly avoiding
  a full scan (*Indexed String Fetch*). In detail:
    - *Indexed String Fetch*: Add archive-local index that maps from str_buf ids to the offsets in the archive file where 
       the strings associated with those ids are located. Using this index avoids a *full scan* to find the str_buf offset
       in the file. However, such an index is automatically created (once) `carbon_archive_query` is called. To
       avoid creation of this index, the index creation can be bypassed by calling `carbon_query_create` on the
       desired archive. Additionally, an index can be dropped during the lifetime of an archive by 
       calling `carbon_archive_drop_indexes`. Once an archive is closed, indexes gets dropped automatically (if any).
    - *Cached String Fetch*: Add archive-local cache that holds strings associated with str_buf ids uncompressed
       in memory until those strings get evicted from the cache due to space limitations. As for indexed str_buf
       fetch, such a cache is automatically generated (once) when `carbon_archive_query` is called but also
       can be bypassed in the same way as the index creation for indexed str_buf fetches. Depending on the
       cache size and data access pattern, accesses to the underlying data provider (i.e., the index for str_buf 
       fetches or a str_buf fetch via a full scan) can be significantly avoided if not completely avoided.  
       Internally, the cache is organized as a (robin hood) hash table using the bernstein hash function that
       uses the str_buf id as key to find a fitting bucket. Inside the bucket, an LRU list for entries
       with str_buf ids that have a collision under the hash function is maintained.
- Add `$ carbon-tool to_json <input>` to convert a CARBON archive file `<input>` into its JSON representation.  

## 0.1.00.06 [2019-02-23]
- Completed compressor framework including embedding into carbin archive operations 
  (e.g., automatically call <code>encode</code> or <code>decode</code> when needed)
    - Compressor _none_ works fully, _huffman_ still cannot <code>decode</code>
- Implement query function to archives
    - *String Identifier Scan*: Iteration through all str_buf ids (including offset and str_buf length) 
      stored in an archives str_buf table, see <code>carbon_query_scan_strids</code>. Iterator model 
      is vec-based to minimize I/O calls to the underyling disk file, 
      see [carbon-strid-iter_t](include/carbon/archive/strid-iter.h)
    - *String Fetch*: Fetch a stored (and potentially compressed) str_buf in an archives str_buf 
      table given its str_buf identifier. For generic and safe calls, this fetch performs *full scan*
      iteration using [carbon-strid-iter_t](include/carbon/archive/strid-iter.h) seeking for the desired
      str_buf id (if a str_buf id is found), see <code>carbon_query_fetch_string_by_id</code>. 
      For fast and unsafe calls having the offset to the desired str_buf id in the archives str_buf table 
      at hand, a _random access_ into the archives underlying disk file is performed, see
      <code>carbon_query_fetch_strings_by_offset</code>. To avoid too many open files and to guarantee 
      thread-safe behavior, such an unsafe operation is invoked via a 
      [carbon-io-context_t](include/carbon/archive/io_context.h). In both case, the str_buf
      gets decompressed using the compressor framework when it gets fetch into main memory.
  - *Id Fetch*: Fetch str_buf ids where the referenced str_buf matches a particular condition,
      see <code>carbon_query_find_ids</code>. Such a condition, implemented as (vectorized) predicate 
      function, can be freely defined by extending the [String Predicate Framework](include/carbon/archive/str_buf-pred.h) with
      user-defined predicates. There are already some [built-in predicates](include/carbon/str_buf-pred),
      such as [equals](include/carbon/str_buf-pred/carbon-str_buf-pred-equals.h) or 
      [contains](include/carbon/str_buf-pred/carbon-str_buf-pred-contains.h). Internally, a str_buf
      identifier scan is invoked. For the entire vec for such an iteration step, a str_buf
      fetch with random access is invoked, yielding the corresponding uncompressed strings to the
      str_buf identifiers inside the vec. For all of these strings, the predicate is invoked
      resulting in the indices inside the vec where the strings match the condition. Marked
      indices are added to the result set. Depending on the predicate and the users choice, a limit
      to the result set size can be set, stopping the fetch potentially earlier.
- Implemented iterator model to iterate trough CARBON archives in order to fully exploit zero-cost access
  to columnar stored key and value data, see [iterators](include/carbon/archive/archive_iter.h). 

## 0.1.00.05 [2019-02-13]
- **Compressor Framework** refactored compressor framework. 
    - Extension with new compressors in [carbon-compressor.c](src/carbon/compressor/compressor.c)
    - Current compressors are located in [include/carbon/compressor](include/carbon/compressor)
    - Available compressors now listed via `carbon-tool list compressors`,
      and selection is done via `carbon-tool convert --use-compressor=$NAME` where 
      `$NAME` is from the list above 

## 0.1.00.04 [2019-02-13]
- Unique object identifiers added in CARBON files, see [oid.c](src/carbon/oid/oid.c)
- Fine-grained switched to turn on and turn of logging, see [INSTALL.md](INSTALL.md)
- Default build type is now release
- Contains the following bug fixes 
    - [size-optimized (Huffman) fails in edge cases](https://github.com/karbonitlabs/karbonit/issues/1)

## 0.1.00.00 [2019-02-07]

- Initial standalone release
- Set license to MIT
- Refactoring and cleanups
