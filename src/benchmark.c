#include "ebl/benchmark.h"
#include "ebl/counters.h"
#include "ebl/measure.h"
#include <stdbool.h>
#include <stddef.h>

#ifndef EBL_MAX_BENCHMARKS
#define EBL_MAX_BENCHMARKS 64
#endif

#ifndef EBL_WARMUP_ITERS
#define EBL_WARMUP_ITERS 100
#endif

struct ebl_benchmark {
    const char* name;
    void (*func)(struct ebl_state*);
    ebl_counter_mask_t counters;
    size_t warmup_iters;
    size_t measure_iters;
};

static struct ebl_benchmark s_registered[EBL_MAX_BENCHMARKS];
static size_t s_nreg = 0;

void ebl_register_bm(const char *name, void (*func)(struct ebl_state *),
                     ebl_counter_mask_t counters, size_t warmup, size_t measure)
{
    if (s_nreg >= EBL_MAX_BENCHMARKS) {
        // No hosted abort/exit available in a freestanding build -- trap
        // instead of silently dropping the registration.
        __builtin_trap();
    }

    s_registered[s_nreg].name = name;
    s_registered[s_nreg].func = func;
    s_registered[s_nreg].counters = counters;
    s_registered[s_nreg].warmup_iters = warmup;
    s_registered[s_nreg].measure_iters = measure;
    s_nreg++;
}

// Throwaway noop region to warm up the icache/branch predictor/CPU clock.
static void warmup(bool report)
{
    struct ebl_state state;
    ebl_state_init(&state, EBL_COUNTER_ALL, 0, EBL_WARMUP_ITERS);
    while (ebl_measure(&state)) {
    }

    if (report)
        ebl_report("warmup", &state.region);
}

void ebl_run_all(void)
{
    // Report the first warmup so its overhead is visible.
    warmup(true);

    for (size_t i = 0; i < s_nreg; i++) {
        // Create consistent cache and branch predictor state for each benchmark.
        warmup(false);

        struct ebl_state state;
        struct ebl_benchmark* bm = &s_registered[i];
        ebl_state_init(&state, bm->counters, bm->warmup_iters, bm->measure_iters);
        bm->func(&state);
        ebl_report(bm->name, &state.region);
    }
}