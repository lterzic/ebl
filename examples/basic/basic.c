#include "ebl/benchmark.h"
#include "ebl/utils.h"

static int g_data = 42;

// Measures ebl_measure()'s own overhead -- the noise floor every other
// benchmark's numbers should be read against.
static void bm_noop(struct ebl_state *state)
{
    while (ebl_measure(state)) {
    }
}
EBL_BENCHMARK(bm_noop, 0, 1000)

static void bm_read(struct ebl_state *state)
{
    while (ebl_measure(state)) {
        int v = g_data;
        ebl_do_not_optimize(v);
    }
}
EBL_BENCHMARK(bm_read, 0, 1000)

static void bm_write(struct ebl_state *state)
{
    while (ebl_measure(state)) {
        g_data = 42;
        ebl_clobber();
    }
}
EBL_BENCHMARK(bm_write, 0, 1000)
