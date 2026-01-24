#pragma once

#include <Preferences.h>
#include <cstdint>

struct Settings {
    static constexpr uint16_t DEFAULT_SLEEP_TIMEOUT = 300;  // 5 minutes

    bool soundEnabled = true;
    uint16_t sleepTimeoutSecs = DEFAULT_SLEEP_TIMEOUT;  // 0 = disabled
    bool wifiAutoConnect = false;

    void initDefaults();
    bool load(Preferences& prefs);
    bool save(Preferences& prefs);
};
