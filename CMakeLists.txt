cmake_minimum_required(VERSION 3.10)
project(ebl)

add_subdirectory(arch)

add_library(ebl-measure src/measure.c)
target_include_directories(ebl-measure PUBLIC include)
target_link_libraries(ebl-measure PRIVATE ebl-arch)