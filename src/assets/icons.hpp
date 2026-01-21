#pragma once

#include <M5GFX.h>
#include "icons_generated.hpp"

// Use the generated ICON_SIZE constant
static constexpr int16_t ICON_SIZE = GENERATED_ICON_SIZE;

// Fast bitmap drawing using M5GFX drawBitmap
inline void drawIcon(M5GFX* gfx, const uint8_t* icon, int16_t x, int16_t y, uint32_t color) {
    gfx->drawBitmap(x, y, icon, ICON_SIZE, ICON_SIZE, color);
}
