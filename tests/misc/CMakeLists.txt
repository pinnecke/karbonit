cmake_minimum_required(VERSION 3.5)
project(carbon-api-examples-project-setup CXX)

set (CMAKE_CXX_STANDARD 11)

link_directories("/usr/local/lib/")

CreateTest(test-thread-pool)
CreateTest(test-fn-result)