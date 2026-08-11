#include "ebl/measure.h"
#include "ebl/counters.h"

#if defined(EBL_MEASURE_FAST) && defined(EBL_MEASURE_PRECISE)
#error "define at most one of EBL_MEASURE_FAST or EBL_MEASURE_PRECISE"
#endif

static void ebl_region_init(struct ebl_region *region, ebl_counter_mask_t counters)
{
    region->counter_mask = ebl_counters_arch_init(counters);
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
        if (!(region->counter_mask & (1 << i)))
            continue;

        if (delta->counters[i] < region->min.counters[i])
            region->min.counters[i] = delta->counters[i];
        if (delta->counters[i] > region->max.counters[i])
            region->max.counters[i] = delta->counters[i];
        region->sum.counters[i] += delta->counters[i];
    }

    region->count++;
}

void ebl_state_init(struct ebl_state *state, ebl_counter_mask_t counters, size_t warmup, size_t measure)
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
        case 0: // about to start a bracket
            if (state->iter - state->warmup_iters >= state->measure_iters)
                return false;
            ebl_counters_arch_save_start(&state->start);
            state->phase = 1;
            return true;

#ifdef EBL_MEASURE_FAST
        default: { // case 1: bracket done; the fixed snapshot overhead is not cancelled
            struct ebl_snapshot end, delta;
            ebl_counters_arch_save_end(&end);
            ebl_snapshot_delta(&delta, &end, &state->start);
            ebl_region_update(&state->region, &delta);
            state->iter++;
            state->phase = 0;
            continue;
        }
#else
        case 1: { // single-run done; record it, then start the double-run bracket
            struct ebl_snapshot mid_end;
            ebl_counters_arch_save_end(&mid_end);
            ebl_snapshot_delta(&state->single, &mid_end, &state->start);
            ebl_counters_arch_save_start(&state->start);
            state->phase = 2;
            return true;
        }

        case 2: // first of the double-run's two executions done; run the second
            state->phase = 3;
            return true;

        default: { // case 3: double-run done; subtract the single-run to cancel fixed overhead
            struct ebl_snapshot end, doubled, delta;
            ebl_counters_arch_save_end(&end);
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
#endif
        }
    }
}

const char *ebl_measure_mode_name(void)
{
#ifdef EBL_MEASURE_FAST
    return "fast";
#else
    return "precise";
#endif
}
