#include "MTGLifeScreen.hpp"
#include <Arduino.h>
#include <Preferences.h>
#include "../../app/Navigation.hpp"
#include "../../ui/Layout.hpp"
#include "../../utils/Sound.hpp"
#include "MTGApp.hpp"

MTGLifeScreen::MTGLifeScreen(MTGApp* app) : HeaderScreen("LIFE COUNTER"), _app(app) {
    // Navigation buttons will be set up in onEnter when we know app is fully constructed
}

GameState& MTGLifeScreen::gameState() {
    return _app->gameState();
}

void MTGLifeScreen::onEnter() {
    // Load state from NVS
    Preferences prefs;
    gameState().load(prefs);

    // Set up navigation buttons
    setLeftButton("< HOME", []() { Navigation::instance().goHome(); });
    setRightButton("SETTINGS",
                   [this]() { Navigation::instance().pushScreen(_app->settingsScreen()); });

    createPlayerCards();
    setNeedsFullRedraw(true);
}

void MTGLifeScreen::onExit() {
    // Save state to NVS
    Preferences prefs;
    gameState().save(prefs);

    destroyPlayerCards();
    if (_keyboard) {
        delete _keyboard;
        _keyboard = nullptr;
    }
}

void MTGLifeScreen::createPlayerCards() {
    destroyPlayerCards();

    for (int i = 0; i < gameState().playerCount; i++) {
        int idx = i;  // Capture for lambda
        _playerCards[i] =
            new PlayerCard(&gameState().players[i], [this, idx]() { showKeyboard(idx); });
    }
    layoutPlayerCards();
}

void MTGLifeScreen::destroyPlayerCards() {
    for (int i = 0; i < 6; i++) {
        if (_playerCards[i]) {
            delete _playerCards[i];
            _playerCards[i] = nullptr;
        }
    }
}

void MTGLifeScreen::layoutPlayerCards() {
    for (int i = 0; i < gameState().playerCount; i++) {
        if (_playerCards[i]) {
            Rect r = getPlayerCardRect(i, gameState().playerCount);
            _playerCards[i]->setBounds(r);
        }
    }
}

Rect MTGLifeScreen::getPlayerCardRect(int index, int playerCount) const {
    int16_t startY = Layout::headerContentY() + Layout::MARGIN_S;
    int16_t availableH = Layout::headerContentH() - Layout::MARGIN_S * 2;
    int16_t availableW = Layout::screenW() - Layout::MARGIN_M;

    switch (playerCount) {
        case 2: {
            // 2 side by side
            int16_t w = availableW / 2 - 2;
            int16_t h = availableH;
            int16_t x = 4 + index * (w + 4);
            return Rect(x, startY, w, h);
        }
        case 3: {
            // 3 in a row
            int16_t w = availableW / 3 - 3;
            int16_t h = availableH;
            int16_t x = 4 + index * (w + 4);
            return Rect(x, startY, w, h);
        }
        case 4: {
            // 2x2 grid
            int16_t w = availableW / 2 - 2;
            int16_t h = availableH / 2 - 2;
            int row = index / 2;
            int col = index % 2;
            int16_t x = 4 + col * (w + 4);
            int16_t y = startY + row * (h + 4);
            return Rect(x, y, w, h);
        }
        case 5: {
            // 3 top, 2 bottom
            int16_t h = availableH / 2 - 2;
            if (index < 3) {
                int16_t w = availableW / 3 - 3;
                int16_t x = 4 + index * (w + 4);
                return Rect(x, startY, w, h);
            } else {
                int16_t w = availableW / 2 - 2;
                int col = index - 3;
                int16_t x = 4 + col * (w + 4);
                return Rect(x, startY + h + 4, w, h);
            }
        }
        case 6:
        default: {
            // 3x2 grid
            int16_t w = availableW / 3 - 3;
            int16_t h = availableH / 2 - 2;
            int row = index / 3;
            int col = index % 3;
            int16_t x = 4 + col * (w + 4);
            int16_t y = startY + row * (h + 4);
            return Rect(x, y, w, h);
        }
    }
}

void MTGLifeScreen::onUpdate() {
    // Auto-save periodically
    uint32_t now = millis();
    if (now - _lastSaveTime > SAVE_INTERVAL_MS) {
        Preferences prefs;
        gameState().save(prefs);
        _lastSaveTime = now;
    }
}

void MTGLifeScreen::onHeaderFullRedraw(M5GFX* gfx) {
    // Mark all components dirty after full redraw
    for (int i = 0; i < gameState().playerCount; i++) {
        if (_playerCards[i]) {
            _playerCards[i]->setDirty(true);
            _playerCards[i]->draw(gfx);
        }
    }
    if (_keyboard) {
        _keyboard->setDirty(true);
        _keyboard->draw(gfx);
    }
}

bool MTGLifeScreen::onDraw(M5GFX* gfx) {
    bool needsDisplay = false;

    // Draw player cards (only if dirty)
    for (int i = 0; i < gameState().playerCount; i++) {
        if (_playerCards[i] && _playerCards[i]->isDirty()) {
            _playerCards[i]->draw(gfx);
            needsDisplay = true;
        }
    }

    // Draw keyboard overlay if active (only if dirty)
    if (_keyboard && _keyboard->isDirty()) {
        _keyboard->draw(gfx);
        needsDisplay = true;
    }

    return needsDisplay;
}

bool MTGLifeScreen::onTouch(int16_t x, int16_t y, bool pressed, bool released) {
    // Keyboard has priority when visible
    if (_keyboard) {
        return _keyboard->handleTouch(x, y, pressed, released);
    }

    // Check player cards
    for (int i = 0; i < gameState().playerCount; i++) {
        if (_playerCards[i] && _playerCards[i]->handleTouch(x, y, pressed, released)) {
            return true;
        }
    }

    return false;
}

void MTGLifeScreen::showKeyboard(int playerIndex) {
    if (_keyboard) {
        delete _keyboard;
    }
    _editingPlayerIndex = playerIndex;
    _keyboard = new Keyboard(gameState().players[playerIndex].name,
                             [this](const char* result, bool confirmed) {
                                 // Save index before hideKeyboard clears it
                                 int idx = _editingPlayerIndex;
                                 hideKeyboard(confirmed);
                                 if (confirmed && idx >= 0) {
                                     gameState().players[idx].setName(result);
                                     // Mark player card dirty to show updated name
                                     if (_playerCards[idx]) {
                                         _playerCards[idx]->setDirty(true);
                                     }
                                     Preferences prefs;
                                     gameState().save(prefs);
                                 }
                             });
    setNeedsFullRedraw(true);
}

void MTGLifeScreen::hideKeyboard(bool confirmed) {
    (void)confirmed;
    if (_keyboard) {
        delete _keyboard;
        _keyboard = nullptr;
    }
    _editingPlayerIndex = -1;
    setNeedsFullRedraw(true);
}
