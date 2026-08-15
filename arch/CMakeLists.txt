# Standardize the processor string to lowercase
string(TOLOWER "${CMAKE_SYSTEM_PROCESSOR}" AT_ARCH)

if(AT_ARCH MATCHES "x86_64" OR AT_ARCH MATCHES "amd64")
    add_subdirectory(x86_64)
elseif(AT_ARCH MATCHES "riscv64")
    set(EBL_RISCV_XLEN 64)
    add_subdirectory(riscv)
elseif(AT_ARCH MATCHES "riscv32")
    set(EBL_RISCV_XLEN 32)
    add_subdirectory(riscv)
elseif(AT_ARCH MATCHES "arm64" OR AT_ARCH MATCHES "aarch64")
    message(FATAL_ERROR "ebl: no counter backend for architecture 'arm64' yet")
elseif(AT_ARCH MATCHES "arm")
    # CMAKE_SYSTEM_PROCESSOR reports "arm" for both Cortex-A/R (Linux-hosted)
    # and Cortex-M (bare-metal) targets; only the latter has the DWT cycle
    # counter this backend relies on, so ask the compiler which core profile
    # it's targeting via the __ARM_ARCH_PROFILE predefine ('A', 'R', or 'M').
    include(CheckCSourceCompiles)
    set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
    check_c_source_compiles("
        #if !defined(__ARM_ARCH_PROFILE) || __ARM_ARCH_PROFILE != 'M'
        #error not-cortex-m
        #endif
        int ebl_arch_probe;
    " EBL_IS_CORTEX_M)
    unset(CMAKE_TRY_COMPILE_TARGET_TYPE)

    if(EBL_IS_CORTEX_M)
        add_subdirectory(cortex-m)
    else()
        message(FATAL_ERROR "ebl: no counter backend for architecture 'arm' yet (only Cortex-M is supported)")
    endif()
elseif(AT_ARCH MATCHES "i386" OR AT_ARCH MATCHES "x86")
    message(FATAL_ERROR "ebl: no counter backend for architecture 'x86' yet")
else()
    message(FATAL_ERROR "ebl: no counter backend for architecture '${CMAKE_SYSTEM_PROCESSOR}' yet")
endif()
