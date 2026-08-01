#pragma once

#if defined(__GNUC__) || defined(__clang__)

// Forces `value` to be materialized in a register or memory, so a compiler
// can't prove it's dead and optimize away the computation that produced it.
// `value` must be a plain modifiable variable (not an expression): a pure
// input constraint here isn't enough -- GCC can still prove the load feeding
// an otherwise-unused input is dead and drop it entirely, so this is marked
// as read-write ("+") to force the load to actually happen.
#define ebl_do_not_optimize(value) __asm__ volatile("" : "+r,m"(value) : : "memory")

// A full compiler barrier: forces prior memory accesses to complete, and
// prevents the compiler from assuming memory is unchanged across this point
// (e.g. hoisting a load, or eliding a store, across loop iterations).
static inline void ebl_clobber(void)
{
    __asm__ volatile("" : : : "memory");
}

#else
#error "ebl_do_not_optimize/ebl_clobber not implemented for this compiler"
#endif