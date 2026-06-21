#pragma once

#include <cstdint>

// M5Paper S3, landscape: 960x540.
namespace Layout {
// Screen dimensions
constexpr int16_t screenW() {
    return 960;
}
constexpr int16_t screenH() {
    return 540;
}

// Standard heights
constexpr int16_t TOOLBAR_H = 32;
constexpr int16_t HEADER_H = 44;

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
constexpr int16_t MIN_TOUCH = 44;

// Standard button sizes
constexpr int16_t BUTTON_W = 90;
constexpr int16_t BUTTON_H = 32;
constexpr int16_t BUTTON_MARGIN = 10;
}  // namespace Layout
