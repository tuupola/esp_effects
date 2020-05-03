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
#include <rgb565.h>

#include "metaballs.h"

struct ball {
    struct vector2 position;
    struct vector2 velocity;
    uint16_t radius;
    uint16_t color;
};

struct ball balls[255];
struct settings settings;

void metaballs_init(struct settings incoming)
{
    settings = incoming;
    for (int16_t i = 0; i < settings.num; i++) {
        balls[i].radius = (rand() % settings.radius.max) + settings.radius.min;
        balls[i].color = 0xffff;
        balls[i].position.x = rand() % settings.max.x;
        balls[i].position.y = rand() % settings.max.y;
        balls[i].velocity.x = (rand() % settings.velocity.max) + settings.velocity.min;
        balls[i].velocity.y = (rand() % settings.velocity.max) + settings.velocity.min;
    }
}

void metaballs_animate()
{
    for (int16_t i = 0; i < settings.num; i++) {
        balls[i].position.x += balls[i].velocity.x;
        balls[i].position.y += balls[i].velocity.y;

        if ((balls[i].position.x < 0) | (balls[i].position.x > settings.max.x)) {
            balls[i].velocity.x = balls[i].velocity.x * -1;
        }
        if ((balls[i].position.y < 0) | (balls[i].position.y > settings.max.y)) {
            balls[i].velocity.y = balls[i].velocity.y * -1;
        }
    }
}

float sqrt3(const float x)
{
    union {
        int i;
        float x;
    } u;

    u.x = x;
    u.i = (1<<29) + (u.i >> 1) - (1<<22);
    return u.x;
}

void metaballs_render()
{
    for(uint16_t x = 0; x < settings.max.x; x += 2) {
        for(uint16_t y = 0; y < settings.max.y; y += 2) {
            float sum = 0;
            for (uint8_t i = 0; i < settings.num; i++) {
                float dx = x - balls[i].position.x;
                float dy = y - balls[i].position.y;
                float d2 = dx * dx + dy * dy;
                sum += balls[i].radius * balls[i].radius / d2;
                // sum += balls[i].radius / sqrt3(d2);

            }

            //printf("%f\n", sum);
            if (sum > 0.65) {
                hagl_put_pixel(x, y, settings.color[0]);
            } else if (sum > 0.5) {
                hagl_put_pixel(x, y, settings.color[1]);
            } else if (sum > 0.4) {
                hagl_put_pixel(x, y, settings.color[2]);
            }
        }
    }
}
