#include "ebl/counters.h"
#include "ebl/measure.h"

#include <time.h>
#include <x86intrin.h>

// perf_event_open-based instruction counting would add a read() syscall per
// sample, which would dominate the overhead of the tight loops this library
// measures -- left unsupported for now.
int ebl_counters_arch_init(ebl_counter_mask_t counters)
{
    return counters & (EBL_COUNTER_CYCLES | EBL_COUNTER_TIME_NS);
}

static ebl_counter_t read_cycles(void)
{
    // Fence before reading so out-of-order execution of surrounding code
    // can't leak into the measured window.
    _mm_lfence();
    return __rdtsc();
}

static ebl_counter_t read_time_ns(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (ebl_counter_t)ts.tv_sec * 1000000000ull + (ebl_counter_t)ts.tv_nsec;
}

void ebl_counters_arch_save(struct ebl_snapshot *snapshot)
{
    snapshot->counters[0] = read_cycles();
    snapshot->counters[1] = read_time_ns();
    snapshot->counters[2] = 0;
}
