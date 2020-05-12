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

#include <stdint.h>
#include <math.h>
#include <hagl.h>

#include "plasma.h"

color_t palette[256];
uint8_t plasma[DISPLAY_WIDTH][DISPLAY_HEIGHT];

static const uint8_t PLASMA_SPEED = 4;

void plasma_init()
{
    for(int i = 0; i < 256; i++) {
        uint8_t r, g, b;

        r = 128.0 + 128.0 * sin((M_PI * i / 128.0) + 1);
        g = 128.0 + 128.0 * sin((M_PI * i / 64.0) + 1);
        b = 64;
        palette[i] = hagl_color(r, g, b);
    }

    for (uint16_t x = 0; x < DISPLAY_WIDTH; x++) {
        for (uint16_t y = 0; y < DISPLAY_HEIGHT; y++) {
                float v1 = 128.0 + (128.0 * sin(x / 32.0));
                float v2 = 128.0 + (128.0 * sin(y / 24.0));
                float v3 = 128.0 + (128.0 * sin(sqrt(x * x + y * y) / 24.0));
                uint8_t color = (v1 + v2 + v3) / 3;
                plasma[x][y] = color;
        }
    }
}

void plasma_render()
{
    for (uint16_t x = 0; x < DISPLAY_WIDTH; x++) {
        for (uint16_t y = 0; y < DISPLAY_HEIGHT; y++) {
            hagl_put_pixel(x, y, palette[plasma[x][y]]);
        }
    }
}

void plasma_animate()
{
    for (uint16_t x = 0; x < DISPLAY_WIDTH; x++) {
        for (uint16_t y = 0; y < DISPLAY_HEIGHT; y++) {
                uint8_t color = plasma[x][y];
                color += PLASMA_SPEED;
                color %= 256;
                plasma[x][y] = color;
        }
    }
}
