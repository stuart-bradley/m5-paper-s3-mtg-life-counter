#pragma once

#include "../../assets/icons.hpp"
#include "../../ui/ToolbarScreen.hpp"

class App;

class HomeScreen : public ToolbarScreen {
   public:
    HomeScreen() = default;

    void onEnter() override;
    bool handleTouch(int16_t x, int16_t y, bool pressed, bool released) override;

   protected:
    void onFullRedraw(M5GFX* gfx) override;

   private:
    static constexpr int16_t CARD_WIDTH = 140;
    static constexpr int16_t CARD_HEIGHT = 140;
    static constexpr int16_t CARD_SPACING = 60;
    // ICON_SIZE comes from icons.hpp

    Rect getAppCardRect(int index, int totalApps) const;
    void drawAppCard(M5GFX* gfx, Rect r, const uint8_t* icon, const char* label);
};
