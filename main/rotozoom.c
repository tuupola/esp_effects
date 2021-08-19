/*

MIT No Attribution

Copyright (c) 2020-2021 Mika Tuupola

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

#include <stdlib.h>
#include <math.h>
#include <hagl.h>

#include "head.h"

static const uint8_t SPEED = 2;
static const uint8_t PIXEL_SIZE = 2;

static uint16_t angle;
// static float sinlut[360];
// static float coslut[360];

void rotozoom_init()
{
    /* Generate look up tables. */
    // for (uint16_t i = 0; i < 360; i++) {
    //     sinlut[i] = sin(i * M_PI / 180);
    //     coslut[i] = cos(i * M_PI / 180);
    // }
}

void rotozoom_render()
{
    float s, c, z;

    s = sin(angle * M_PI / 180);
    c = cos(angle * M_PI / 180);
    // s = sinlut[angle];
    // c = coslut[angle];
    z = s * 1.2;

    for (uint16_t x = 0; x < DISPLAY_WIDTH; x = x + PIXEL_SIZE) {
        for (uint16_t y = 0; y < DISPLAY_HEIGHT; y = y + PIXEL_SIZE) {

            /* Get a rotated pixel from the head image. */
            int16_t u = (int16_t)((x * c - y * s) * z) % HEAD_WIDTH;
            int16_t v = (int16_t)((x * s + y * c) * z) % HEAD_HEIGHT;

            u = abs(u);
            if (v < 0) {
                v += HEAD_HEIGHT;
            }
            color_t *color = (color_t*) (head + HEAD_WIDTH * sizeof(color_t) * v + sizeof(color_t) * u);

            if (1 == PIXEL_SIZE) {
                hagl_put_pixel(x, y, *color);
            } else {
                hagl_fill_rectangle(x, y, x + PIXEL_SIZE - 1, y + PIXEL_SIZE - 1, *color);
            }
            hagl_put_pixel(x, y, *color);
        }
    }
}

void rotozoom_animate()
{
    angle = (angle + SPEED) % 360;
}
