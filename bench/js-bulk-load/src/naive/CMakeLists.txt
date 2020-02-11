cmake_minimum_required(VERSION 3.9.6)

project(bench)

ADD_CUSTOM_TARGET(bench-js-bulk-load-naive)
add_dependencies(bench-js-bulk-load-naive karbonit-sources)
set_target_properties(bench-js-bulk-load-naive PROPERTIES EXCLUDE_FROM_ALL TRUE)
