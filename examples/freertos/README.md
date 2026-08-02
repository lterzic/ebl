# FreeRTOS mutex/queue benchmarks

Benchmarks FreeRTOS's mutex and queue primitives -- uncontended lock/unlock
and push/pop, plus contended/multi-task variants -- using FreeRTOS's POSIX
simulator (`FREERTOS_PORT=GCC_POSIX`) so the existing `ebl` x86_64 counter
backend can be reused as-is.

Off by default; enable with:

```sh
cmake -B build -S . -DEBL_BUILD_FREERTOS_EXAMPLE=ON
cmake --build build
./build/examples/freertos/ebl-freertos-example
```

## Caveats

- This runs FreeRTOS's Linux POSIX simulator, not real hardware: each
  FreeRTOS task is a pthread, and tick/context-switch handling goes through
  POSIX signals. Absolute numbers reflect Linux scheduling behavior, not
  deterministic hard-real-time timing -- useful for relative comparisons
  (contended vs. uncontended, queue vs. mutex), not for judging real RTOS
  latency bounds.
- The signal-driven scheduler adds jitter to individual iterations, so
  min/mean/max spread is wider here than in the CPU-bound examples in
  `examples/basic/basic.c` and `examples/basic/matrix.cpp`.
