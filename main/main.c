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

#include <hagl.h>
#include <hagl_hal.h>
#include <rgb565.h>
#include <font6x9.h>
#include <fps.h>

#include "metaballs.h"

static const char *TAG = "main";
static EventGroupHandle_t event;
static float fb_fps;
static bitmap_t *bb;

static const uint8_t RENDER_FINISHED = (1 << 0);

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

void demo_task(void *params)
{
    uint16_t green = rgb565(0, 255, 0);
    char16_t message[128];

    struct settings settings;
    settings.num = 5;
    settings.radius.min = 12;
    settings.radius.max = 18;
    settings.velocity.min = 3;
    settings.velocity.max = 5;
    settings.min.x = 0;
    settings.min.y = 0;
    settings.max.x = DISPLAY_WIDTH - 1;
    settings.max.y = DISPLAY_HEIGHT - 1;

    settings.color[0] = rgb565(255, 255, 255);
    settings.color[1] = rgb565(210, 210, 210);
    settings.color[2] = rgb565(150, 150, 150);
    metaballs_init(settings);

    while (1) {
        hagl_clear_clip_window();
        metaballs_animate();
        metaballs_render();
        xEventGroupSetBits(event, RENDER_FINISHED);

        hagl_set_clip_window(0, 0, DISPLAY_WIDTH - 1, DISPLAY_HEIGHT - 1);
        swprintf(message, sizeof(message), u"%d METABALLS  ", settings.num);
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
    xTaskCreatePinnedToCore(demo_task, "Demo", 4096, NULL, 1, NULL, 1);
}
