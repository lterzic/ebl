#include "ebl/measure.h"

void ebl_region_init(struct ebl_region *region, int counters)
{
    region->counters = counters & ebl_counters_arch_init();
    region->count = 0;
    for (int i = 0; i < EBL_NUM_COUNTERS; i++) {
        region->min[i] = (ebl_counter_t)-1;
        region->max[i] = 0;
        region->sum[i] = 0;
    }
}

void ebl_region_start(struct ebl_region *region)
{
    ebl_counters_arch_save(&region->start);
}

void ebl_region_end(struct ebl_region *region)
{
    struct ebl_snapshot end;
    ebl_counters_arch_save(&end);

    for (int i = 0; i < EBL_NUM_COUNTERS; i++) {
        if (!(region->counters & (1 << i)))
            continue;

        // Overflow/reset of a counter across start->end isn't detected yet;
        // this assumes it never wraps past the full counter width.
        ebl_counter_t delta = end.counters[i] - region->start.counters[i];
        if (delta < region->min[i])
            region->min[i] = delta;
        if (delta > region->max[i])
            region->max[i] = delta;
        region->sum[i] += delta;
    }

    region->count++;
}

void ebl_state_init(struct ebl_state *state, int counters, size_t warmup, size_t measure)
{
    ebl_region_init(&state->region, counters);
    state->warmup_iters = warmup;
    state->measure_iters = measure;
    state->iter = 0;
}

bool ebl_measure(struct ebl_state *state)
{
    if (state->iter > state->warmup_iters) {
        ebl_region_end(&state->region);
    }

    if (state->iter >= state->warmup_iters + state->measure_iters)
        return false;

    if (state->iter >= state->warmup_iters) {
        ebl_region_start(&state->region);
    }

    state->iter++;
    return true;
}
