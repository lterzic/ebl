cmake_minimum_required(VERSION 3.12)
project(ebl C CXX)

add_subdirectory(arch)

# Builds src/measure.c under a given EBL_MEASURE_<MODE> macro (see
# src/measure.c). Final executable targets must link exactly one of
# ebl-measure-precise / ebl-measure-fast alongside ebl-benchmark to pick a
# measurement mode -- ebl-benchmark itself is mode-agnostic.
function(ebl_add_measure_library TARGET_NAME MODE)
    add_library(${TARGET_NAME} src/measure.c)
    target_include_directories(${TARGET_NAME} PUBLIC include)
    target_link_libraries(${TARGET_NAME} PUBLIC ebl-arch)
    target_compile_options(${TARGET_NAME} PRIVATE -ffreestanding)
    target_compile_definitions(${TARGET_NAME} PRIVATE EBL_MEASURE_${MODE})
endfunction()

ebl_add_measure_library(ebl-measure-precise PRECISE)
ebl_add_measure_library(ebl-measure-fast FAST)

add_library(ebl-benchmark src/benchmark.c)
target_include_directories(ebl-benchmark PUBLIC include)
# ebl-arch for EBL_COUNTER_32BIT/64BIT (an architecture property, not a
# measurement-mode one) -- the measurement mode itself comes from whichever
# ebl-measure-precise/fast the final executable target links in separately.
target_link_libraries(ebl-benchmark PUBLIC ebl-arch)
target_compile_options(ebl-benchmark PRIVATE -ffreestanding)

# Link this to provide a default benchmark reporter to std output.
add_library(ebl-stdio OBJECT src/stdio.c)
target_link_libraries(ebl-stdio PUBLIC ebl-benchmark)

# Link this to add main which automatically runs all benchmarks.
add_library(ebl-main src/main.c)
target_link_libraries(ebl-main PUBLIC ebl-benchmark)

option(EBL_BUILD_EXAMPLES "Build ebl example programs" ON)
if(EBL_BUILD_EXAMPLES)
    add_subdirectory(examples/basic)
    add_subdirectory(examples/freertos)
endif()
