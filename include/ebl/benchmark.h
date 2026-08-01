#pragma once

#include <stddef.h>

#include "ebl/measure.h"

#ifdef __cplusplus
extern "C" {
#endif

// Add a benchmark function to a list of benchmarks to be executed.
void ebl_register_bm(const char* name, void (*func)(struct ebl_state*),
                            int counters, size_t warmup, size_t measure);

// Run every registered benchmark and pass its result to ebl_report.
void ebl_run_all(void);

// Report the result of a region. Implemented by the platform, with
// a default implementation provided by ebl-stdio which prints to
// the standard output.
void ebl_report(const char* name, const struct ebl_region* region);

#ifdef __cplusplus
}
#endif

// Register a benchmark with specific counters.
#define EBL_BENCHMARK_COUNTERS(bm_func, warmup, measure, counters) \
    static void __attribute__((constructor)) ebl_call_register_bm_ ## bm_func (void) {\
        ebl_register_bm(#bm_func, &bm_func, counters, warmup, measure); }

#define EBL_COUNTER_ALL (EBL_COUNTER_CYCLES | EBL_COUNTER_TIME_NS | EBL_COUNTER_INSTRET)

// Register a benchmark with default counters.
#define EBL_BENCHMARK(bm_func, warmup, iters) \
    EBL_BENCHMARK_COUNTERS(bm_func, warmup, iters, EBL_COUNTER_ALL)
