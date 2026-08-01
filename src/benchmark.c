#include "ebl/benchmark.h"
#include "ebl/measure.h"
#include <stddef.h>

#ifndef EBL_MAX_BENCHMARKS
#define EBL_MAX_BENCHMARKS 64
#endif

struct ebl_benchmark {
    const char* name;
    void (*func)(struct ebl_state*);
    int counters;
    size_t warmup_iters;
    size_t measure_iters;
};

static struct ebl_benchmark s_registered[EBL_MAX_BENCHMARKS];
static size_t s_nreg = 0;

void ebl_register_bm(const char *name, void (*func)(struct ebl_state *), int counters, size_t warmup, size_t measure)
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

void ebl_run_all(void)
{
    for (size_t i = 0; i < s_nreg; i++) {
        struct ebl_state state;
        struct ebl_benchmark* bm = &s_registered[i];
        ebl_state_init(&state, bm->counters, bm->warmup_iters, bm->measure_iters);
        bm->func(&state);
        ebl_report(bm->name, &state.region);
    }
}