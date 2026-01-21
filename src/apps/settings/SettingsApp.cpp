#include "SettingsApp.hpp"
#include <Preferences.h>
#include <cstring>
#include "../../utils/Sound.hpp"

// Define static constexpr member (required for ODR-use)
constexpr AppMetadata SettingsApp::_metadata;

SettingsApp::SettingsApp() : _systemScreen(this), _wifiScreen(this) {}

void SettingsApp::onLaunch() {
    Preferences prefs;
    _settings.load(prefs);
    Sound::setEnabled(_settings.soundEnabled);
}

void SettingsApp::onSuspend() {
    Preferences prefs;
    _settings.save(prefs);
}

Screen* SettingsApp::getScreen(const char* id) {
    if (strcmp(id, "wifi") == 0) {
        return &_wifiScreen;
    }
    return nullptr;
}
