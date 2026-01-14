#pragma once

#include <cstdint>
#include <Preferences.h>

struct Settings {
    static constexpr uint16_t DEFAULT_SLEEP_TIMEOUT = 300;  // 5 minutes

    bool soundEnabled = true;
    uint16_t sleepTimeoutSecs = DEFAULT_SLEEP_TIMEOUT;  // 0 = disabled

    void initDefaults();
    bool load(Preferences& prefs);
    bool save(Preferences& prefs);
};
