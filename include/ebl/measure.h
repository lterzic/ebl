#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#if EBL_COUNTER_32BIT
    typedef uint32_t ebl_counter_t;
#elif EBL_COUNTER_64BIT
    typedef uint64_t ebl_counter_t;
#else
    #error "Architecture counter size not defined"
#endif

#define EBL_NUM_COUNTERS 3

enum ebl_counter {
    EBL_COUNTER_CYCLES  = 1 << 0,
    EBL_COUNTER_TIME_NS = 1 << 1,
    EBL_COUNTER_INSTRET = 1 << 2,
};

struct ebl_snapshot {
    ebl_counter_t counters[EBL_NUM_COUNTERS];
};

struct ebl_region {
    int                 counters;
    size_t              count;
    struct ebl_snapshot min;
    struct ebl_snapshot max;
    struct ebl_snapshot sum;
};

struct ebl_state {
    struct ebl_region   region;
    struct ebl_snapshot start;
    size_t              warmup_iters;
    size_t              measure_iters;
    size_t              iter;
};


#ifdef __cplusplus
extern "C" {
#endif

// Arch defined.
int  ebl_counters_arch_init(void);
void ebl_counters_arch_save(struct ebl_snapshot* snapshot);

void ebl_state_init(struct ebl_state* state, int counters, size_t warmup, size_t measure);
bool ebl_measure(struct ebl_state* state);

#ifdef __cplusplus
}
#endif
