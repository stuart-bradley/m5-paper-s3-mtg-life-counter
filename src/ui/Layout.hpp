#pragma once

#include "../platform/Device.hpp"

namespace Layout {
// Screen dimensions
constexpr int16_t screenW() {
    return Device::SCREEN_WIDTH;
}
constexpr int16_t screenH() {
    return Device::SCREEN_HEIGHT;
}

// Standard heights
constexpr int16_t TOOLBAR_H = Device::TOOLBAR_HEIGHT;
constexpr int16_t HEADER_H = Device::HEADER_HEIGHT;

// Content area (below toolbar only)
constexpr int16_t contentY() {
    return TOOLBAR_H;
}
constexpr int16_t contentH() {
    return screenH() - TOOLBAR_H;
}

// Content area (below toolbar + header)
constexpr int16_t headerContentY() {
    return TOOLBAR_H + HEADER_H;
}
constexpr int16_t headerContentH() {
    return screenH() - TOOLBAR_H - HEADER_H;
}

// Center of screen
constexpr int16_t centerX() {
    return screenW() / 2;
}
constexpr int16_t centerY() {
    return screenH() / 2;
}

// Margins
constexpr int16_t MARGIN_S = 4;
constexpr int16_t MARGIN_M = 8;
constexpr int16_t MARGIN_L = 20;

// Touch
constexpr int16_t MIN_TOUCH = Device::MIN_TOUCH_TARGET;

// Standard button sizes
constexpr int16_t BUTTON_W = 90;
constexpr int16_t BUTTON_H = 32;
constexpr int16_t BUTTON_MARGIN = 10;
}  // namespace Layout
