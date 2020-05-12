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
#include <hagl.h>

#include "metaballs.h"

struct vector2 {
    int16_t x;
    int16_t y;
};

struct ball {
    struct vector2 position;
    struct vector2 velocity;
    uint16_t radius;
    uint16_t color;
};

struct ball balls[16];

static const uint8_t NUM_BALLS = 5;
static const uint8_t MIN_VELOCITY = 3;
static const uint8_t MAX_VELOCITY = 5;
static const uint8_t MIN_RADIUS = 10;
static const uint8_t MAX_RADIUS = 20;

void metaballs_init()
{
    for (int16_t i = 0; i < NUM_BALLS; i++) {
        balls[i].radius = (rand() % MAX_RADIUS) + MIN_RADIUS;
        balls[i].color = 0xffff;
        balls[i].position.x = rand() % DISPLAY_WIDTH;
        balls[i].position.y = rand() % DISPLAY_HEIGHT;
        balls[i].velocity.x = (rand() % MAX_VELOCITY) + MIN_VELOCITY;
        balls[i].velocity.y = (rand() % MAX_VELOCITY) + MIN_VELOCITY;
    }
}

void metaballs_animate()
{
    for (int16_t i = 0; i < NUM_BALLS; i++) {
        balls[i].position.x += balls[i].velocity.x;
        balls[i].position.y += balls[i].velocity.y;

        if ((balls[i].position.x < 0) | (balls[i].position.x > DISPLAY_WIDTH)) {
            balls[i].velocity.x = balls[i].velocity.x * -1;
        }
        if ((balls[i].position.y < 0) | (balls[i].position.y > DISPLAY_HEIGHT)) {
            balls[i].velocity.y = balls[i].velocity.y * -1;
        }
    }
}

void metaballs_render()
{
    color_t black = hagl_color(0, 0, 0);
    color_t white = hagl_color(255, 255, 255);
    color_t green = hagl_color(0, 255, 0);

    for(uint16_t x = 0; x < DISPLAY_WIDTH; x = x + 2) {
        for(uint16_t y = 0; y < DISPLAY_HEIGHT; y = y + 2) {
            float sum = 0;
            for (uint8_t i = 0; i < NUM_BALLS; i++) {
                float dx = x - balls[i].position.x;
                float dy = y - balls[i].position.y;
                float d2 = dx * dx + dy * dy;
                sum += balls[i].radius * balls[i].radius / d2;
                // sum += balls[i].radius / sqrt(d2);
            }

            if (sum > 0.65) {
                hagl_put_pixel(x, y, black);
            } else if (sum > 0.5) {
                hagl_put_pixel(x, y, white);
            } else if (sum > 0.4) {
                hagl_put_pixel(x, y, green);
            }
        }
    }
}
