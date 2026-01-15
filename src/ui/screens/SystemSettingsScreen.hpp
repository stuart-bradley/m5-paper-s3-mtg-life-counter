#pragma once

#include "../HeaderBar.hpp"
#include "../Screen.hpp"
#include "../Toolbar.hpp"

class ScreenManager;

class SystemSettingsScreen : public Screen {
   public:
    SystemSettingsScreen(ScreenManager* manager);

    void setHomeScreen(Screen* screen);

    void onEnter() override;
    void draw(M5GFX* gfx) override;
    bool handleTouch(int16_t x, int16_t y, bool pressed, bool released) override;

   private:
    ScreenManager* _manager;
    Screen* _homeScreen = nullptr;
    Toolbar _toolbar;
    HeaderBar _headerBar;
};
