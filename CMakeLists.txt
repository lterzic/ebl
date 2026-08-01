cmake_minimum_required(VERSION 3.12)
project(ebl C CXX)

add_subdirectory(arch)

add_library(ebl-measure src/measure.c)
target_include_directories(ebl-measure PUBLIC include)
target_link_libraries(ebl-measure PUBLIC ebl-arch)
target_compile_options(ebl-measure PRIVATE -ffreestanding)

add_library(ebl-benchmark src/benchmark.c)
target_link_libraries(ebl-benchmark PUBLIC ebl-measure)
target_compile_options(ebl-benchmark PRIVATE -ffreestanding)

# Link this to provide a default benchmark reporter to std output.
add_library(ebl-stdio OBJECT src/stdio.c)
target_link_libraries(ebl-stdio PUBLIC ebl-benchmark)

# Link this to add main which automatically runs all benchmarks.
add_library(ebl-main src/main.c)
target_link_libraries(ebl-main PUBLIC ebl-benchmark)

option(EBL_BUILD_EXAMPLES "Build ebl example programs" ON)
if(EBL_BUILD_EXAMPLES)
    add_subdirectory(example)
endif()
