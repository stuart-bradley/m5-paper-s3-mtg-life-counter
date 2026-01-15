#include "Toolbar.hpp"
#include <M5Unified.h>
#include <WiFi.h>

Toolbar::Toolbar() : Component(Rect(0, 0, 960, HEIGHT)) {}

void Toolbar::update() {
    int8_t newBattery = M5.Power.getBatteryLevel();

    auto dt = M5.Rtc.getDateTime();
    uint8_t newHour = dt.time.hours;
    uint8_t newMinute = dt.time.minutes;

    // Check WiFi status
    bool newWifiConnected = (WiFi.status() == WL_CONNECTED);
    int8_t newWifiStrength = 0;
    if (newWifiConnected) {
        int32_t rssi = WiFi.RSSI();
        if (rssi >= -50)
            newWifiStrength = 4;  // Excellent
        else if (rssi >= -60)
            newWifiStrength = 3;  // Good
        else if (rssi >= -70)
            newWifiStrength = 2;  // Fair
        else if (rssi >= -80)
            newWifiStrength = 1;  // Weak
        else
            newWifiStrength = 1;  // Very weak but connected
    }

    if (newBattery != _batteryLevel || newHour != _hour || newMinute != _minute ||
        newWifiConnected != _wifiConnected || newWifiStrength != _wifiStrength) {
        _batteryLevel = newBattery;
        _hour = newHour;
        _minute = newMinute;
        _wifiConnected = newWifiConnected;
        _wifiStrength = newWifiStrength;
        setDirty();
    }
}

void Toolbar::draw(M5GFX* gfx) {
    if (!isDirty())
        return;

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

    // Right of center: WiFi indicator
    // Format: ((●)) for connected, ((x)) for disconnected
    // Signal bars version: .oOO for strength levels
    const char* wifiStr;
    if (_wifiConnected) {
        switch (_wifiStrength) {
            case 4:
                wifiStr = "(((+)))";
                break;  // Excellent
            case 3:
                wifiStr = "((+))";
                break;  // Good
            case 2:
                wifiStr = "(+)";
                break;  // Fair
            default:
                wifiStr = "+";
                break;  // Weak
        }
    } else {
        wifiStr = "(x)";  // Disconnected
    }
    gfx->setTextDatum(MC_DATUM);
    gfx->drawString(wifiStr, x + w / 2 + 120, y + h / 2);

    // Right: Battery bar + percentage (right-aligned)
    // Format: [####--] 67%
    gfx->setTextDatum(MR_DATUM);

    char battStr[24];
    if (_batteryLevel >= 0) {
        // Build 6-character bar: # for filled, - for empty
        int filled = (_batteryLevel * 6 + 50) / 100;  // Round to nearest
        if (filled > 6)
            filled = 6;
        if (filled < 0)
            filled = 0;

        char bar[7];
        for (int i = 0; i < 6; i++) {
            bar[i] = (i < filled) ? '#' : '-';
        }
        bar[6] = '\0';
        snprintf(battStr, sizeof(battStr), "[%s] %d%%", bar, _batteryLevel);
    } else {
        snprintf(battStr, sizeof(battStr), "[------] --%%");
    }
    gfx->drawString(battStr, x + w - 8, y + h / 2);

    setDirty(false);
}
