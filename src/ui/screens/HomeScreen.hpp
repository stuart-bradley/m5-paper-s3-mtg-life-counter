#pragma once

#include "../Screen.hpp"
#include "../Toolbar.hpp"

class ScreenManager;

class HomeScreen : public Screen {
public:
    HomeScreen(ScreenManager* manager);

    void onEnter() override;
    void draw(M5GFX* gfx) override;
    bool handleTouch(int16_t x, int16_t y, bool pressed, bool released) override;

private:
    static constexpr int16_t CARD_WIDTH = 180;
    static constexpr int16_t CARD_HEIGHT = 180;
    static constexpr int16_t CARD_SPACING = 40;

    ScreenManager* _manager;
    Toolbar _toolbar;

    Rect getSettingsCardRect() const;
    Rect getMTGCardRect() const;

    void drawAppCard(M5GFX* gfx, Rect r, const uint8_t* icon, const char* label);
};
