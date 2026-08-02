#include "ebl/benchmark.h"

#include "FreeRTOS.h"
#include "task.h"

// ebl_run_all() must run from inside a FreeRTOS task, not from main() before
// the scheduler exists -- the benchmarks under test create/synchronize with
// other tasks, which requires the scheduler to already be running.
static void runner_task(void *params)
{
    (void)params;
    ebl_run_all();
    vTaskEndScheduler();
}

int main(void)
{
    xTaskCreate(runner_task, "ebl_runner", configMINIMAL_STACK_SIZE, NULL,
                tskIDLE_PRIORITY + 1, NULL);
    vTaskStartScheduler();
    return 0;
}
