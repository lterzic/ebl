cmake_minimum_required(VERSION 3.12)
project(ebl)

add_subdirectory(arch)

add_library(ebl-measure src/measure.c)
target_include_directories(ebl-measure PUBLIC include)
target_link_libraries(ebl-measure PUBLIC ebl-arch)

add_library(ebl-benchmark src/benchmark.c)
target_link_libraries(ebl-benchmark PUBLIC ebl-measure)

# Link this to provide a default benchmark reporter to std output.
add_library(ebl-stdio OBJECT src/stdio.c)
target_link_libraries(ebl-stdio PUBLIC ebl-benchmark)

# Link this to add main which automatically runs all benchmarks.
add_library(ebl-main src/main.c)
target_link_libraries(ebl-main PUBLIC ebl-benchmark)
