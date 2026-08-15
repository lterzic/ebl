add_library(ebl-arch-cortex-m counters.c)
target_compile_definitions(ebl-arch-cortex-m PUBLIC EBL_COUNTER_32BIT "EBL_ARCH_NAME=\"cortex-m\"")
target_include_directories(ebl-arch-cortex-m PRIVATE ${PROJECT_SOURCE_DIR}/include)

add_library(ebl-arch ALIAS ebl-arch-cortex-m)
