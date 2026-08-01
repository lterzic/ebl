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

#define EBL_COUNTER_ALL (EBL_COUNTER_CYCLES | EBL_COUNTER_TIME_NS | EBL_COUNTER_INSTRET)

struct ebl_snapshot {
    ebl_counter_t counters[EBL_NUM_COUNTERS];
};

struct ebl_region {
    int                 counters;
    size_t              count;
    struct ebl_snapshot start;
    ebl_counter_t       min[EBL_NUM_COUNTERS];
    ebl_counter_t       max[EBL_NUM_COUNTERS];
    ebl_counter_t       sum[EBL_NUM_COUNTERS];
};

struct ebl_state {
    struct ebl_region   region;
    size_t              warmup_iters;
    size_t              measure_iters;
    size_t              iter;
};


// Arch defined.
int  ebl_counters_arch_init(void);
void ebl_counters_arch_save(struct ebl_snapshot* snapshot);


void ebl_region_init(struct ebl_region* region, int counters);
void ebl_region_start(struct ebl_region* region);
void ebl_region_end(struct ebl_region* region);

void ebl_state_init(struct ebl_state* state, int counters, size_t warmup, size_t measure);
bool ebl_measure(struct ebl_state* state);
