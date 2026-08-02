#include "ebl/benchmark.h"

#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

#include <stdatomic.h>

static void bm_mutex_uncontended(struct ebl_state *state)
{
    SemaphoreHandle_t mutex = xSemaphoreCreateMutex();

    while (ebl_measure(state)) {
        xSemaphoreTake(mutex, portMAX_DELAY);
        xSemaphoreGive(mutex);
    }

    vSemaphoreDelete(mutex);
}
EBL_BENCHMARK(bm_mutex_uncontended, 200, 2000)

struct mutex_contender_ctx {
    SemaphoreHandle_t mutex;
    SemaphoreHandle_t started;
    SemaphoreHandle_t stopped;
    atomic_bool       stop;
};

// Fights the measured task for the same mutex so bm_mutex_contended sees
// realistic blocked-acquisition latency instead of the uncontended fast path.
static void contender_task(void *param)
{
    struct mutex_contender_ctx *ctx = param;

    xSemaphoreGive(ctx->started);
    while (!atomic_load(&ctx->stop)) {
        xSemaphoreTake(ctx->mutex, portMAX_DELAY);
        xSemaphoreGive(ctx->mutex);
    }
    xSemaphoreGive(ctx->stopped);
    vTaskDelete(NULL);
}

static void bm_mutex_contended(struct ebl_state *state)
{
    struct mutex_contender_ctx ctx = {
        .mutex   = xSemaphoreCreateMutex(),
        .started = xSemaphoreCreateBinary(),
        .stopped = xSemaphoreCreateBinary(),
        .stop    = false,
    };
    xTaskCreate(contender_task, "mutex_contender", configMINIMAL_STACK_SIZE,
                &ctx, tskIDLE_PRIORITY + 1, NULL);
    xSemaphoreTake(ctx.started, portMAX_DELAY);

    while (ebl_measure(state)) {
        xSemaphoreTake(ctx.mutex, portMAX_DELAY);
        xSemaphoreGive(ctx.mutex);
    }

    atomic_store(&ctx.stop, true);
    xSemaphoreTake(ctx.stopped, portMAX_DELAY);
    vSemaphoreDelete(ctx.started);
    vSemaphoreDelete(ctx.stopped);
    vSemaphoreDelete(ctx.mutex);
}
EBL_BENCHMARK(bm_mutex_contended, 200, 2000)
