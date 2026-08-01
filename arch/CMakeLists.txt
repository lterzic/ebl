# Standardize the processor string to lowercase
string(TOLOWER "${CMAKE_SYSTEM_PROCESSOR}" AT_ARCH)

if(AT_ARCH MATCHES "x86_64" OR AT_ARCH MATCHES "amd64")
    add_subdirectory(x86_64)
elseif(AT_ARCH MATCHES "arm64" OR AT_ARCH MATCHES "aarch64")
    message(STATUS "Target is ARM64")
elseif(AT_ARCH MATCHES "arm")
    message(STATUS "Target is ARM 32-bit")
elseif(AT_ARCH MATCHES "i386" OR AT_ARCH MATCHES "x86")
    message(STATUS "Target is x86 32-bit")
else()
    message(STATUS "Target is an unknown architecture: ${CMAKE_SYSTEM_PROCESSOR}")
endif()
