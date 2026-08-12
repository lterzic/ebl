#include "ebl/counters.h"
#include "ebl/measure.h"

// Zicntr's `time` CSR ticks at a platform-defined timebase frequency, not
// nanoseconds, and that frequency isn't discoverable from Zicntr itself --
// EBL_COUNTER_TIME_NS is left unsupported for now, same as x86_64 leaves
// EBL_COUNTER_INSTRET unsupported.
int ebl_counters_arch_init(ebl_counter_mask_t counters)
{
    return counters & (EBL_COUNTER_CYCLES | EBL_COUNTER_INSTRET);
}

static ebl_counter_t read_cycle(void)
{
    unsigned long val;
    __asm__ volatile("rdcycle %0" : "=r"(val));
    return (ebl_counter_t)val;
}

static ebl_counter_t read_instret(void)
{
    unsigned long val;
    __asm__ volatile("rdinstret %0" : "=r"(val));
    return (ebl_counter_t)val;
}

// Unlike x86_64's TSC, RISC-V instructions retire in program order even on
// out-of-order implementations, so rdcycle/rdinstret don't need fences to
// keep them from being reordered around the timed code -- one helper
// suffices for both the start and end snapshot.
static void save_snapshot(struct ebl_snapshot *snapshot)
{
    snapshot->counters[0] = read_cycle();
    snapshot->counters[1] = 0;
    snapshot->counters[2] = read_instret();
}

void ebl_counters_arch_save_start(struct ebl_snapshot *snapshot)
{
    save_snapshot(snapshot);
}

void ebl_counters_arch_save_end(struct ebl_snapshot *snapshot)
{
    save_snapshot(snapshot);
}
