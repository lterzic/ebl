# ebl

A small, Google-Benchmark-like microbenchmarking library for embedded and
low-resource targets. The core is freestanding C (no libc dependency) so it
can run on bare metal; hosted features like printing results are opt-in
separate libraries.

## Building

```sh
cmake -B build -S .
cmake --build build
./build/examples/basic/ebl-example
```

## Writing a benchmark

```c
#include "ebl/benchmark.h"

static void bm_something(struct ebl_state *state)
{
    while (ebl_measure(state)) {
        // code under test
    }
}
EBL_BENCHMARK(bm_something, /*warmup=*/5, /*iters=*/1000)
```

Benchmarks self-register at startup (via a constructor function) — no need
to list them anywhere. Link against `ebl-benchmark` plus:
- `ebl-main`, to get a `main()` that runs everything, and/or
- `ebl-stdio`, to get a default `printf`-based `ebl_report`.

Both are separate libraries so a bare-metal target can supply its own `main`
and its own reporter (e.g. writing over a serial port) instead.

`ebl_run_all()` warms up before each benchmark and reports the first warmup
as `warmup`, so you can see the measurement overhead itself before judging
any other benchmark's numbers.

Use `EBL_BENCHMARK_COUNTERS(bm_func, warmup, iters, counters)` instead of
`EBL_BENCHMARK` to select a subset of counters (`EBL_COUNTER_CYCLES`,
`EBL_COUNTER_TIME_NS`, `EBL_COUNTER_INSTRET`) instead of all of them.

## Status

- x86_64 only; other architectures fail CMake configuration with a clear
  error rather than silently building without counters.
- `EBL_COUNTER_INSTRET` (instructions retired) is not implemented on x86_64
  yet — it needs `perf_event_open`, which is a bigger follow-up.
- Counter overflow across a region isn't detected; counters are assumed not
  to wrap during a single start/end measurement.
