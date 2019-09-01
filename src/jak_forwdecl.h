/**
 * Copyright 2018 Marcus Pinnecke
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of
 * the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef JAK_FORWDECL_H
#define JAK_FORWDECL_H

typedef struct jak_allocator jak_allocator;

typedef struct jak_archive jak_archive;
typedef struct jak_archive_callback jak_archive_callback;
typedef struct jak_sid_cache_stats jak_sid_cache_stats;
typedef struct jak_archive_header jak_archive_header;
typedef struct jak_record_header jak_record_header;
typedef struct jak_object_header jak_object_header;
typedef struct jak_prop_header jak_prop_header;
typedef union jak_string_tab_flags jak_string_tab_flags_u;
typedef struct jak_string_table_header jak_string_table_header;
typedef struct jak_object_array_header jak_object_array_header;
typedef struct jak_column_group_header jak_column_group_header;
typedef struct jak_column_header jak_column_header;
typedef union jak_object_flags jak_object_flags_u;
typedef struct jak_archive_prop_offs jak_archive_prop_offs;
typedef struct jak_fixed_prop jak_fixed_prop;
typedef struct jak_table_prop jak_table_prop;
typedef struct jak_var_prop jak_var_prop;
typedef struct jak_array_prop jak_array_prop;
typedef struct jak_null_prop jak_null_prop;
typedef struct jak_record_flags jak_record_flags;
typedef struct jak_string_table jak_string_table;
typedef struct jak_record_table jak_record_table;
typedef struct jak_archive_info jak_archive_info;
typedef struct jak_string_entry_header jak_string_entry_header;
typedef struct jak_archive_io_context jak_archive_io_context;
typedef struct jak_archive_object jak_archive_object;
typedef struct jak_collection_iter_state jak_collection_iter_state;
typedef struct jak_archive_value_vector jak_archive_value_vector;
typedef struct jak_prop_iter jak_prop_iter;
typedef struct jak_independent_iter_state jak_independent_iter_state;
typedef struct jak_column_object_iter jak_column_object_iter;
typedef struct jak_string_pred jak_string_pred;
typedef struct jak_archive_query jak_archive_query;
typedef struct jak_strid_info jak_strid_info;
typedef struct jak_strid_iter jak_strid_iter;
typedef struct jak_path_entry jak_path_entry;
typedef struct jak_archive_visitor_desc jak_archive_visitor_desc;
typedef struct jak_archive_visitor jak_archive_visitor;
typedef struct jak_column_doc_column jak_column_doc_column;
typedef struct jak_column_doc_group jak_column_doc_group;
typedef struct jak_column_doc_obj jak_column_doc_obj;
typedef struct jak_column_doc jak_column_doc;
typedef struct jak_doc_entries jak_doc_entries;
typedef struct jak_doc_bulk jak_doc_bulk;
typedef struct jak_doc jak_doc;
typedef struct jak_doc_obj jak_doc_obj;
typedef union jak_encoded_doc_value jak_encoded_doc_value_u;
typedef struct jak_encoded_doc_prop_header jak_encoded_doc_prop_header;
typedef struct jak_encoded_doc_prop jak_encoded_doc_prop;
typedef struct jak_encoded_doc_prop_array jak_encoded_doc_prop_array;
typedef struct jak_encoded_doc jak_encoded_doc;
typedef struct jak_encoded_doc_list jak_encoded_doc_list;

typedef struct jak_error jak_error;

typedef struct jak_async_func_proxy jak_async_func_proxy;
typedef struct jak_filter_arg jak_filter_arg;
typedef struct jak_map_args jak_map_args;
typedef struct jak_gather_scatter_args jak_gather_scatter_args;

typedef struct jak_bitmap jak_bitmap;

typedef struct jak_carbon jak_carbon;
typedef struct jak_carbon_insert jak_carbon_insert;
typedef struct jak_carbon_new jak_carbon_new;
typedef struct jak_field_access jak_field_access;
typedef struct jak_carbon_array_it jak_carbon_array_it;
typedef struct jak_carbon_column_it jak_carbon_column_it;
typedef struct jak_carbon_dot_node jak_carbon_dot_node;
typedef struct jak_carbon_dot_path jak_carbon_dot_path;
typedef struct jak_carbon_find jak_carbon_find;
typedef struct jak_carbon_insert_array_state jak_carbon_insert_array_state;
typedef struct jak_carbon_insert_object_state jak_carbon_insert_object_state;
typedef struct jak_carbon_insert_column_state jak_carbon_insert_column_state;
typedef struct jak_carbon_object_it jak_carbon_object_it;
typedef struct jak_carbon_path_evaluator jak_carbon_path_evaluator;
typedef struct jak_carbon_path_index jak_carbon_path_index;
typedef struct jak_carbon_path_index_it jak_carbon_path_index_it;
typedef struct jak_carbon_printer jak_carbon_printer;
typedef struct jak_carbon_revise jak_carbon_revise;
typedef struct jak_carbon_binary jak_carbon_binary;
typedef struct jak_carbon_update jak_carbon_update;
typedef struct jak_packer jak_packer;

typedef struct jak_hashset_bucket jak_hashset_bucket;
typedef struct jak_hashset jak_hashset;
typedef struct jak_hashtable_bucket jak_hashtable_bucket;
typedef struct jak_hashtable jak_hashtable;
typedef struct jak_huffman jak_huffman;
typedef struct jak_pack_huffman_entry jak_pack_huffman_entry;
typedef struct jak_pack_huffman_info jak_pack_huffman_info;
typedef struct jak_pack_huffman_str_info jak_pack_huffman_str_info;

typedef struct jak_json_token jak_json_token;
typedef struct jak_json_err jak_json_err;
typedef struct jak_json_tokenizer jak_json_tokenizer;
typedef struct jak_json_parser jak_json_parser;
typedef struct jak_json jak_json;
typedef struct jak_json_node_value jak_json_node_value;
typedef struct jak_json_object jak_json_object;
typedef struct jak_json_element jak_json_element;
typedef struct jak_json_string jak_json_string;
typedef struct jak_json_prop jak_json_prop;
typedef struct jak_json_members jak_json_members;
typedef struct jak_json_elements jak_json_elements;
typedef struct jak_json_array jak_json_array;
typedef struct jak_json_number jak_json_number;

typedef struct jak_memblock jak_memblock;
typedef struct jak_memfile jak_memfile;

typedef struct jak_command_opt jak_command_opt;
typedef struct jak_command_opt_group jak_command_opt_group;
typedef struct jak_command_opt_mgr jak_command_opt_mgr;

typedef struct jak_priority_queue_element_info jak_priority_queue_element_info;
typedef struct jak_priority_queue jak_priority_queue;

typedef struct jak_slice jak_slice;
typedef struct jak_hash_bounds jak_hash_bounds;
typedef struct jak_slice_descriptor jak_slice_descriptor;
typedef struct jak_slice_list jak_slice_list;
typedef struct jak_slice_handle jak_slice_handle;

typedef struct jak_spinlock jak_spinlock;

typedef struct jak_vector jak_vector;

typedef struct jak_str_hash jak_str_hash;
typedef struct jak_str_hash_counters jak_str_hash_counters;

typedef struct jak_string jak_string;

typedef struct jak_string_dict jak_string_dict;

typedef struct jak_thread_task jak_thread_task;
typedef struct jak_task_state jak_task_state;
typedef struct jak_task_handle jak_task_handle;
typedef struct jak_thread_pool jak_thread_pool;
typedef struct jak_thread_info jak_thread_info;
typedef struct jak_thread_pool_stats jak_thread_pool_stats;
typedef struct jak_thread_stats jak_thread_stats;
typedef struct jak_task_stats jak_task_stats;

#endif