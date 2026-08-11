#pragma once

#include <stdint.h>

#if EBL_COUNTER_32BIT
    typedef uint32_t ebl_counter_t;
#elif EBL_COUNTER_64BIT
    typedef uint64_t ebl_counter_t;
#else
    #error "Architecture counter size not defined"
#endif


enum ebl_counter {
    EBL_COUNTER_CYCLES  = 1 << 0,
    EBL_COUNTER_TIME_NS = 1 << 1,
    EBL_COUNTER_INSTRET = 1 << 2,
};

#define EBL_NUM_COUNTERS 3

typedef uint32_t ebl_counter_mask_t;
