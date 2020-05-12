/*

MIT No Attribution

Copyright (c) 2020 Mika Tuupola

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

-cut-

SPDX-License-Identifier: MIT-0

*/

#include "sdkconfig.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>
#include <esp_log.h>

#include <hagl_hal.h>
#include <hagl.h>
#include <font6x9.h>
#include <fps.h>

#include "alien.h"
#include "metaballs.h"
#include "plasma.h"
#include "rotozoom.h"

static const char *TAG = "main";
static EventGroupHandle_t event;
static float fb_fps;
static bitmap_t *bb;
static uint8_t effect = 0;

static const uint8_t RENDER_FINISHED = (1 << 0);

static char demo[3][32] = {
    "5 METABALLS   ",
    "PALETTE PLASMA",
    "ROTOZOOM      ",
};

void flush_task(void *params)
{
    while (1) {
        EventBits_t bits = xEventGroupWaitBits(
            event,
            RENDER_FINISHED,
            pdTRUE,
            pdFALSE,
            0
        );

        if ((bits & RENDER_FINISHED) != 0 ) {
            hagl_flush();
            fb_fps = fps();
        }
    }

    vTaskDelete(NULL);
}

void switch_task(void *params)
{
    while (1) {
        effect = (effect + 1) % 3;
        hagl_clear_clip_window();

        vTaskDelay(10000 / portTICK_RATE_MS);
    }

    vTaskDelete(NULL);
}

void demo_task(void *params)
{
    color_t green = hagl_color(0, 255, 0);
    char16_t message[128];

    metaballs_init();
    plasma_init();

    while (1) {
        switch(effect) {
        case 0:
            hagl_clear_clip_window();
            metaballs_animate();
            metaballs_render();
            break;
        case 1:
            plasma_animate();
            plasma_render();
            break;
        case 2:
            rotozoom_animate();
            rotozoom_render();
            break;
        }
        xEventGroupSetBits(event, RENDER_FINISHED);

        swprintf(message, sizeof(message), u"%s    ", demo[effect]);
        hagl_set_clip_window(0, 0, DISPLAY_WIDTH - 1, DISPLAY_HEIGHT - 1);
        hagl_put_text(message, 4, 4, green, font6x9);

        swprintf(message, sizeof(message), u"%.*f FPS  ", 0, fb_fps);
        hagl_put_text(message, DISPLAY_WIDTH - 40, DISPLAY_HEIGHT - 14, green, font6x9);
        hagl_set_clip_window(0, 20, DISPLAY_WIDTH - 1, DISPLAY_HEIGHT - 21);
    }

    vTaskDelete(NULL);
}

void app_main()
{
    ESP_LOGI(TAG, "SDK version: %s", esp_get_idf_version());
    ESP_LOGI(TAG, "Heap when starting: %d", esp_get_free_heap_size());

    event = xEventGroupCreate();

    bb = hagl_init();
    if (bb) {
        ESP_LOGI(TAG, "Back buffer: %dx%dx%d", bb->width, bb->height, bb->depth);
    }

    hagl_set_clip_window(0, 20, DISPLAY_WIDTH - 1, DISPLAY_HEIGHT - 21);

    ESP_LOGI(TAG, "Heap after HAGL init: %d", esp_get_free_heap_size());

#ifdef HAGL_HAL_USE_BUFFERING
    xTaskCreatePinnedToCore(flush_task, "Framebuffer", 8192, NULL, 1, NULL, 0);
#endif
    xTaskCreatePinnedToCore(demo_task, "Demo", 8192, NULL, 1, NULL, 1);
    xTaskCreatePinnedToCore(switch_task, "Switch", 2048, NULL, 2, NULL, 1);
}
