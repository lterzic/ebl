#pragma once

// Minimal config for FreeRTOS's POSIX simulator, just enough to run the
// mutex/queue latency benchmarks in this example -- no networking, tracing,
// or run-time-stats support.

#include <assert.h>
#include <pthread.h>
#include <stdint.h>

#define configASSERT(x) assert(x)

#define configUSE_PREEMPTION                    1
#define configUSE_PORT_OPTIMISED_TASK_SELECTION 0
#define configUSE_TICKLESS_IDLE                 0
#define configUSE_IDLE_HOOK                     0
#define configUSE_TICK_HOOK                     0
#define configTICK_RATE_HZ                      ( 1000 )
#define configMINIMAL_STACK_SIZE                ( PTHREAD_STACK_MIN )
#define configMAX_TASK_NAME_LEN                 ( 16 )
#define configUSE_16_BIT_TICKS                  0
#define configIDLE_SHOULD_YIELD                 1
#define configUSE_MUTEXES                       1
#define configUSE_RECURSIVE_MUTEXES             0
#define configUSE_COUNTING_SEMAPHORES           1
#define configQUEUE_REGISTRY_SIZE               8
#define configUSE_TIME_SLICING                  1
#define configCHECK_FOR_STACK_OVERFLOW          0

#define configSUPPORT_STATIC_ALLOCATION         0
#define configSUPPORT_DYNAMIC_ALLOCATION        1
// The POSIX port's "stack" for a task is just a pvPortMalloc'd buffer sized
// configMINIMAL_STACK_SIZE * sizeof(StackType_t) -- with
// configMINIMAL_STACK_SIZE == PTHREAD_STACK_MIN (16384) and StackType_t as
// unsigned long, that's 128KiB per task, so the heap needs room for several.
#define configTOTAL_HEAP_SIZE                   ( 2 * 1024 * 1024 )

#define configMAX_PRIORITIES                    ( 5 )
#define configUSE_TIMERS                        0

#define configUSE_TASK_NOTIFICATIONS            1
#define configUSE_APPLICATION_TASK_TAG          0
#define configUSE_QUEUE_SETS                    0
#define configUSE_TRACE_FACILITY                0
#define configUSE_STATS_FORMATTING_FUNCTIONS    0
#define configGENERATE_RUN_TIME_STATS           0
#define configRECORD_STACK_HIGH_ADDRESS         1
#define configSTACK_DEPTH_TYPE                  uint32_t

#define configUSE_CO_ROUTINES                   0
#define configMAX_CO_ROUTINE_PRIORITIES         1

#define INCLUDE_vTaskDelay                      1
#define INCLUDE_vTaskDelete                     1
#define INCLUDE_vTaskSuspend                    1
#define INCLUDE_xTaskGetSchedulerState          1
#define INCLUDE_eTaskGetState                   1
