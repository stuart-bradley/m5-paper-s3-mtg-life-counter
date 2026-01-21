#include "MTGApp.hpp"
#include <Preferences.h>
#include <cstring>

// Define static constexpr member (required for ODR-use)
constexpr AppMetadata MTGApp::_metadata;

MTGApp::MTGApp() : _lifeScreen(this), _settingsScreen(this) {}

void MTGApp::onLaunch() {
    Preferences prefs;
    _gameState.load(prefs);
}

void MTGApp::onSuspend() {
    Preferences prefs;
    _gameState.save(prefs);
}

Screen* MTGApp::getScreen(const char* id) {
    if (strcmp(id, "settings") == 0) {
        return &_settingsScreen;
    }
    return nullptr;
}
