#pragma once

#include "counters.h"

#include <stddef.h>
#include <stdbool.h>

struct ebl_snapshot {
    ebl_counter_t counters[EBL_NUM_COUNTERS];
};

struct ebl_region {
    ebl_counter_mask_t  counter_mask;
    size_t              count;
    struct ebl_snapshot min;
    struct ebl_snapshot max;
    struct ebl_snapshot sum;
};

struct ebl_state {
    struct ebl_region   region;
    struct ebl_snapshot start;    // start of the current timing bracket (single- or double-run)
    struct ebl_snapshot single;   // pending single-run delta, held until the double-run completes
    size_t              warmup_iters;
    size_t              measure_iters;
    size_t              iter;
    size_t              phase;    // sub-step within the current sample: 0..3
};


#ifdef __cplusplus
extern "C" {
#endif

// Arch defined.
int  ebl_counters_arch_init(ebl_counter_mask_t counters);
void ebl_counters_arch_save(struct ebl_snapshot* snapshot);

void ebl_state_init(struct ebl_state* state, ebl_counter_mask_t counters, size_t warmup, size_t measure);
bool ebl_measure(struct ebl_state* state);

#ifdef __cplusplus
}
#endif
