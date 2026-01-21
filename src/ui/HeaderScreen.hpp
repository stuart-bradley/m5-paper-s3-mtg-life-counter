#pragma once

#include <functional>
#include "HeaderBar.hpp"
#include "ToolbarScreen.hpp"

class HeaderScreen : public ToolbarScreen {
   public:
    explicit HeaderScreen(const char* title) { _headerBar.setTitle(title); }

    bool handleTouch(int16_t x, int16_t y, bool pressed, bool released) override {
        if (_headerBar.handleTouch(x, y, pressed, released)) {
            return true;
        }
        return onTouch(x, y, pressed, released);
    }

   protected:
    HeaderBar _headerBar;

    void setLeftButton(const char* label, std::function<void()> cb) {
        _headerBar.setLeftButton(label, cb);
    }

    void setRightButton(const char* label, std::function<void()> cb) {
        _headerBar.setRightButton(label, cb);
    }

    void onFullRedraw(M5GFX* gfx) override {
        _headerBar.draw(gfx);
        onHeaderFullRedraw(gfx);
    }

    virtual void onHeaderFullRedraw(M5GFX* gfx) { (void)gfx; }
    virtual bool onTouch(int16_t x, int16_t y, bool pressed, bool released) {
        (void)x;
        (void)y;
        (void)pressed;
        (void)released;
        return false;
    }
};
