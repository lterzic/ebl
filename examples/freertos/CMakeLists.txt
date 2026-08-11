# Benchmarks FreeRTOS mutex/queue primitives via the FreeRTOS POSIX
# simulator. Off by default -- it pulls in FreeRTOS-Kernel over the network.
option(EBL_BUILD_FREERTOS_EXAMPLE "Build the FreeRTOS POSIX-sim example" OFF)
if(NOT EBL_BUILD_FREERTOS_EXAMPLE)
    return()
endif()

include(FetchContent)
FetchContent_Declare(
    freertos_kernel
    GIT_REPOSITORY https://github.com/FreeRTOS/FreeRTOS-Kernel.git
    GIT_TAG        V11.1.0
    GIT_SHALLOW    TRUE
)

# freertos_kernel requires this target (for FreeRTOSConfig.h) and these
# cache variables to be set before it's added.
add_library(freertos_config INTERFACE)
target_include_directories(freertos_config SYSTEM INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
set(FREERTOS_PORT GCC_POSIX CACHE STRING "" FORCE)
set(FREERTOS_HEAP "4" CACHE STRING "" FORCE)

FetchContent_MakeAvailable(freertos_kernel)

foreach(mode precise fast)
    add_executable(ebl-freertos-example-${mode} main.c bench_mutex.c bench_queue.c)
    target_link_libraries(ebl-freertos-example-${mode} PRIVATE ebl-benchmark ebl-stdio ebl-measure-${mode} freertos_kernel)
endforeach()
