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

    return _bounds.contains(x, y);
}
