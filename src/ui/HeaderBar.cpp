#include "HeaderBar.hpp"
#include "../utils/Sound.hpp"
#include "Layout.hpp"

HeaderBar::HeaderBar() {
    int16_t y = Toolbar::HEIGHT;
    _bounds = Rect(0, y, Layout::screenW(), HEIGHT);
    _leftButtonRect = Rect(Layout::BUTTON_MARGIN, y + (HEIGHT - Layout::BUTTON_H) / 2,
                           Layout::BUTTON_W, Layout::BUTTON_H);
    _rightButtonRect =
        Rect(Layout::screenW() - Layout::BUTTON_MARGIN - Layout::BUTTON_W,
             y + (HEIGHT - Layout::BUTTON_H) / 2, Layout::BUTTON_W, Layout::BUTTON_H);
}

void HeaderBar::setLeftButton(const char* label, std::function<void()> callback) {
    _leftLabel = label;
    _leftCallback = callback;
}

void HeaderBar::setRightButton(const char* label, std::function<void()> callback) {
    _rightLabel = label;
    _rightCallback = callback;
}

void HeaderBar::setToggle(const char* leftLabel, const char* rightLabel, bool state,
                          std::function<void(bool)> callback) {
    _hasToggle = true;
    _toggleLeftLabel = leftLabel;
    _toggleRightLabel = rightLabel;
    _toggleState = state;
    _toggleCallback = callback;

    // Position toggle right-justified, to the left of the right button
    static constexpr int16_t TOGGLE_W = 80;
    static constexpr int16_t TOGGLE_H = 32;
    int16_t toggleRight = _rightButtonRect.x - Layout::BUTTON_MARGIN;
    int16_t y = _bounds.y + (HEIGHT - TOGGLE_H) / 2;
    _toggleLeftRect = Rect(toggleRight - TOGGLE_W * 2, y, TOGGLE_W, TOGGLE_H);
    _toggleRightRect = Rect(toggleRight - TOGGLE_W, y, TOGGLE_W, TOGGLE_H);
}

void HeaderBar::setToggleState(bool state) {
    _toggleState = state;
    setDirty();
}

void HeaderBar::draw(M5GFX* gfx) {
    // Black header background
    gfx->fillRect(_bounds.x, _bounds.y, _bounds.w, _bounds.h, TFT_BLACK);

    // Left button (if set)
    if (_leftLabel) {
        Rect r = _leftButtonRect;
        gfx->drawRect(r.x, r.y, r.w, r.h, TFT_WHITE);
        gfx->setTextColor(TFT_WHITE);
        gfx->setTextDatum(MC_DATUM);
        gfx->setTextSize(1);
        gfx->drawString(_leftLabel, r.x + r.w / 2, r.y + r.h / 2);
    }

    // Centered title
    if (_title) {
        gfx->setTextColor(TFT_WHITE);
        gfx->setTextDatum(MC_DATUM);
        gfx->setTextSize(1);
        gfx->drawString(_title, Layout::centerX(), _bounds.y + HEIGHT / 2);
    }

    // Toggle (right-justified, next to right button)
    if (_hasToggle) {
        bool leftActive = !_toggleState;
        Rect lr = _toggleLeftRect;
        gfx->fillRect(lr.x, lr.y, lr.w, lr.h, leftActive ? TFT_WHITE : TFT_BLACK);
        gfx->drawRect(lr.x, lr.y, lr.w, lr.h, TFT_WHITE);
        gfx->setTextColor(leftActive ? TFT_BLACK : TFT_WHITE);
        gfx->setTextDatum(MC_DATUM);
        gfx->setTextSize(1);
        gfx->drawString(_toggleLeftLabel, lr.x + lr.w / 2, lr.y + lr.h / 2);

        bool rightActive = _toggleState;
        Rect rr = _toggleRightRect;
        gfx->fillRect(rr.x, rr.y, rr.w, rr.h, rightActive ? TFT_WHITE : TFT_BLACK);
        gfx->drawRect(rr.x, rr.y, rr.w, rr.h, TFT_WHITE);
        gfx->setTextColor(rightActive ? TFT_BLACK : TFT_WHITE);
        gfx->setTextDatum(MC_DATUM);
        gfx->setTextSize(1);
        gfx->drawString(_toggleRightLabel, rr.x + rr.w / 2, rr.y + rr.h / 2);
    }

    // Right button (if set)
    if (_rightLabel) {
        Rect r = _rightButtonRect;
        gfx->drawRect(r.x, r.y, r.w, r.h, TFT_WHITE);
        gfx->setTextColor(TFT_WHITE);
        gfx->setTextDatum(MC_DATUM);
        gfx->setTextSize(1);
        gfx->drawString(_rightLabel, r.x + r.w / 2, r.y + r.h / 2);
    }

    _dirty = false;
}

bool HeaderBar::handleTouch(int16_t x, int16_t y, bool pressed, bool released) {
    if (!released)
        return _bounds.contains(x, y);

    if (_leftLabel && _leftCallback && _leftButtonRect.contains(x, y)) {
        Sound::click();
        _leftCallback();
        return true;
    }

    if (_rightLabel && _rightCallback && _rightButtonRect.contains(x, y)) {
        Sound::click();
        _rightCallback();
        return true;
    }

    if (_hasToggle && _toggleCallback) {
        if (_toggleLeftRect.contains(x, y) && _toggleState) {
            Sound::click();
            _toggleState = false;
            _toggleCallback(false);
            return true;
        }
        if (_toggleRightRect.contains(x, y) && !_toggleState) {
            Sound::click();
            _toggleState = true;
            _toggleCallback(true);
            return true;
        }
    }

    return _bounds.contains(x, y);
}
