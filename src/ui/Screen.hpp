#pragma once

#include <M5GFX.h>

class Screen {
   public:
    virtual ~Screen() = default;

    // Screen identification for save/restore
    virtual const char* screenId() const { return "main"; }

    // Lifecycle
    virtual void onEnter() {}
    virtual void onExit() {}

    // Frame update (called every loop iteration)
    virtual void update() {}

    // Drawing
    virtual void draw(M5GFX* gfx) = 0;

    // Touch handling - return true if touch was consumed
    virtual bool handleTouch(int16_t x, int16_t y, bool pressed, bool released) {
        (void)x;
        (void)y;
        (void)pressed;
        (void)released;
        return false;
    }

    void setNeedsFullRedraw(bool needs = true) { _needsFullRedraw = needs; }
    bool needsFullRedraw() const { return _needsFullRedraw; }

   protected:
    bool _needsFullRedraw = true;
};
