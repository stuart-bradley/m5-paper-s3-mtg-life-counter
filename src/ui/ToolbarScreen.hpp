#pragma once

#include "Screen.hpp"
#include "Toolbar.hpp"

class ToolbarScreen : public Screen {
   public:
    void update() override {
        _toolbar.update();
        onUpdate();
    }

    void draw(M5GFX* gfx) override {
        bool needsDisplay = false;

        if (needsFullRedraw()) {
            gfx->fillScreen(TFT_WHITE);
            setNeedsFullRedraw(false);
            _toolbar.setDirty(true);
            onFullRedraw(gfx);
            needsDisplay = true;
        }

        if (_toolbar.isDirty()) {
            _toolbar.draw(gfx);
            needsDisplay = true;
        }

        if (onDraw(gfx)) {
            needsDisplay = true;
        }

        if (needsDisplay) {
            gfx->display();
        }
    }

   protected:
    Toolbar _toolbar;

    virtual void onUpdate() {}
    virtual void onFullRedraw(M5GFX* gfx) { (void)gfx; }
    virtual bool onDraw(M5GFX* gfx) {
        (void)gfx;
        return false;
    }
};
