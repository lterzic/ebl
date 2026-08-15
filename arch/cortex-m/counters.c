#include "ebl/counters.h"
#include "ebl/measure.h"

#include <stdint.h>

// DWT cycle counter (CYCCNT), gated by DEMCR.TRCENA -- present on
// Cortex-M3/M4/M7/M33/M35P; Cortex-M0/M0+/M1 have no DWT and so no cycle
// counter at all.
#define DEMCR      (*(volatile uint32_t *)0xE000EDFCu)
#define DWT_CTRL   (*(volatile uint32_t *)0xE0001000u)
#define DWT_CYCCNT (*(volatile uint32_t *)0xE0001004u)

#define DEMCR_TRCENA       (1u << 24)
#define DWT_CTRL_CYCCNTENA (1u << 0)

// Neither a program-order instruction counter nor a nanosecond-resolution
// clock is available from the core itself (DWT's other counters are 8-bit
// profiling tallies, not a running instruction count; timing would need a
// vendor-specific timer), so only EBL_COUNTER_CYCLES is supported here.
int ebl_counters_arch_init(ebl_counter_mask_t counters)
{
    DEMCR |= DEMCR_TRCENA;
    DWT_CTRL |= DWT_CTRL_CYCCNTENA;
    return counters & EBL_COUNTER_CYCLES;
}

static ebl_counter_t read_cycle(void)
{
    return (ebl_counter_t)DWT_CYCCNT;
}

static void save_snapshot(struct ebl_snapshot *snapshot)
{
    snapshot->counters[0] = read_cycle();
    snapshot->counters[1] = 0;
    snapshot->counters[2] = 0;
}

void ebl_counters_arch_save_start(struct ebl_snapshot *snapshot)
{
    save_snapshot(snapshot);
}

void ebl_counters_arch_save_end(struct ebl_snapshot *snapshot)
{
    save_snapshot(snapshot);
}
