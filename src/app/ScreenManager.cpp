#include "ScreenManager.hpp"
#include <Preferences.h>

static constexpr const char* PREF_NAMESPACE = "app";
static constexpr const char* PREF_LAST_SCREEN = "lastScreen";

static void saveCurrentScreenId(ScreenId id) {
    Preferences prefs;
    prefs.begin(PREF_NAMESPACE, false);
    prefs.putUChar(PREF_LAST_SCREEN, static_cast<uint8_t>(id));
    prefs.end();
}

void ScreenManager::registerScreen(Screen* screen) {
    if (_screenCount < MAX_SCREENS && screen != nullptr) {
        _screens[_screenCount++] = screen;
    }
}

Screen* ScreenManager::getScreenById(ScreenId id) {
    for (int i = 0; i < _screenCount; i++) {
        if (_screens[i] && _screens[i]->getScreenId() == id) {
            return _screens[i];
        }
    }
    return nullptr;
}

ScreenId ScreenManager::getCurrentScreenId() {
    if (_current) {
        return _current->getScreenId();
    }
    return ScreenId::Home;  // Default
}

void ScreenManager::setScreenById(ScreenId id) {
    Screen* screen = getScreenById(id);
    if (screen) {
        setScreen(screen);
    }
}

void ScreenManager::setScreen(Screen* newScreen) {
    if (_current == newScreen)
        return;

    if (_current) {
        _current->onExit();
    }

    _previous = _current;
    _current = newScreen;

    if (_current) {
        _current->onEnter();
        _current->setNeedsFullRedraw(true);
        saveCurrentScreenId(_current->getScreenId());
    }
}

void ScreenManager::goBack() {
    if (!_previous)
        return;

    if (_current) {
        _current->onExit();
    }

    Screen* temp = _current;
    _current = _previous;
    _previous = temp;

    if (_current) {
        _current->onEnter();
        _current->setNeedsFullRedraw(true);
        saveCurrentScreenId(_current->getScreenId());
    }
}

void ScreenManager::update() {
    if (_current) {
        _current->update();
    }
}

void ScreenManager::draw(M5GFX* gfx) {
    if (_current) {
        _current->draw(gfx);
    }
}

bool ScreenManager::handleTouch(int16_t x, int16_t y, bool pressed, bool released) {
    if (_current) {
        return _current->handleTouch(x, y, pressed, released);
    }
    return false;
}
