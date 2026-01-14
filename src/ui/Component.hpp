#pragma once

#include <cstdint>
#include <M5GFX.h>

struct Rect {
    int16_t x = 0;
    int16_t y = 0;
    int16_t w = 0;
    int16_t h = 0;

    Rect() = default;
    Rect(int16_t x_, int16_t y_, int16_t w_, int16_t h_)
        : x(x_), y(y_), w(w_), h(h_) {}

    bool contains(int16_t px, int16_t py) const {
        return px >= x && px < x + w && py >= y && py < y + h;
    }
};

class Component {
public:
    Component() = default;
    explicit Component(Rect bounds) : _bounds(bounds) {}
    virtual ~Component() = default;

    virtual void draw(M5GFX* gfx) = 0;
    virtual bool handleTouch(int16_t x, int16_t y, bool pressed, bool released) {
        (void)x; (void)y; (void)pressed; (void)released;
        return false;
    }

    bool contains(int16_t x, int16_t y) const {
        return _bounds.contains(x, y);
    }

    void setDirty(bool dirty = true) { _dirty = dirty; }
    bool isDirty() const { return _dirty; }

    Rect getBounds() const { return _bounds; }
    void setBounds(Rect bounds) { _bounds = bounds; _dirty = true; }

protected:
    Rect _bounds;
    bool _dirty = true;
};
