#pragma once

#include "Component.hpp"

class Toolbar : public Component {
public:
    static constexpr int16_t HEIGHT = 32;

    Toolbar();

    void draw(M5GFX* gfx) override;

    void update();  // Call to refresh battery/time readings

private:
    int8_t _batteryLevel = -1;
    uint8_t _hour = 0;
    uint8_t _minute = 0;
};
