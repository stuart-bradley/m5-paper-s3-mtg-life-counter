#pragma once

#include <functional>
#include "Component.hpp"

class Keyboard : public Component {
public:
    using Callback = std::function<void(const char* result, bool confirmed)>;

    Keyboard(const char* initialText, Callback onComplete);

    void draw(M5GFX* gfx) override;
    bool handleTouch(int16_t x, int16_t y, bool pressed, bool released) override;

private:
    static constexpr int16_t KEY_WIDTH = 75;
    static constexpr int16_t KEY_HEIGHT = 50;
    static constexpr int16_t KEY_SPACING = 6;
    static constexpr int16_t PREVIEW_HEIGHT = 44;
    static constexpr uint8_t MAX_TEXT_LEN = 15;

    char _buffer[MAX_TEXT_LEN + 1] = "";
    char _originalText[MAX_TEXT_LEN + 1] = "";
    uint8_t _cursorPos = 0;
    bool _shifted = true;  // Start with shift on for first letter
    Callback _onComplete;

    void appendChar(char c);
    void backspace();
    void complete(bool confirmed);

    Rect getKeyRect(int row, int col) const;
    char getKeyChar(int row, int col) const;
    const char* getKeyLabel(int row, int col) const;
};
