#pragma once

#include <M5GFX.h>
#include "../utils/Rect.hpp"

class Component {
   public:
    Component() = default;
    explicit Component(Rect bounds) : _bounds(bounds) {}
    virtual ~Component() = default;

    virtual void draw(M5GFX* gfx) = 0;
    virtual bool handleTouch(int16_t x, int16_t y, bool pressed, bool released) {
        (void)x;
        (void)y;
        (void)pressed;
        (void)released;
        return false;
    }

    bool contains(int16_t x, int16_t y) const { return _bounds.contains(x, y); }

    void setDirty(bool dirty = true) { _dirty = dirty; }
    bool isDirty() const { return _dirty; }

    Rect getBounds() const { return _bounds; }
    void setBounds(Rect bounds) {
        _bounds = bounds;
        _dirty = true;
    }

   protected:
    Rect _bounds;
    bool _dirty = true;
};
