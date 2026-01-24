#include "Settings.hpp"

static const char* NVS_NAMESPACE = "settings";
static const char* KEY_SOUND_ON = "soundOn";
static const char* KEY_SLEEP_SECS = "sleepSecs";
static const char* KEY_WIFI_AUTO = "wifiAuto";

void Settings::initDefaults() {
    soundEnabled = true;
    sleepTimeoutSecs = DEFAULT_SLEEP_TIMEOUT;
    wifiAutoConnect = false;
}

bool Settings::load(Preferences& prefs) {
    if (!prefs.begin(NVS_NAMESPACE, true)) {
        initDefaults();
        return false;
    }

    soundEnabled = prefs.getBool(KEY_SOUND_ON, true);
    sleepTimeoutSecs = prefs.getUShort(KEY_SLEEP_SECS, DEFAULT_SLEEP_TIMEOUT);
    wifiAutoConnect = prefs.getBool(KEY_WIFI_AUTO, false);

    prefs.end();
    return true;
}

bool Settings::save(Preferences& prefs) {
    if (!prefs.begin(NVS_NAMESPACE, false)) {
        return false;
    }

    prefs.putBool(KEY_SOUND_ON, soundEnabled);
    prefs.putUShort(KEY_SLEEP_SECS, sleepTimeoutSecs);
    prefs.putBool(KEY_WIFI_AUTO, wifiAutoConnect);

    prefs.end();
    return true;
}
