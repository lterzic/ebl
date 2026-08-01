#include "ebl/benchmark.h"

#include <inttypes.h>
#include <stdio.h>

#define NAME_WIDTH     20
#define ITERS_WIDTH    8
#define SUBFIELD_WIDTH 10
#define NUM_WIDTH      (SUBFIELD_WIDTH * 3 + 2) // 3 subfields + 2 separating spaces

// ebl_counter_t is uint32_t or uint64_t depending on the target (see
// measure.h) -- match the printf conversion to it instead of widening to
// uint64_t/PRIu64, which would drag in 64-bit printf support unconditionally
// even on 32-bit targets (e.g. an MCU) that only need 32-bit formatting.
#if EBL_COUNTER_32BIT
#define EBL_COUNTER_FMT PRIu32
#elif EBL_COUNTER_64BIT
#define EBL_COUNTER_FMT PRIu64
#endif

static const char *counter_name(int index)
{
    switch (1 << index) {
    case EBL_COUNTER_CYCLES:  return "cycles";
    case EBL_COUNTER_TIME_NS: return "time_ns";
    case EBL_COUNTER_INSTRET: return "instret";
    default:                  return "unknown";
    }
}

static void print_header(void)
{
    printf("%-*s %*s", NAME_WIDTH, "", ITERS_WIDTH, "");
    for (int i = 0; i < EBL_NUM_COUNTERS; i++)
        printf(" %*s", NUM_WIDTH, counter_name(i));
    printf("\n");

    printf("%-*s %*s", NAME_WIDTH, "name", ITERS_WIDTH, "iters");
    for (int i = 0; i < EBL_NUM_COUNTERS; i++)
        printf(" %*s %*s %*s", SUBFIELD_WIDTH, "min", SUBFIELD_WIDTH, "mean", SUBFIELD_WIDTH, "max");
    printf("\n");
}

void ebl_report(const char *name, const struct ebl_region *region)
{
    // Regions are reported one at a time as each benchmark finishes, so the
    // header is only known to be complete (covers every counter) on the
    // first call.
    static int header_printed = 0;
    if (!header_printed) {
        print_header();
        header_printed = 1;
    }

    printf("%-*s %*zu", NAME_WIDTH, name, ITERS_WIDTH, region->count);
    for (int i = 0; i < EBL_NUM_COUNTERS; i++) {
        if (!(region->counters & (1 << i))) {
            printf(" %*s", NUM_WIDTH, "-");
            continue;
        }

        double mean = region->count ? (double)region->sum[i] / region->count : 0.0;
        printf(" %*" EBL_COUNTER_FMT " %*.2f %*" EBL_COUNTER_FMT,
               SUBFIELD_WIDTH, region->min[i], SUBFIELD_WIDTH, mean, SUBFIELD_WIDTH, region->max[i]);
    }
    printf("\n");
}
