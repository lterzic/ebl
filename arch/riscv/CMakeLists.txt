add_library(ebl-arch-riscv counters.c)
target_compile_definitions(ebl-arch-riscv PUBLIC EBL_COUNTER_${EBL_RISCV_XLEN}BIT "EBL_ARCH_NAME=\"riscv${EBL_RISCV_XLEN}\"")
target_include_directories(ebl-arch-riscv PRIVATE ${PROJECT_SOURCE_DIR}/include)

add_library(ebl-arch ALIAS ebl-arch-riscv)
