#include "ebl/benchmark.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"

#include <stdatomic.h>

static void bm_queue_uncontended(struct ebl_state *state)
{
    QueueHandle_t queue = xQueueCreate(1, sizeof(int));
    int item = 42;

    while (ebl_measure(state)) {
        xQueueSend(queue, &item, 0);
        xQueueReceive(queue, &item, 0);
    }

    vQueueDelete(queue);
}
EBL_BENCHMARK(bm_queue_uncontended, 200, 2000)

struct queue_echo_ctx {
    QueueHandle_t     request;
    QueueHandle_t     response;
    SemaphoreHandle_t started;
    SemaphoreHandle_t stopped;
    atomic_bool       stop;
};

// Bounces items back on a second queue so bm_queue_roundtrip captures a full
// producer -> consumer -> producer trip, including cross-task wake latency,
// not just a same-task push+pop.
static void echo_task(void *param)
{
    struct queue_echo_ctx *ctx = param;
    int item;

    xSemaphoreGive(ctx->started);
    while (!atomic_load(&ctx->stop)) {
        if (xQueueReceive(ctx->request, &item, pdMS_TO_TICKS(50)) == pdTRUE)
            xQueueSend(ctx->response, &item, portMAX_DELAY);
    }
    xSemaphoreGive(ctx->stopped);
    vTaskDelete(NULL);
}

static void bm_queue_roundtrip(struct ebl_state *state)
{
    struct queue_echo_ctx ctx = {
        .request  = xQueueCreate(1, sizeof(int)),
        .response = xQueueCreate(1, sizeof(int)),
        .started  = xSemaphoreCreateBinary(),
        .stopped  = xSemaphoreCreateBinary(),
        .stop     = false,
    };
    xTaskCreate(echo_task, "queue_echo", configMINIMAL_STACK_SIZE, &ctx,
                tskIDLE_PRIORITY + 1, NULL);
    xSemaphoreTake(ctx.started, portMAX_DELAY);

    int item = 0;
    while (ebl_measure(state)) {
        xQueueSend(ctx.request, &item, portMAX_DELAY);
        xQueueReceive(ctx.response, &item, portMAX_DELAY);
    }

    atomic_store(&ctx.stop, true);
    xSemaphoreTake(ctx.stopped, portMAX_DELAY);
    vSemaphoreDelete(ctx.started);
    vSemaphoreDelete(ctx.stopped);
    vQueueDelete(ctx.request);
    vQueueDelete(ctx.response);
}
EBL_BENCHMARK(bm_queue_roundtrip, 200, 2000)
