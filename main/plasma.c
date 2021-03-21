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
#include <stdlib.h>
#include <math.h>
#include <hagl.h>

#include "plasma.h"

color_t *palette;
uint8_t *plasma;

static const uint8_t SPEED = 4;
static const uint8_t PIXEL_SIZE = 1;

void plasma_init()
{
    uint8_t *ptr = plasma = malloc(DISPLAY_WIDTH * DISPLAY_HEIGHT * sizeof(uint8_t));
    palette = malloc(256 * sizeof(color_t));

    /* Generate nice continous palette. */
    for(int i = 0; i < 256; i++) {
        uint8_t r, g, b;
        r = 128.0 + 128.0 * sin((M_PI * i / 128.0) + 1);
        g = 128.0 + 128.0 * sin((M_PI * i / 64.0) + 1);
        b = 64;
        palette[i] = hagl_color(r, g, b);
    }

    for (uint16_t y = 0; y < DISPLAY_HEIGHT; y += PIXEL_SIZE) {
        for (uint16_t x = 0; x < DISPLAY_WIDTH; x += PIXEL_SIZE) {
                /* Generate three different sinusoids. */
                float v1 = 128.0 + (128.0 * sin(x / 32.0));
                float v2 = 128.0 + (128.0 * sin(y / 24.0));
                float v3 = 128.0 + (128.0 * sin(sqrt(x * x + y * y) / 24.0));
                /* Calculate average of the three sinusoids */
                /* and use it as color index. */
                uint8_t color = (v1 + v2 + v3) / 3;
                *(ptr++) = color;
        }
    }
}

void plasma_render()
{
    uint8_t *ptr = plasma;

    for (uint16_t y = 0; y < DISPLAY_HEIGHT; y = y + PIXEL_SIZE) {
        for (uint16_t x = 0; x < DISPLAY_WIDTH; x = x + PIXEL_SIZE) {
            /* Get a color for pixel from the plasma buffer. */
            uint8_t index = *(ptr++);
            color_t color = palette[index];
            /* Put a pixel to the display. */
            if (1 == PIXEL_SIZE) {
                hagl_put_pixel(x, y, color);
            } else {
                hagl_fill_rectangle(x, y, x + PIXEL_SIZE - 1, y + PIXEL_SIZE - 1, color);
            }
        }
    }
}

void plasma_animate()
{
    uint8_t *ptr = plasma;

    for (uint16_t y = 0; y < DISPLAY_HEIGHT; y = y + PIXEL_SIZE) {
        for (uint16_t x = 0; x < DISPLAY_WIDTH; x = x + PIXEL_SIZE) {
                /* Get a pixel from the plasma buffer. */
                uint8_t index = *ptr;
                /* Choose next color from the palette. */
                index = (index + SPEED) % 256;
                /* Put a pixel to the plasma buffer. */
                *(ptr++) = index;
        }
    }
}

void plasma_close()
{
    free(plasma);
    free(palette);
}