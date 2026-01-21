#pragma once

#include <cstdint>

struct Rect {
    int16_t x = 0;
    int16_t y = 0;
    int16_t w = 0;
    int16_t h = 0;

    Rect() = default;
    Rect(int16_t x_, int16_t y_, int16_t w_, int16_t h_) : x(x_), y(y_), w(w_), h(h_) {}

    bool contains(int16_t px, int16_t py) const {
        return px >= x && px < x + w && py >= y && py < y + h;
    }
};
