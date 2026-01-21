#include "Keyboard.hpp"
#include <Arduino.h>
#include <cstring>
#include "../utils/Sound.hpp"
#include "Layout.hpp"

// Keyboard layout - letter mode
static const char* LETTER_ROW0 = "QWERTYUIOP";  // 10 keys + backspace
static const char* LETTER_ROW1 = "ASDFGHJKL'";  // 10 keys
static const char* LETTER_ROW2 = "#ZXCVBNM.^";  // 123 + 8 keys + shift (# = 123 button, ^ = shift)
// Row 3: SPACE, DONE, CANCEL (special handling)

// Keyboard layout - number/symbol mode
static const char* NUM_ROW0 = "1234567890";   // 10 keys + backspace
static const char* NUM_ROW1 = "-/:;()$&@\"";  // 10 keys
static const char* NUM_ROW2 =
    "#.,?!'*+=#";  // ABC + symbols (first # = ABC button, last position is also typeable)

Keyboard::Keyboard(const char* initialText, Callback onComplete) : _onComplete(onComplete) {
    strncpy(_buffer, initialText, MAX_TEXT_LEN);
    _buffer[MAX_TEXT_LEN] = '\0';
    strncpy(_originalText, initialText, MAX_TEXT_LEN);
    _originalText[MAX_TEXT_LEN] = '\0';
    _cursorPos = strlen(_buffer);

    // Size: full width, half screen height at bottom
    int16_t kbdH = Layout::screenH() / 2;
    _bounds = Rect(0, Layout::screenH() - kbdH, Layout::screenW(), kbdH);
}

void Keyboard::appendChar(char c) {
    if (_cursorPos >= MAX_TEXT_LEN)
        return;
    _buffer[_cursorPos++] = c;
    _buffer[_cursorPos] = '\0';
    _shifted = false;  // Auto-unshift after typing
    setDirty();
}

void Keyboard::backspace() {
    if (_cursorPos > 0) {
        _buffer[--_cursorPos] = '\0';
        setDirty();
    }
}

void Keyboard::complete(bool confirmed) {
    if (_onComplete) {
        _onComplete(confirmed ? _buffer : _originalText, confirmed);
    }
}

Rect Keyboard::getKeyRect(int row, int col) const {
    int16_t y = _bounds.y + PREVIEW_HEIGHT + row * (KEY_HEIGHT + KEY_SPACING);
    int16_t x;

    if (row == 3) {
        // Special row: SPACE (wide), DONE, CANCEL
        if (col == 0) {
            // SPACE - wide key
            return Rect(_bounds.x + 100, y, 400, KEY_HEIGHT);
        } else if (col == 1) {
            // DONE
            return Rect(_bounds.x + 520, y, 100, KEY_HEIGHT);
        } else {
            // CANCEL
            return Rect(_bounds.x + 640, y, 100, KEY_HEIGHT);
        }
    }

    // Regular rows - center the keyboard
    int numKeys = (row == 0) ? 11 : (row == 1) ? 10 : 10;  // row 0 has backspace
    int16_t rowWidth = numKeys * KEY_WIDTH + (numKeys - 1) * KEY_SPACING;
    int16_t startX = _bounds.x + (_bounds.w - rowWidth) / 2;

    x = startX + col * (KEY_WIDTH + KEY_SPACING);
    return Rect(x, y, KEY_WIDTH, KEY_HEIGHT);
}

char Keyboard::getKeyChar(int row, int col) const {
    const char* rowStr;
    if (_numMode) {
        rowStr = (row == 0) ? NUM_ROW0 : (row == 1) ? NUM_ROW1 : NUM_ROW2;
    } else {
        rowStr = (row == 0) ? LETTER_ROW0 : (row == 1) ? LETTER_ROW1 : LETTER_ROW2;
    }

    int len = strlen(rowStr);
    if (col >= len)
        return '\0';

    char c = rowStr[col];
    // Row 2 position 0 is always the mode toggle (123/ABC), not a char
    if (row == 2 && col == 0)
        return '\0';
    // Row 2 position 9 in letter mode is shift
    if (!_numMode && row == 2 && col == 9 && c == '^')
        return '\0';

    if (!_numMode && !_shifted && c >= 'A' && c <= 'Z') {
        return c + 32;  // lowercase
    }
    return c;
}

const char* Keyboard::getKeyLabel(int row, int col) const {
    static char label[2] = {0, 0};

    if (row == 0 && col == 10)
        return "<-";  // Backspace
    if (row == 3) {
        if (col == 0)
            return "SPACE";
        if (col == 1)
            return "DONE";
        if (col == 2)
            return "CANCEL";
    }
    // Mode toggle button (first key on row 2)
    if (row == 2 && col == 0) {
        return _numMode ? "ABC" : "123";
    }
    // Shift button (last key on row 2) - only in letter mode
    if (row == 2 && col == 9) {
        if (_numMode) {
            return "#+=";  // More symbols indicator
        }
        return _shifted ? "^" : "v";  // Shift indicator
    }

    char c = getKeyChar(row, col);
    if (c == '\0')
        return "";
    label[0] = c;
    return label;
}

void Keyboard::draw(M5GFX* gfx) {
    if (!isDirty())
        return;

    // Background - solid white to cover any ghosting
    gfx->fillRect(_bounds.x, _bounds.y, _bounds.w, _bounds.h, TFT_WHITE);
    gfx->drawRect(_bounds.x, _bounds.y, _bounds.w, _bounds.h, TFT_BLACK);
    gfx->drawRect(_bounds.x + 1, _bounds.y + 1, _bounds.w - 2, _bounds.h - 2, TFT_BLACK);

    gfx->setTextColor(TFT_BLACK);

    // Text preview - large
    gfx->setTextDatum(MC_DATUM);
    gfx->setTextSize(3);
    gfx->drawString(_buffer, _bounds.x + _bounds.w / 2, _bounds.y + PREVIEW_HEIGHT / 2);
    gfx->drawLine(_bounds.x, _bounds.y + PREVIEW_HEIGHT, _bounds.x + _bounds.w,
                  _bounds.y + PREVIEW_HEIGHT, TFT_BLACK);

    // Draw rows 0-2 (letter keys)
    gfx->setTextSize(2);  // Larger text for keys
    for (int row = 0; row < 3; row++) {
        int numCols = (row == 0) ? 11 : 10;
        for (int col = 0; col < numCols; col++) {
            Rect r = getKeyRect(row, col);
            const char* label = getKeyLabel(row, col);
            gfx->drawRect(r.x, r.y, r.w, r.h, TFT_BLACK);
            gfx->setTextDatum(MC_DATUM);
            gfx->drawString(label, r.x + r.w / 2, r.y + r.h / 2);
        }
    }

    // Draw row 3 (SPACE, DONE, CANCEL) - slightly smaller text to fit labels
    gfx->setTextSize(2);
    for (int col = 0; col < 3; col++) {
        Rect r = getKeyRect(3, col);
        const char* label = getKeyLabel(3, col);
        gfx->drawRect(r.x, r.y, r.w, r.h, TFT_BLACK);
        gfx->setTextDatum(MC_DATUM);
        gfx->drawString(label, r.x + r.w / 2, r.y + r.h / 2);
    }

    setDirty(false);
}

bool Keyboard::handleTouch(int16_t x, int16_t y, bool pressed, bool released) {
    if (!contains(x, y))
        return false;
    if (!released)
        return pressed;

    Sound::click();

    // Check rows 0-2
    for (int row = 0; row < 3; row++) {
        int numCols = (row == 0) ? 11 : 10;
        for (int col = 0; col < numCols; col++) {
            Rect r = getKeyRect(row, col);
            if (r.contains(x, y)) {
                if (row == 0 && col == 10) {
                    // Backspace
                    backspace();
                } else if (row == 2 && col == 0) {
                    // 123/ABC toggle
                    _numMode = !_numMode;
                    setDirty();
                } else if (row == 2 && col == 9 && !_numMode) {
                    // Shift (letter mode only)
                    _shifted = !_shifted;
                    setDirty();
                } else {
                    char c = getKeyChar(row, col);
                    if (c != '\0') {
                        appendChar(c);
                    }
                }
                return true;
            }
        }
    }

    // Check row 3
    for (int col = 0; col < 3; col++) {
        Rect r = getKeyRect(3, col);
        if (r.contains(x, y)) {
            if (col == 0) {
                appendChar(' ');
            } else if (col == 1) {
                complete(true);  // DONE
            } else {
                complete(false);  // CANCEL
            }
            return true;
        }
    }

    return true;
}
