# ebl

A small, Google-Benchmark-like microbenchmarking library for embedded and
low-resource targets. The core is freestanding C (no libc dependency) so it
can run on bare metal; hosted features like printing results are opt-in
separate libraries.

## Building

```sh
cmake -B build -S .
cmake --build build
./build/examples/basic/ebl-example-precise
./build/examples/basic/ebl-example-fast
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
- `ebl-stdio`, to get a default `printf`-based `ebl_report`, and
- exactly one of `ebl-measure-precise` or `ebl-measure-fast`, to pick a
  measurement mode.

Both `ebl-main`/`ebl-stdio` are separate libraries so a bare-metal target can
supply its own `main` and its own reporter (e.g. writing over a serial port)
instead.

`ebl-measure-precise` runs each sample three times (a single-run bracket plus
a double-run bracket) and subtracts the single-run delta from the double-run
delta to cancel the fixed overhead of the counter snapshots themselves.
`ebl-measure-fast` runs each sample once, at a third of the cost, but the
snapshot overhead leaks into every sample. Pick a mode per target — a quick
smoke benchmark might use `ebl-measure-fast` while a real measurement run
uses `ebl-measure-precise`.

`ebl_run_all()` warms up before each benchmark and reports the first warmup
as `warmup`, so you can see the measurement overhead itself before judging
any other benchmark's numbers.

`ebl-stdio`'s report header prints the architecture and measurement mode the
binary was built with (e.g. `arch: x86_64  mode: precise`), so results are
self-describing when comparing output from different builds.

Use `EBL_BENCHMARK_COUNTERS(bm_func, warmup, iters, counters)` instead of
`EBL_BENCHMARK` to select a subset of counters (`EBL_COUNTER_CYCLES`,
`EBL_COUNTER_TIME_NS`, `EBL_COUNTER_INSTRET`) instead of all of them.

## Status

- x86_64 and RISC-V (RV32/RV64, via the Zicntr extension) are supported;
  other architectures fail CMake configuration with a clear error rather
  than silently building without counters.
- `EBL_COUNTER_INSTRET` (instructions retired) is not implemented on x86_64
  yet — it needs `perf_event_open`, which is a bigger follow-up.
- `EBL_COUNTER_TIME_NS` is not implemented on RISC-V — Zicntr's `time` CSR
  ticks at a platform-defined timebase frequency rather than nanoseconds,
  and that frequency isn't discoverable from Zicntr itself.
- RISC-V's `rdcycle`/`rdinstret` require the counters to be enabled for the
  current privilege level (`mcounteren`/`scounteren`); a locked-down
  kernel or M-mode setup that hasn't enabled them will trap instead.
- Counter overflow across a region isn't detected; counters are assumed not
  to wrap during a single start/end measurement.
