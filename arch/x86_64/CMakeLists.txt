add_library(ebl-arch-x86_64 counters.c)
target_compile_definitions(ebl-arch-x86_64 PUBLIC EBL_COUNTER_64BIT)

add_library(ebl-arch ALIAS ebl-arch-x86_64)