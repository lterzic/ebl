#include "ebl/benchmark.h"
#include "ebl/utils.h"

#include <cstddef>

static constexpr size_t N = 32;

template <typename T>
static void matmul(const T *a, const T *b, T *c)
{
    for (size_t i = 0; i < N; i++) {
        for (size_t j = 0; j < N; j++) {
            T sum{};
            for (size_t k = 0; k < N; k++)
                sum += a[i * N + k] * b[k * N + j];
            c[i * N + j] = sum;
        }
    }
}

template <typename T>
static void bm_matmul(struct ebl_state *state)
{
    static T a[N * N];
    static T b[N * N];
    static T c[N * N];
    for (size_t i = 0; i < N * N; i++) {
        a[i] = static_cast<T>(i % 7);
        b[i] = static_cast<T>(i % 5);
    }

    while (ebl_measure(state)) {
        matmul<T>(a, b, c);
        // do_not_optimize(c[0]) would only escape one element, letting the
        // compiler prove the rest of c is dead and drop the O(N^3) work; c's
        // address is otherwise never taken, so escape the whole array. The
        // clobber then forces a and b to be reread each iteration too,
        // instead of the whole computation being hoisted as loop-invariant.
        ebl_do_not_optimize(c);
        ebl_clobber();
    }
}

static void bm_matmul_int(struct ebl_state *state) { bm_matmul<int>(state); }
EBL_BENCHMARK(bm_matmul_int, 5, 50)

static void bm_matmul_float(struct ebl_state *state) { bm_matmul<float>(state); }
EBL_BENCHMARK(bm_matmul_float, 5, 50)

static void bm_matmul_double(struct ebl_state *state) { bm_matmul<double>(state); }
EBL_BENCHMARK(bm_matmul_double, 5, 50)
