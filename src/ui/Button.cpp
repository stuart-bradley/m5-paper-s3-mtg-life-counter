#include "Button.hpp"
#include <Arduino.h>
#include <cstring>

Button::Button(Rect bounds, const char* label, Callback onClick)
    : Component(bounds), _onClick(onClick) {
    setLabel(label);
}

void Button::setLabel(const char* label) {
    strncpy(_label, label, sizeof(_label) - 1);
    _label[sizeof(_label) - 1] = '\0';
    setDirty();
}

void Button::draw(M5GFX* gfx) {
    if (!isDirty())
        return;

    int16_t x = _bounds.x;
    int16_t y = _bounds.y;
    int16_t w = _bounds.w;
    int16_t h = _bounds.h;

    if (_pressed) {
        // Inverted: black fill, white text
        gfx->fillRect(x, y, w, h, TFT_BLACK);
        gfx->setTextColor(TFT_WHITE);
    } else {
        // Normal: white fill, black border, black text
        gfx->fillRect(x, y, w, h, TFT_WHITE);
        gfx->drawRect(x, y, w, h, TFT_BLACK);
        gfx->setTextColor(TFT_BLACK);
    }

    // Center text
    gfx->setTextDatum(MC_DATUM);
    gfx->drawString(_label, x + w / 2, y + h / 2);

    setDirty(false);
}

bool Button::handleTouch(int16_t x, int16_t y, bool pressed, bool released) {
    if (!contains(x, y)) {
        if (_pressed) {
            _pressed = false;
            setDirty();
        }
        return false;
    }

    if (pressed && !_pressed) {
        _pressed = true;
        setDirty();
    }

    if (released && _pressed) {
        _pressed = false;
        setDirty();

        // Debounce check
        uint32_t now = millis();
        if (now - _lastClickTime >= DEBOUNCE_MS) {
            _lastClickTime = now;
            if (_onClick) {
                _onClick();
            }
        }
        return true;
    }

    return _pressed;
}
