#include "ebl/measure.h"

static void ebl_region_init(struct ebl_region *region, int counters)
{
    region->counters = counters & ebl_counters_arch_init();
    region->count = 0;
    for (int i = 0; i < EBL_NUM_COUNTERS; i++) {
        region->min.counters[i] = (ebl_counter_t)-1;
        region->max.counters[i] = 0;
        region->sum.counters[i] = 0;
    }
}

static void ebl_snapshot_delta(struct ebl_snapshot *delta, const struct ebl_snapshot *end,
                                const struct ebl_snapshot *start)
{
    for (int i = 0; i < EBL_NUM_COUNTERS; i++)
        // Overflow/reset of a counter across start->end isn't detected yet;
        // this assumes it never wraps past the full counter width.
        delta->counters[i] = end->counters[i] - start->counters[i];
}

static void ebl_region_update(struct ebl_region *region, const struct ebl_snapshot *delta)
{
    for (int i = 0; i < EBL_NUM_COUNTERS; i++) {
        if (!(region->counters & (1 << i)))
            continue;

        if (delta->counters[i] < region->min.counters[i])
            region->min.counters[i] = delta->counters[i];
        if (delta->counters[i] > region->max.counters[i])
            region->max.counters[i] = delta->counters[i];
        region->sum.counters[i] += delta->counters[i];
    }

    region->count++;
}

void ebl_state_init(struct ebl_state *state, int counters, size_t warmup, size_t measure)
{
    ebl_region_init(&state->region, counters);
    state->warmup_iters = warmup;
    state->measure_iters = measure;
    state->iter = 0;
    state->phase = 0;
}

bool ebl_measure(struct ebl_state *state)
{
    if (state->iter < state->warmup_iters) {
        state->iter++;
        return true;
    }

    for (;;) {
        switch (state->phase) {
        case 0: // about to start the single-run bracket
            if (state->iter - state->warmup_iters >= state->measure_iters)
                return false;
            ebl_counters_arch_save(&state->start);
            state->phase = 1;
            return true;

        case 1: { // single-run done; record it, start the double-run bracket at the same instant
            struct ebl_snapshot mid;
            ebl_counters_arch_save(&mid);
            ebl_snapshot_delta(&state->single, &mid, &state->start);
            state->start = mid;
            state->phase = 2;
            return true;
        }

        case 2: // first of the double-run's two executions done; run the second
            state->phase = 3;
            return true;

        default: { // case 3: double-run done; subtract the single-run to cancel fixed overhead
            struct ebl_snapshot end, doubled, delta;
            ebl_counters_arch_save(&end);
            ebl_snapshot_delta(&doubled, &end, &state->start);
            ebl_snapshot_delta(&delta, &doubled, &state->single);

            // Noise can make the double-run bracket read below the
            // single-run one; ebl_counter_t is unsigned so an un-clamped
            // subtraction would wrap into a huge bogus value.
            for (int i = 0; i < EBL_NUM_COUNTERS; i++) {
                if (doubled.counters[i] < state->single.counters[i])
                    delta.counters[i] = 0;
            }

            ebl_region_update(&state->region, &delta);
            state->iter++;
            state->phase = 0;
            continue;
        }
        }
    }
}
