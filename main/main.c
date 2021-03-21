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

#ifdef CONFIG_DEVICE_HAS_AXP192
#include <i2c_helper.h>
#include <axp192.h>
#endif /* CONFIG_DEVICE_HAS_AXP192 */
#ifdef CONFIG_DEVICE_HAS_AXP202
#include <i2c_helper.h>
#include <axp202.h>
#endif /* CONFIG_DEVICE_HAS_AXP202 */

#include <font6x9.h>
#include <aps.h>
#include <fps.h>
#include <hagl_hal.h>
#include <hagl.h>

#include "metaballs.h"
#include "plasma.h"
#include "rotozoom.h"
#include "deform.h"

static const char *TAG = "main";
static EventGroupHandle_t event;
static float fb_fps;
static float fb_bps;
static uint8_t effect = 0;

static const uint8_t RENDER_FINISHED = (1 << 0);
static const uint8_t FLUSH_STARTED= (1 << 1);

static char demo[4][32] = {
    "3 METABALLS   ",
    "PALETTE PLASMA",
    "ROTOZOOM      ",
    "PLANE DEFORM     ",
};

/*
 * Flushes the backbuffer to the display. Needed when using
 * double or triple buffering.
 */
void flush_task(void *params)
{
    while (1) {
        size_t bytes = 0;

        EventBits_t bits = xEventGroupWaitBits(
            event,
            RENDER_FINISHED,
            pdTRUE,
            pdFALSE,
            0
        );

        /* Flush only when RENDER_FINISHED is set. */
        if ((bits & RENDER_FINISHED) != 0 ) {
            xEventGroupSetBits(event, FLUSH_STARTED);
            bytes = hagl_flush();
            fb_bps = aps(bytes);
            fb_fps = fps();
        }
    }

    vTaskDelete(NULL);
}

/*
 * Software vsync. Waits for flush to start. Needed to avoid
 * tearing when using double buffering, NOP otherwise. This
 * could be handler with IRQ's if the display supports it.
 */
static void wait_for_vsync()
{
#ifdef CONFIG_HAGL_HAL_USE_DOUBLE_BUFFERING
    xEventGroupWaitBits(
        event,
        FLUSH_STARTED,
        pdTRUE,
        pdFALSE,
        10000 / portTICK_RATE_MS
    );
    ets_delay_us(18000);
#endif /* CONFIG_HAGL_HAL_USE_DOUBLE_BUFFERING */
}

/*
 * Changes the effect every 10 seconds.
 */
void switch_task(void *params)
{
    while (1) {
        /* Print the message in the console. */
        ESP_LOGI(TAG, "%s %.*f FPS", demo[effect], 1, fb_fps);

        hagl_clear_screen();
        hagl_flush();

        switch(effect) {
        case 0:
            //metaballs_close();
            break;
        case 1:
            plasma_close();
            break;
        case 2:
            //rotozoom_close();
            break;
        case 3:
            deform_close();
            break;
        }

        effect = (effect + 1) % 4;

        switch(effect) {
        case 0:
            metaballs_init();
            ESP_LOGI(TAG, "Heap after metaballs init: %d", esp_get_free_heap_size());
            break;
        case 1:
            plasma_init();
            ESP_LOGI(TAG, "Heap after plasma init: %d", esp_get_free_heap_size());
            break;
        case 2:
            rotozoom_init();
            ESP_LOGI(TAG, "Heap after rotozoom init: %d", esp_get_free_heap_size());
            break;
        case 3:
            deform_init();
            ESP_LOGI(TAG, "Heap after deform init: %d", esp_get_free_heap_size());
            break;
        }

        aps(APS_RESET);

        vTaskDelay(10000 / portTICK_RATE_MS);
    }

    vTaskDelete(NULL);
}

/*
 * Runs the actual demo effect.
 */
void demo_task(void *params)
{
    color_t green = hagl_color(0, 255, 0);
    wchar_t message[128];

    /* Avoid waiting when running for the first time. */
    xEventGroupSetBits(event, RENDER_FINISHED);

    while (1) {
        switch(effect) {
        case 0:
            metaballs_animate();
            wait_for_vsync();
            metaballs_render();
            break;
        case 1:
            plasma_animate();
            wait_for_vsync();
            plasma_render();
            break;
        case 2:
            rotozoom_animate();
            wait_for_vsync();
            rotozoom_render();
            break;
        case 3:
            deform_animate();
            wait_for_vsync();
            deform_render();
            break;
        }
        /* Notify flush task that rendering has finished. */
        xEventGroupSetBits(event, RENDER_FINISHED);

        /* Print the message on top left corner. */
        swprintf(message, sizeof(message), u"%s    ", demo[effect]);
        hagl_set_clip_window(0, 0, DISPLAY_WIDTH - 1, DISPLAY_HEIGHT - 1);
        hagl_put_text(message, 4, 4, green, font6x9);

        /* Print the message on lower left corner. */
        swprintf(message, sizeof(message), u"%.*f FPS  ", 0, fb_fps);
        hagl_put_text(message, 4, DISPLAY_HEIGHT - 14, green, font6x9);

        /* Print the message on lower right corner. */
        swprintf(message, sizeof(message), u"%.*f KBPS  ", 0, fb_bps / 1000);
        hagl_put_text(message, DISPLAY_WIDTH - 60, DISPLAY_HEIGHT - 14, green, font6x9);

        hagl_set_clip_window(0, 20, DISPLAY_WIDTH - 1, DISPLAY_HEIGHT - 21);
    }

    vTaskDelete(NULL);
}

void app_main()
{
    vTaskDelay(2000 / portTICK_RATE_MS);

    ESP_LOGI(TAG, "SDK version: %s", esp_get_idf_version());
    ESP_LOGI(TAG, "Heap when starting: %d", esp_get_free_heap_size());

#ifdef CONFIG_DEVICE_HAS_AXP202
    /* TTGO T-Watch-2020 uses AXP202 PMU. */
    static i2c_port_t i2c_port = I2C_NUM_0;
    axp202_t axp;

    ESP_LOGI(TAG, "Initializing I2C");
    i2c_init(i2c_port);

    ESP_LOGI(TAG, "Initializing AXP202");
    axp.read = &i2c_read;
    axp.write = &i2c_write;
    axp.handle = &i2c_port;
    axp202_init(&axp);
#endif /* CONFIG_DEVICE_HAS_AXP202 */

#ifdef CONFIG_DEVICE_HAS_AXP192
    /* M5StickC uses AXP192 PMU. */
    static i2c_port_t i2c_port = I2C_NUM_0;
    axp192_t axp;

    ESP_LOGI(TAG, "Initializing I2C");
    i2c_init(i2c_port);

    ESP_LOGI(TAG, "Initializing AXP192");
    axp.read = &i2c_read;
    axp.write = &i2c_write;
    axp.handle = &i2c_port;
    axp192_init(&axp);
#endif /* CONFIG_DEVICE_HAS_AXP192 */

    event = xEventGroupCreate();

    hagl_init();
    /* Reserve 20 pixels in top and bottom for debug texts. */
    hagl_set_clip_window(0, 20, DISPLAY_WIDTH - 1, DISPLAY_HEIGHT - 21);

    ESP_LOGI(TAG, "Heap after HAGL init: %d", esp_get_free_heap_size());

#ifdef HAGL_HAL_USE_BUFFERING
    xTaskCreatePinnedToCore(flush_task, "Flush", 4096, NULL, 1, NULL, 0);
#endif
    xTaskCreatePinnedToCore(demo_task, "Demo", 8092, NULL, 1, NULL, 1);
    xTaskCreatePinnedToCore(switch_task, "Switch", 2048, NULL, 2, NULL, 1);
}
