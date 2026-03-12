#include "PlayerCard.hpp"
#include <Arduino.h>
#include "../utils/Sound.hpp"

PlayerCard::PlayerCard(Player* player, NameTapCallback onNameTap)
    : _player(player), _onNameTap(onNameTap) {
    if (_player) {
        _lastLife = _player->life;
    }
}

void PlayerCard::checkDeltaExpiry() {
    if (_pendingDelta != 0 && millis() - _lastDeltaTime >= DELTA_DISPLAY_MS) {
        _pendingDelta = 0;
        setDirty();
    }
}

void PlayerCard::setPlayer(Player* player) {
    _player = player;
    _pendingDelta = 0;
    _lastDeltaTime = 0;
    if (_player) {
        _lastLife = _player->life;
    }
    setDirty();
}

void PlayerCard::setMode(CardMode mode) {
    if (_mode != mode) {
        _mode = mode;
        _pendingDelta = 0;
        _lastDeltaTime = 0;
        setDirty();
    }
}

void PlayerCard::setOpponents(OpponentInfo* opponents, uint8_t count, uint8_t playerIndex,
                              CmdrDamageCallback callback) {
    _opponentCount = count;
    _playerIndex = playerIndex;
    _cmdrCallback = callback;
    for (uint8_t i = 0; i < count && i < MAX_OPPONENTS; i++) {
        _opponents[i] = opponents[i];
    }
    setDirty();
}

Rect PlayerCard::getNameRect() const {
    int16_t h = (_mode == CardMode::CMDR) ? CMDR_NAME_HEIGHT : NAME_HEIGHT;
    return Rect(_bounds.x, _bounds.y, _bounds.w, h);
}

Rect PlayerCard::getLifeRect() const {
    if (useStackedLayout()) {
        int16_t lifeY = _bounds.y + NAME_HEIGHT;
        int16_t lifeH = _bounds.h - NAME_HEIGHT;
        int16_t lifeX = _bounds.x + STACK_BUTTON_WIDTH + BUTTON_MARGIN;
        int16_t lifeW = _bounds.w - 2 * (STACK_BUTTON_WIDTH + BUTTON_MARGIN);
        return Rect(lifeX, lifeY, lifeW, lifeH);
    } else {
        int16_t lifeY = _bounds.y + NAME_HEIGHT;
        int16_t lifeH = _bounds.h - NAME_HEIGHT - BUTTON_HEIGHT - BUTTON_MARGIN * 2;
        return Rect(_bounds.x, lifeY, _bounds.w, lifeH);
    }
}

Rect PlayerCard::getButtonRect(int index) const {
    if (useStackedLayout()) {
        int16_t btnW = STACK_BUTTON_WIDTH;
        int16_t btnH = STACK_BUTTON_HEIGHT;
        int16_t vertGap = 4;
        int16_t contentY = _bounds.y + NAME_HEIGHT + BUTTON_MARGIN;
        int16_t contentH = _bounds.h - NAME_HEIGHT - BUTTON_MARGIN * 2;
        int16_t centerY = contentY + contentH / 2;

        switch (index) {
            case 0:  // -5 bottom-left
                return Rect(_bounds.x + BUTTON_MARGIN, centerY + vertGap / 2, btnW, btnH);
            case 1:  // -1 top-left
                return Rect(_bounds.x + BUTTON_MARGIN, centerY - btnH - vertGap / 2, btnW, btnH);
            case 2:  // +1 top-right
                return Rect(_bounds.x + _bounds.w - BUTTON_MARGIN - btnW,
                            centerY - btnH - vertGap / 2, btnW, btnH);
            case 3:  // +5 bottom-right
            default:
                return Rect(_bounds.x + _bounds.w - BUTTON_MARGIN - btnW, centerY + vertGap / 2,
                            btnW, btnH);
        }
    } else {
        int16_t btnY = _bounds.y + _bounds.h - BUTTON_HEIGHT - BUTTON_MARGIN;
        int16_t availableW = _bounds.w - 2 * BUTTON_MARGIN;
        int16_t spacing = (availableW - 4 * BUTTON_WIDTH) / 3;
        int16_t btnX = _bounds.x + BUTTON_MARGIN + index * (BUTTON_WIDTH + spacing);
        return Rect(btnX, btnY, BUTTON_WIDTH, BUTTON_HEIGHT);
    }
}

void PlayerCard::drawButton(M5GFX* gfx, Rect r, const char* label) {
    gfx->drawRect(r.x, r.y, r.w, r.h, TFT_BLACK);
    gfx->drawRect(r.x + 1, r.y + 1, r.w - 2, r.h - 2, TFT_BLACK);
    gfx->setTextDatum(MC_DATUM);
    gfx->setTextSize(2);
    gfx->drawString(label, r.x + r.w / 2, r.y + r.h / 2);
}

void PlayerCard::draw(M5GFX* gfx) {
    if (!_player)
        return;

    if (_mode == CardMode::LIFE) {
        if (_player->life != _lastLife) {
            _lastLife = _player->life;
            setDirty();
        }
    }

    if (!isDirty())
        return;

    // Background
    gfx->fillRect(_bounds.x, _bounds.y, _bounds.w, _bounds.h, TFT_WHITE);
    gfx->drawRect(_bounds.x, _bounds.y, _bounds.w, _bounds.h, TFT_BLACK);

    if (_mode == CardMode::LIFE) {
        drawLifeMode(gfx);
    } else {
        drawCmdrMode(gfx);
    }

    setDirty(false);
}

void PlayerCard::drawLifeMode(M5GFX* gfx) {
    gfx->setTextColor(TFT_BLACK);

    // Name
    Rect nameR = getNameRect();
    gfx->setTextDatum(MC_DATUM);
    gfx->setTextSize(4);
    gfx->drawString(_player->name, nameR.x + nameR.w / 2, nameR.y + nameR.h / 2);
    gfx->drawLine(nameR.x, nameR.y + nameR.h, nameR.x + nameR.w, nameR.y + nameR.h, TFT_BLACK);

    // Life total
    Rect lifeR = getLifeRect();
    char lifeStr[8];
    snprintf(lifeStr, sizeof(lifeStr), "%d", _player->life);
    gfx->setTextDatum(MC_DATUM);
    gfx->setTextSize(4);
    int16_t lifeCenterY = lifeR.y + lifeR.h / 2;
    if (_pendingDelta != 0) {
        lifeCenterY -= 16;
    }
    gfx->drawString(lifeStr, lifeR.x + lifeR.w / 2, lifeCenterY);

    // Delta indicator
    if (_pendingDelta != 0) {
        char deltaStr[12];
        snprintf(deltaStr, sizeof(deltaStr), "[%+d]", _pendingDelta);
        gfx->setTextSize(2);
        gfx->drawString(deltaStr, lifeR.x + lifeR.w / 2, lifeCenterY + 38);
    }

    // Buttons
    gfx->setTextColor(TFT_BLACK);
    drawButton(gfx, getButtonRect(0), "-5");
    drawButton(gfx, getButtonRect(1), "-1");
    drawButton(gfx, getButtonRect(2), "+1");
    drawButton(gfx, getButtonRect(3), "+5");
}

void PlayerCard::getCmdrRowLayout(uint8_t row, int16_t& ry, int16_t& rowH, Rect& minusBtn,
                                  Rect& plusBtn) const {
    int16_t rowsY = _bounds.y + CMDR_NAME_HEIGHT;
    int16_t rowsH = _bounds.h - CMDR_NAME_HEIGHT;
    rowH = rowsH / _opponentCount;
    ry = rowsY + row * rowH;

    int16_t btnY = ry + (rowH - CMDR_BTN_H) / 2;
    int16_t minusBtnX = _bounds.x + _bounds.w - CMDR_BTN_W * 2 - CMDR_BTN_GAP * 2;
    int16_t plusBtnX = _bounds.x + _bounds.w - CMDR_BTN_W - CMDR_BTN_GAP;
    minusBtn = Rect(minusBtnX, btnY, CMDR_BTN_W, CMDR_BTN_H);
    plusBtn = Rect(plusBtnX, btnY, CMDR_BTN_W, CMDR_BTN_H);
}

void PlayerCard::drawCmdrMode(M5GFX* gfx) {
    gfx->setTextColor(TFT_BLACK);

    // Compact name header
    Rect nameR = getNameRect();
    gfx->setTextDatum(MC_DATUM);
    gfx->setTextSize(2);
    gfx->drawString(_player->name, nameR.x + nameR.w / 2, nameR.y + nameR.h / 2);
    gfx->drawLine(nameR.x, nameR.y + nameR.h, nameR.x + nameR.w, nameR.y + nameR.h, TFT_BLACK);

    if (_opponentCount == 0)
        return;

    for (uint8_t i = 0; i < _opponentCount; i++) {
        int16_t ry, rowH;
        Rect minusBtn, plusBtn;
        getCmdrRowLayout(i, ry, rowH, minusBtn, plusBtn);

        int16_t dmg = *_opponents[i].damage;
        bool lethal = dmg >= LETHAL_DAMAGE;

        uint16_t bg = lethal ? TFT_BLACK : TFT_WHITE;
        uint16_t fg = lethal ? TFT_WHITE : TFT_BLACK;

        gfx->fillRect(_bounds.x + 1, ry, _bounds.w - 2, rowH, bg);
        gfx->drawLine(_bounds.x, ry, _bounds.x + _bounds.w, ry, TFT_BLACK);

        gfx->setTextColor(fg);

        // Opponent name (left-aligned)
        gfx->setTextDatum(ML_DATUM);
        gfx->setTextSize(1);
        gfx->drawString(_opponents[i].name, _bounds.x + 8, ry + rowH / 2);

        // Damage value (centered)
        char dmgStr[8];
        snprintf(dmgStr, sizeof(dmgStr), "%d", dmg);
        gfx->setTextDatum(MC_DATUM);
        gfx->setTextSize(2);
        gfx->drawString(dmgStr, _bounds.x + _bounds.w / 2, ry + rowH / 2);

        // -1 / +1 buttons
        gfx->fillRect(minusBtn.x, minusBtn.y, minusBtn.w, minusBtn.h, fg);
        gfx->setTextColor(bg);
        gfx->setTextDatum(MC_DATUM);
        gfx->setTextSize(2);
        gfx->drawString("-1", minusBtn.x + minusBtn.w / 2, minusBtn.y + minusBtn.h / 2);

        gfx->fillRect(plusBtn.x, plusBtn.y, plusBtn.w, plusBtn.h, fg);
        gfx->setTextColor(bg);
        gfx->setTextDatum(MC_DATUM);
        gfx->setTextSize(2);
        gfx->drawString("+1", plusBtn.x + plusBtn.w / 2, plusBtn.y + plusBtn.h / 2);
    }

    // Delta indicator at bottom
    if (_pendingDelta != 0) {
        gfx->setTextColor(TFT_BLACK);
        gfx->setTextDatum(MC_DATUM);
        gfx->setTextSize(1);
        char deltaStr[12];
        snprintf(deltaStr, sizeof(deltaStr), "[%+d]", _pendingDelta);
        gfx->drawString(deltaStr, _bounds.x + _bounds.w / 2, _bounds.y + _bounds.h - 10);
    }
}

bool PlayerCard::handleTouch(int16_t x, int16_t y, bool pressed, bool released) {
    if (!_player || !contains(x, y))
        return false;
    if (!released)
        return pressed;

    uint32_t now = millis();
    if (now - _lastTouchTime < DEBOUNCE_MS)
        return true;
    _lastTouchTime = now;

    if (_mode == CardMode::LIFE) {
        return handleLifeTouch(x, y, now);
    } else {
        return handleCmdrTouch(x, y, now);
    }
}

bool PlayerCard::handleLifeTouch(int16_t x, int16_t y, uint32_t now) {
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
            _pendingDelta += deltas[i];
            _lastDeltaTime = now;
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

bool PlayerCard::handleCmdrTouch(int16_t x, int16_t y, uint32_t now) {
    if (_opponentCount == 0)
        return true;

    for (uint8_t i = 0; i < _opponentCount; i++) {
        int16_t ry, rowH;
        Rect minusR, plusR;
        getCmdrRowLayout(i, ry, rowH, minusR, plusR);

        if (y < ry || y >= ry + rowH)
            continue;

        if (minusR.contains(x, y)) {
            if (*_opponents[i].damage > 0) {
                if (_cmdrCallback) {
                    _cmdrCallback(_playerIndex, _opponents[i].sourceIndex, -1);
                }
                _pendingDelta -= 1;
                _lastDeltaTime = now;
                Sound::lifeDown();
                setDirty();
            }
            return true;
        }

        if (plusR.contains(x, y)) {
            if (_cmdrCallback) {
                _cmdrCallback(_playerIndex, _opponents[i].sourceIndex, 1);
            }
            _pendingDelta += 1;
            _lastDeltaTime = now;
            Sound::lifeUp();
            setDirty();
            return true;
        }

        return true;  // Touch was in row but not on a button
    }

    return true;
}
