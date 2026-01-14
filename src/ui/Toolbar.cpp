#include "Toolbar.hpp"
#include <M5Unified.h>

Toolbar::Toolbar() : Component(Rect(0, 0, 960, HEIGHT)) {
}

void Toolbar::update() {
    int8_t newBattery = M5.Power.getBatteryLevel();

    auto dt = M5.Rtc.getDateTime();
    uint8_t newHour = dt.time.hours;
    uint8_t newMinute = dt.time.minutes;

    if (newBattery != _batteryLevel || newHour != _hour || newMinute != _minute) {
        _batteryLevel = newBattery;
        _hour = newHour;
        _minute = newMinute;
        setDirty();
    }
}

void Toolbar::draw(M5GFX* gfx) {
    if (!isDirty()) return;

    int16_t x = _bounds.x;
    int16_t y = _bounds.y;
    int16_t w = _bounds.w;
    int16_t h = _bounds.h;

    // Background
    gfx->fillRect(x, y, w, h, TFT_WHITE);
    gfx->drawLine(x, y + h - 1, x + w, y + h - 1, TFT_BLACK);

    gfx->setTextColor(TFT_BLACK);
    gfx->setTextSize(1);

    // Left: Device identifier
    gfx->setTextDatum(ML_DATUM);
    gfx->drawString("[M5] PAPER-S3", x + 8, y + h / 2);

    // Center: Time
    char timeStr[8];
    snprintf(timeStr, sizeof(timeStr), "%02d:%02d", _hour, _minute);
    gfx->setTextDatum(MC_DATUM);
    gfx->drawString(timeStr, x + w / 2, y + h / 2);

    // Right: Battery
    char battStr[16];
    if (_batteryLevel >= 0) {
        snprintf(battStr, sizeof(battStr), "%d%%", _batteryLevel);
    } else {
        snprintf(battStr, sizeof(battStr), "--%%");
    }
    gfx->setTextDatum(MR_DATUM);
    gfx->drawString(battStr, x + w - 8, y + h / 2);

    setDirty(false);
}
