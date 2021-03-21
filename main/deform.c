/*

MIT No Attribution

Copyright (c) 2021 Mika Tuupola

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

Adapted from article by Inigo Quilez:
https://iquilezles.org/www/articles/deform/deform.htm

SPDX-License-Identifier: MIT-0

*/

#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <hagl.h>

#include "head.h"
#include "deform.h"

static const uint8_t SPEED = 2;
static const uint8_t PIXEL_SIZE = 1;
static uint32_t frame;

int8_t *lut;

void deform_init()
{
    /* Allocate memory for lut and store address also to ptr. */
    int8_t *ptr = lut = malloc(DISPLAY_HEIGHT * DISPLAY_WIDTH * 2 * sizeof(int8_t));

    for (uint16_t j = 0; j < DISPLAY_HEIGHT; j += PIXEL_SIZE) {
        for (uint16_t i = 0; i < DISPLAY_WIDTH; i += PIXEL_SIZE) {

            const float x = -1.00f + 2.00f * i / DISPLAY_WIDTH;
            const float y = -1.00f + 2.00f * j / DISPLAY_HEIGHT;
            const float r = sqrtf(x * x + y * y);
            const float a = atan2f(y, x);

            const float u = cosf(a) / r;
            const float v = sinf(a) / r;

            // const float u = 0.5 * a / M_PI;
            // const float v = sin(7 * r);

            // const float u = 0.02 * y + 0.03 * cos(a * 3) / r;
            // const float v = 0.02 * x + 0.03 * sin(a * 3) / r;

            // const float u = 1 / (r + 0.5 + 0.5 * sin(5 * a));
            // const float v = a * 3 / M_PI;


            // const float u = x * cos(2 * r) - y * sin(2 * r);
            // const float v = y * cos(2 * r) + x * sin(2 * r);

            // const float u = 0.3 / (r + 0.5 * x);
            // const float v = 3 * a / M_PI;

            // const float u = 0.1 * x / (0.11 + r * 0.5);
            // const float v = 0.1 * y / (0.11 + r * 0.5);

            // const float u = r * cos(a + r);
            // const float v = r * sin(a + r);

            // const float u = x / fabs(y);
            // const float v = 1 / fabs(y);

            // const float u = x;
            // const float v = y;

            /* Target x and y coordinates in the texture. */
            const int8_t tx = ((int8_t)(HEAD_WIDTH * u)) % HEAD_WIDTH;
            const int8_t ty = ((int8_t)(HEAD_HEIGHT * v)) % HEAD_HEIGHT;

            *(ptr++) = tx;
            *(ptr++) = ty;
        }
    }
}

void deform_render()
{
    int8_t *ptr = lut;

    for (uint16_t y = 0; y < DISPLAY_HEIGHT; y += PIXEL_SIZE) {
        for (uint16_t x = 0; x < DISPLAY_WIDTH; x += PIXEL_SIZE) {

            /* Retrieve texture x and y coordinates for display coordinates. */
            int16_t u = *(ptr++) + frame;
            int16_t v = *(ptr++) + frame;

            u = abs(u) % HEAD_WIDTH;
            v = abs(v) % HEAD_HEIGHT;

            /* Get the pixel from texture and put it to the screen. */
            const color_t *color = (color_t*) (head + HEAD_WIDTH * sizeof(color_t) * v + sizeof(color_t) * u);

            if (1 == PIXEL_SIZE) {
                hagl_put_pixel(x, y, *color);
            } else {
                hagl_fill_rectangle(x, y, x + PIXEL_SIZE - 1, y + PIXEL_SIZE - 1, *color);
            }
        }
    }
}

void deform_animate()
{
    frame = frame + SPEED;
}

void deform_close()
{
    free(lut);
}
