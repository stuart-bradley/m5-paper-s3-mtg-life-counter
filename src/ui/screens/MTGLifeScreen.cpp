#include "MTGLifeScreen.hpp"
#include "../../app/ScreenManager.hpp"
#include "../../utils/Sound.hpp"
#include <Arduino.h>

MTGLifeScreen::MTGLifeScreen(ScreenManager* manager)
    : _manager(manager) {
}

void MTGLifeScreen::onEnter() {
    loadState();
    createPlayerCards();
    setNeedsFullRedraw(true);
}

void MTGLifeScreen::onExit() {
    saveState();
    destroyPlayerCards();
    if (_keyboard) {
        delete _keyboard;
        _keyboard = nullptr;
    }
}

void MTGLifeScreen::loadState() {
    Preferences prefs;
    _gameState.load(prefs);
}

void MTGLifeScreen::saveState() {
    Preferences prefs;
    _gameState.save(prefs);
}

void MTGLifeScreen::createPlayerCards() {
    destroyPlayerCards();

    for (int i = 0; i < _gameState.playerCount; i++) {
        int idx = i;  // Capture for lambda
        _playerCards[i] = new PlayerCard(
            &_gameState.players[i],
            [this, idx]() { showKeyboard(idx); }
        );
    }
    layoutPlayerCards();
}

void MTGLifeScreen::destroyPlayerCards() {
    for (int i = 0; i < GameState::MAX_PLAYERS; i++) {
        if (_playerCards[i]) {
            delete _playerCards[i];
            _playerCards[i] = nullptr;
        }
    }
}

void MTGLifeScreen::layoutPlayerCards() {
    for (int i = 0; i < _gameState.playerCount; i++) {
        if (_playerCards[i]) {
            Rect r = getPlayerCardRect(i, _gameState.playerCount);
            _playerCards[i]->setBounds(r);
        }
    }
}

Rect MTGLifeScreen::getPlayerCardRect(int index, int playerCount) const {
    int16_t startY = Toolbar::HEIGHT + 4;
    int16_t availableH = 540 - startY - 4;
    int16_t availableW = 960 - 8;

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

void MTGLifeScreen::update() {
    _toolbar.update();

    // Auto-save periodically
    uint32_t now = millis();
    if (now - _lastSaveTime > SAVE_INTERVAL_MS) {
        saveState();
        _lastSaveTime = now;
    }
}

void MTGLifeScreen::draw(M5GFX* gfx) {
    if (needsFullRedraw()) {
        gfx->fillScreen(TFT_WHITE);
        setNeedsFullRedraw(false);
        _dirty = true;
    }

    // Draw toolbar
    _toolbar.setDirty(true);
    _toolbar.draw(gfx);

    // Draw player cards
    for (int i = 0; i < _gameState.playerCount; i++) {
        if (_playerCards[i]) {
            _playerCards[i]->setDirty(true);
            _playerCards[i]->draw(gfx);
        }
    }

    // Draw keyboard overlay if active
    if (_keyboard) {
        _keyboard->setDirty(true);
        _keyboard->draw(gfx);
    }

    _dirty = false;
}

bool MTGLifeScreen::handleTouch(int16_t x, int16_t y, bool pressed, bool released) {
    // Keyboard has priority when visible
    if (_keyboard) {
        return _keyboard->handleTouch(x, y, pressed, released);
    }

    // Check player cards
    for (int i = 0; i < _gameState.playerCount; i++) {
        if (_playerCards[i] && _playerCards[i]->handleTouch(x, y, pressed, released)) {
            _dirty = true;
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
    _keyboard = new Keyboard(
        _gameState.players[playerIndex].name,
        [this](const char* result, bool confirmed) {
            hideKeyboard(confirmed);
            if (confirmed && _editingPlayerIndex >= 0) {
                _gameState.players[_editingPlayerIndex].setName(result);
                saveState();
            }
        }
    );
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
