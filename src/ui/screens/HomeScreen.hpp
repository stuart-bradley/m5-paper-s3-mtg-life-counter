#pragma once

#include "../Screen.hpp"
#include "../Toolbar.hpp"

class ScreenManager;
class Screen;

class HomeScreen : public Screen {
   public:
    HomeScreen(ScreenManager* manager);

    void setMTGScreen(Screen* screen) { _mtgScreen = screen; }
    void setSettingsScreen(Screen* screen) { _settingsScreen = screen; }

    void onEnter() override;
    void draw(M5GFX* gfx) override;
    bool handleTouch(int16_t x, int16_t y, bool pressed, bool released) override;

   private:
    static constexpr int16_t CARD_WIDTH = 140;
    static constexpr int16_t CARD_HEIGHT = 140;
    static constexpr int16_t CARD_SPACING = 60;

    ScreenManager* _manager;
    Toolbar _toolbar;
    Screen* _mtgScreen = nullptr;
    Screen* _settingsScreen = nullptr;

    Rect getSettingsCardRect() const;
    Rect getMTGCardRect() const;

    void drawAppCard(M5GFX* gfx, Rect r, const uint8_t* icon, const char* label);
};
