cmake_minimum_required(VERSION 3.1)

project(erasure-codes)

enable_testing()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)

add_subdirectory(gf-arithmetic)
add_subdirectory(erasure-codes)
add_subdirectory(lohi-generator)
add_subdirectory(log-table-generator)

# Testing
add_subdirectory(erasure-tests)
add_subdirectory(gf-arith-tests)



# v2
add_subdirectory(liberasure)
