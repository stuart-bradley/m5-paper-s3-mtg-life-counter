#include "Power.hpp"
#include <M5Unified.h>
#include "Log.hpp"

namespace Power {

static uint32_t lastActivityMs = 0;
static bool imuInitialized = false;

void init() {
    lastActivityMs = millis();

    // Check if IMU is available
    if (M5.Imu.isEnabled()) {
        imuInitialized = true;
        LOG_I("Power: IMU enabled for motion detection");
    } else {
        imuInitialized = false;
        LOG_I("Power: IMU not available, button wake only");
    }
}

void resetInactivityTimer() {
    lastActivityMs = millis();
}

bool shouldSleep(uint16_t timeoutSecs) {
    if (timeoutSecs == 0) {
        return false;  // Auto-sleep disabled
    }

    uint32_t elapsedMs = millis() - lastActivityMs;
    uint32_t timeoutMs = static_cast<uint32_t>(timeoutSecs) * 1000;

    return elapsedMs >= timeoutMs;
}

void powerOff() {
    LOG_I("Power: Entering power-off mode");

    // Flush serial to ensure debug output completes before shutdown
    Serial.flush();
    delay(50);

    // Power off the device
    // Note: IMU wake may not work on M5Paper S3 (see issue v21)
    // Device will wake on button press
    M5.Power.powerOff();
}

}  // namespace Power
