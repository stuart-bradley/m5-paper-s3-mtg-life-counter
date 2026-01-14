#pragma once

#include <functional>
#include "Component.hpp"

class Button : public Component {
public:
    using Callback = std::function<void()>;

    Button(Rect bounds, const char* label, Callback onClick);

    void draw(M5GFX* gfx) override;
    bool handleTouch(int16_t x, int16_t y, bool pressed, bool released) override;

    void setLabel(const char* label);
    const char* getLabel() const { return _label; }

private:
    static constexpr uint32_t DEBOUNCE_MS = 150;

    char _label[16] = "";
    Callback _onClick;
    bool _pressed = false;
    uint32_t _lastClickTime = 0;
};
