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

static ebl_counter_t read_cycles_start(void)
{
    // Fence before reading so the timed code that follows can't be
    // reordered ahead of the timestamp read.
    _mm_lfence();
    return __rdtsc();
}

static ebl_counter_t read_cycles_end(void)
{
    // rdtscp waits for prior (timed) instructions to retire before
    // reading the counter, unlike rdtsc which can be reordered earlier.
    // No trailing fence: every call site is followed by a start read
    // before more timed code runs, and that read's lfence already
    // blocks it from being reordered ahead of this one.
    unsigned int aux;
    return __rdtscp(&aux);
}

static ebl_counter_t read_time_ns(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (ebl_counter_t)ts.tv_sec * 1000000000ull + (ebl_counter_t)ts.tv_nsec;
}

void ebl_counters_arch_save_start(struct ebl_snapshot *snapshot)
{
    snapshot->counters[0] = read_cycles_start();
    snapshot->counters[1] = read_time_ns();
    snapshot->counters[2] = 0;
}

void ebl_counters_arch_save_end(struct ebl_snapshot *snapshot)
{
    snapshot->counters[0] = read_cycles_end();
    snapshot->counters[1] = read_time_ns();
    snapshot->counters[2] = 0;
}
