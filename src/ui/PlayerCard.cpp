#include "PlayerCard.hpp"
#include "../utils/Sound.hpp"
#include <Arduino.h>

PlayerCard::PlayerCard(Player* player, NameTapCallback onNameTap)
    : _player(player), _onNameTap(onNameTap) {
    if (_player) {
        _lastLife = _player->life;
    }
}

void PlayerCard::setPlayer(Player* player) {
    _player = player;
    if (_player) {
        _lastLife = _player->life;
    }
    setDirty();
}

Rect PlayerCard::getNameRect() const {
    return Rect(_bounds.x, _bounds.y, _bounds.w, NAME_HEIGHT);
}

Rect PlayerCard::getLifeRect() const {
    int16_t lifeY = _bounds.y + NAME_HEIGHT;
    int16_t lifeH = _bounds.h - NAME_HEIGHT - BUTTON_HEIGHT - BUTTON_MARGIN * 2;
    return Rect(_bounds.x, lifeY, _bounds.w, lifeH);
}

Rect PlayerCard::getButtonRect(int index) const {
    // Position buttons with margin from bottom and sides
    int16_t btnY = _bounds.y + _bounds.h - BUTTON_HEIGHT - BUTTON_MARGIN;
    int16_t availableW = _bounds.w - 2 * BUTTON_MARGIN;
    int16_t spacing = (availableW - 4 * BUTTON_WIDTH) / 3;  // 3 gaps between 4 buttons
    int16_t btnX = _bounds.x + BUTTON_MARGIN + index * (BUTTON_WIDTH + spacing);
    return Rect(btnX, btnY, BUTTON_WIDTH, BUTTON_HEIGHT);
}

void PlayerCard::drawButton(M5GFX* gfx, Rect r, const char* label) {
    // Draw button with 2px border for better visibility
    gfx->drawRect(r.x, r.y, r.w, r.h, TFT_BLACK);
    gfx->drawRect(r.x + 1, r.y + 1, r.w - 2, r.h - 2, TFT_BLACK);
    gfx->setTextDatum(MC_DATUM);
    gfx->setTextSize(2);  // Larger text for buttons
    gfx->drawString(label, r.x + r.w / 2, r.y + r.h / 2);
}

void PlayerCard::draw(M5GFX* gfx) {
    if (!_player) return;

    // Check if life changed
    if (_player->life != _lastLife) {
        _lastLife = _player->life;
        setDirty();
    }

    if (!isDirty()) return;

    // Background
    gfx->fillRect(_bounds.x, _bounds.y, _bounds.w, _bounds.h, TFT_WHITE);
    gfx->drawRect(_bounds.x, _bounds.y, _bounds.w, _bounds.h, TFT_BLACK);

    gfx->setTextColor(TFT_BLACK);

    // Name - same size as life text for readability
    Rect nameR = getNameRect();
    gfx->setTextDatum(MC_DATUM);
    gfx->setTextSize(4);
    gfx->drawString(_player->name, nameR.x + nameR.w / 2, nameR.y + nameR.h / 2);
    gfx->drawLine(nameR.x, nameR.y + nameR.h, nameR.x + nameR.w, nameR.y + nameR.h, TFT_BLACK);

    // Life total - LARGE
    Rect lifeR = getLifeRect();
    char lifeStr[8];
    snprintf(lifeStr, sizeof(lifeStr), "%d", _player->life);
    gfx->setTextDatum(MC_DATUM);
    gfx->setTextSize(4);  // Large text for life
    gfx->drawString(lifeStr, lifeR.x + lifeR.w / 2, lifeR.y + lifeR.h / 2);

    // Buttons
    gfx->setTextColor(TFT_BLACK);
    drawButton(gfx, getButtonRect(0), "-5");
    drawButton(gfx, getButtonRect(1), "-1");
    drawButton(gfx, getButtonRect(2), "+1");
    drawButton(gfx, getButtonRect(3), "+5");

    setDirty(false);
}

bool PlayerCard::handleTouch(int16_t x, int16_t y, bool pressed, bool released) {
    if (!_player || !contains(x, y)) return false;
    if (!released) return pressed;

    // Debounce
    uint32_t now = millis();
    if (now - _lastTouchTime < DEBOUNCE_MS) return true;
    _lastTouchTime = now;

    // Check name tap
    Rect nameR = getNameRect();
    if (nameR.contains(x, y)) {
        if (_onNameTap) {
            Sound::click();
            _onNameTap();
        }
        return true;
    }

    // Check buttons
    static const int16_t deltas[] = {-5, -1, 1, 5};
    for (int i = 0; i < 4; i++) {
        Rect btnR = getButtonRect(i);
        if (btnR.contains(x, y)) {
            _player->adjustLife(deltas[i]);
            if (deltas[i] > 0) {
                Sound::lifeUp();
            } else {
                Sound::lifeDown();
            }
            setDirty();
            return true;
        }
    }

    return true;
}
