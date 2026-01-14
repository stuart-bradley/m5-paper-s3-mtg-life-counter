#include "ScreenManager.hpp"

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
