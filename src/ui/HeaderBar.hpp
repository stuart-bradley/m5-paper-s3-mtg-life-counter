#pragma once

#include <M5GFX.h>
#include <functional>
#include "Component.hpp"
#include "Layout.hpp"
#include "Toolbar.hpp"

class HeaderBar : public Component {
   public:
    static constexpr int16_t HEIGHT = Layout::HEADER_H;
    // Button sizes come from Layout

    HeaderBar();

    void setTitle(const char* title) { _title = title; }
    void setLeftButton(const char* label, std::function<void()> callback);
    void setRightButton(const char* label, std::function<void()> callback);

    void draw(M5GFX* gfx) override;
    bool handleTouch(int16_t x, int16_t y, bool pressed, bool released) override;

   private:
    const char* _title = nullptr;
    const char* _leftLabel = nullptr;
    const char* _rightLabel = nullptr;
    std::function<void()> _leftCallback;
    std::function<void()> _rightCallback;
    Rect _leftButtonRect;
    Rect _rightButtonRect;
};
