#pragma once

#include <M5GFX.h>
#include "../ui/Screen.hpp"

class ScreenManager {
   public:
    void registerScreen(Screen* screen);
    void setScreen(Screen* newScreen);
    void setScreenById(ScreenId id);
    void goBack();

    Screen* getCurrentScreen() { return _current; }
    Screen* getPreviousScreen() { return _previous; }
    ScreenId getCurrentScreenId();
    Screen* getScreenById(ScreenId id);

    void update();
    void draw(M5GFX* gfx);
    bool handleTouch(int16_t x, int16_t y, bool pressed, bool released);

   private:
    static constexpr int MAX_SCREENS = 8;
    Screen* _screens[MAX_SCREENS] = {nullptr};
    int _screenCount = 0;

    Screen* _current = nullptr;
    Screen* _previous = nullptr;
};
