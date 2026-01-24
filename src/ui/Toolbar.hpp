#pragma once

#include "../platform/Device.hpp"
#include "Component.hpp"

class Toolbar : public Component {
   public:
    static constexpr int16_t HEIGHT = Device::TOOLBAR_HEIGHT;

    Toolbar();

    void draw(M5GFX* gfx) override;

    void update();  // Call to refresh battery/time readings

   private:
    static constexpr int BATTERY_SAMPLE_COUNT = 8;
    static constexpr int BATTERY_HYSTERESIS = 2;  // Only update display if change >= 2%

    int8_t _batteryLevel = -1;  // Displayed battery level
    int16_t _batterySamples[BATTERY_SAMPLE_COUNT] = {0};
    uint8_t _batterySampleIndex = 0;
    bool _batterySamplesInitialized = false;

    uint8_t _hour = 0;
    uint8_t _minute = 0;
    bool _wifiConnected = false;
    int8_t _wifiStrength = 0;  // 0=none, 1=weak, 2=fair, 3=good, 4=excellent
};
