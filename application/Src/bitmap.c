// The MIT License (MIT)
//
// Copyright (c) 2015 Aleksandr Aleshin <silencer@quadrius.net>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <stdint.h>

#include "bitmap.h"

void HSV2RGB(HSV_t *hsv, RGB_t *rgb)
{
    if (!hsv->v)
    {
        rgb->r = rgb->g = rgb->b = 0;
    }
    else if (!hsv->s)
    {
        rgb->r = rgb->g = rgb->b = hsv->v;
    }
    else
    {
        int hue = HUE(hsv->h);

        int sector = hue / 60;
        int angle = sector & 1 ? 60 - hue % 60 : hue % 60;

        int high = hsv->v;
        int low = (255 - hsv->s) * high / 255;
        int middle = low + (high - low) * angle / 60;

        switch (sector)
        {
        case 0: // red -> yellow
            rgb->r = high;
            rgb->g = middle;
            rgb->b = low;

            break;

        case 1: // yellow -> green
            rgb->r = middle;
            rgb->g = high;
            rgb->b = low;

            break;

        case 2: // green -> cyan
            rgb->r = low;
            rgb->g = high;
            rgb->b = middle;

            break;

        case 3: // cyan -> blue
            rgb->r = low;
            rgb->g = middle;
            rgb->b = high;

            break;

        case 4: // blue -> magenta
            rgb->r = middle;
            rgb->g = low;
            rgb->b = high;

            break;

        case 5: // magenta -> red
            rgb->r = high;
            rgb->g = low;
            rgb->b = middle;
        }
    }
}
