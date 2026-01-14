#include "Settings.hpp"

static const char* NVS_NAMESPACE = "settings";
static const char* KEY_SOUND_ON = "soundOn";
static const char* KEY_SLEEP_SECS = "sleepSecs";

void Settings::initDefaults() {
    soundEnabled = true;
    sleepTimeoutSecs = DEFAULT_SLEEP_TIMEOUT;
}

bool Settings::load(Preferences& prefs) {
    if (!prefs.begin(NVS_NAMESPACE, true)) {
        initDefaults();
        return false;
    }

    soundEnabled = prefs.getBool(KEY_SOUND_ON, true);
    sleepTimeoutSecs = prefs.getUShort(KEY_SLEEP_SECS, DEFAULT_SLEEP_TIMEOUT);

    prefs.end();
    return true;
}

bool Settings::save(Preferences& prefs) {
    if (!prefs.begin(NVS_NAMESPACE, false)) {
        return false;
    }

    prefs.putBool(KEY_SOUND_ON, soundEnabled);
    prefs.putUShort(KEY_SLEEP_SECS, sleepTimeoutSecs);

    prefs.end();
    return true;
}
